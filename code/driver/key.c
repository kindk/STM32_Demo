#define LOG_TAG "key"
#include "log.h"

#include "key.h"
#include "gpio.h"
#include "clk.h" //ut1ms
#include "irq.h"

#define KEY_PERIOD 10	//10ms
#define DEFAULT_JITTER_MS 100
#define DEFAULT_LONG_PRESS_MS 2000
//TODO: should use list to add all registered key, and scan every key 
// in state_machine.
const char *key_action_string[KEY_ACTION_NUM] = {
    "NONE",
    "UP",
    "DOWN",
    "LONG"
};

void key_gpio_init(key_t *key);

void key_press(key_t *key)
{
	key_event_t event = {
		.id = key->id,
		.action = KEY_ACTION_DOWN
	};
    key->fifo->in(key->fifo, (u8 *)&event, sizeof(key_event_t));
    LOGD(" ");
}

void key_release(key_t *key)
{
	key_event_t event = {
		.id = key->id,
		.action = KEY_ACTION_UP
	};
    key->fifo->in(key->fifo, (u8 *)&event, sizeof(key_event_t));
    LOGD(" ");
}

void key_long_press(key_t *key)
{
	key_event_t event = {
		.id = key->id,
		.action = KEY_ACTION_LONG
	};
    key->fifo->in(key->fifo, (u8 *)&event, sizeof(key_event_t));
    LOGD(" ");
}

void key_init_fifo(fifo_t *fifo)
{
    fifo_init(fifo, 20, 2);
}

int key_init(key_t *key, u8 id, GPIO_TypeDef *gpio_group, u16 pin, u8 active, u16 jitter_ms, fifo_t *fifo)
{
    if (!key) {
        LOGE("null");
        return -EINVAL;
    }

    //TODO: check whether id has been occupied in the global key list.
    //if(id is occupied)

    //TODO: gpio_group and pin should checked in lower layer, cpu specified.
    if (!gpio_group) {
        LOGE("null");
        return -EINVAL;
    }

    if (0 != active && 1 != active) {
        LOGE("wrong value");
        return -EINVAL;
    }

    if (jitter_ms > 1000) {
        LOGE("wrong value");
        return -EINVAL;
    }

    if (!fifo) {
        LOGE("null");
        return -EINVAL;
    }

    key->id = id;
    key->gpio_group = gpio_group;
    key->pin = pin;
	key->active = active;
    if (jitter_ms) {
        key->jitter_time = jitter_ms;
    } else {
        key->jitter_time = DEFAULT_JITTER_MS;
    }
    key_init_fifo(fifo);
    key->fifo = fifo;
    key_gpio_init(key);
	key->state = KEY_INITIALIZED;
    key->flag = KEY_IRQ_FLAG_NONE;
    key->long_press_time = DEFAULT_LONG_PRESS_MS;
    key->wait_timer = 0;

    key->press = key_press;
    key->release = key_release;
    key->long_press = key_long_press;
	
	return 0;
}

void key_gpio_init(key_t *key)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    u8 pinsource;

    gpio_clk_on(key->gpio_group);
    RCC_AFIO_ON; //for exti line

    GPIO_InitStructure.GPIO_Pin = key->pin;
    if (key->active) {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    } else {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    }
    GPIO_Init(key->gpio_group, &GPIO_InitStructure);

    pinsource = gpio_pin_to_pinsource(key->pin);
    GPIO_EXTILineConfig(gpio_group_to_portsource(key->gpio_group), pinsource);

    if (pinsource < 5) {
        key->irq = (enum IRQn)(pinsource + EXTI0_IRQn);
    } else if (pinsource < 10) {
        key->irq = EXTI9_5_IRQn;
    } else if (pinsource < 16) {
        key->irq = EXTI15_10_IRQn;
    }
    
    NVIC_InitStructure.NVIC_IRQChannel = key->irq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void key_irq_open(key_t *key, u8 wait_press)
{
    EXTI_InitTypeDef EXTI_InitStructure;

  	EXTI_InitStructure.EXTI_Line = key->pin;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    if (wait_press) {
        if (key->active) {
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;            
        } else {
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;            
        }
    } else {
        if (key->active) {
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;            
        } else {
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;            
        }
    }
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);
}

void key_irq_close(key_t *key)
{
    *(__IO uint32_t *) EXTI_BASE &= ~(key->pin);
}

void key_irq_handler(key_t *key)
{
    if (EXTI_GetITStatus(key->pin) == SET) {
        key->irq_flag = 1;
        EXTI_ClearITPendingBit(key->pin);  
        key_irq_close(key);
    } else {
    //TODO: should not run here, but if run here, must clear the pending bit.
        LOGE("yue %x", EXTI->PR);
    }
}

static u8 key_is_pressed(key_t *key)
{
    u8 val = GPIO_ReadInputDataBit(key->gpio_group, key->pin);
    return (key->active ? val : !val);
}

//FIXME: should not put user's callback directly in state_machine,
//If user block the state_machine, will lose many key.
//Should use key_fifo store key event
void key_state_machine(key_t *key)
{	
	switch(key->state){
		case KEY_INITIALIZED:
            key_irq_open(key, 1);
			key->state = KEY_RELEASED;
            LOGD("INITIALIZED -> RELEASED");
		case KEY_RELEASED:
            if (key->irq_flag) {
                
                key_irq_disable(key);
                key->irq_flag = 0;
                key_irq_enable(key);
                
                key->pressed_time = 0;
                key->released_time = 0;
    			key->state = KEY_PRESS_JITTER;
                LOGD("RELEASED -> PRESS_JITTER");
            }
			break;
		case KEY_PRESS_JITTER:
            if (key_is_pressed(key)){
                key->pressed_time += KEY_PERIOD;
                key->released_time = 0;
            } else {
                key->released_time += KEY_PERIOD;
                key->pressed_time = 0;
            }
            
            if (key->pressed_time > key->jitter_time) {
                if (key->press) {
                    key->press(key);
                }
                key_irq_open(key, 0);
                //TODO: timer
                if (key->long_press_time) {
                    //start a timer of key->long_press_time
                    key->expired_time = ut1ms + key->long_press_time;
                    key->wait_timer = 1;
                }
                key->state = KEY_PRESSED;
                LOGD("PRESS_JITTER -> PRESSED");
            } else if (key->released_time > key->jitter_time) {
                if (key->press) {
                    key->press(key);
                }
                
                if (key->release) {
                    key->release(key);
                }
                key->state = KEY_INITIALIZED;
                LOGD("PRESS_JITTER -> INITIALIZED");
            }
			break;
		case KEY_PRESSED:
            if (key->irq_flag) {
                
                key_irq_disable(key);
                key->irq_flag = 0;
                key_irq_enable(key);
                
                key->pressed_time = 0;
                key->released_time = 0;
    			key->state = KEY_RELEASE_JITTER;
                LOGD("PRESSED -> RELEASE_JITTER");
            }
            
            if (key->expired_time <= ut1ms && key->wait_timer) {
                key->wait_timer = 0;
                if (key->long_press) {
                    key->long_press(key);
                }
            }
			break;
		case KEY_RELEASE_JITTER:
            if (key_is_pressed(key)){
                key->pressed_time += KEY_PERIOD;
                key->released_time = 0;
            } else {
                key->released_time += KEY_PERIOD;
                key->pressed_time = 0;
            }
            
            if (key->pressed_time > key->jitter_time) {
                LOGE("THIS should not happen");
                key_irq_open(key, 0);
                if (key->long_press_time) {
                    //resume timer
                }
                key->state = KEY_PRESSED;
                LOGD("RELEASE_JITTER -> PRESSED");
            } else if (key->released_time > key->jitter_time) {
                if (key->release) {
                    key->release(key);
                }
                key->state = KEY_INITIALIZED;
                LOGD("RELEASE_JITTER -> INITIALIZED");
            }
			break;
		default:
			break;
	}
}
