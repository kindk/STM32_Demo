#ifndef _KEY_H
#define _KEY_H
#include "type.h"
#include "stm32f10x.h"
#include "fifo.h"

typedef enum{
	KEY_INITIALIZED = 1,
	KEY_RELEASED,
	KEY_PRESS_JITTER,
	KEY_PRESSED,
	KEY_RELEASE_JITTER,
}key_state_t;

typedef enum{
    KEY_IRQ_FLAG_NONE = 0,
    KEY_IRQ_FLAG_PRESS,
    KEY_IRQ_FLAG_RELEASE,
    KEY_IRQ_FLAG_TIMER,
}key_irq_flag;

typedef struct _key key_t;
struct _key{
    u8 id;
    GPIO_TypeDef *gpio_group;
    u16 pin;
    u8 active;
    u32 jitter_time;
    
    enum IRQn irq;
    vu32 irq_flag;
    
	key_state_t state;
    key_irq_flag flag;
    u32 pressed_time;
    u32 released_time;
    u32 long_press_time;
    u32 expired_time;

    //u8 wait_press;
    u8 wait_timer;

    fifo_t *fifo;

    void (*press)(key_t *key);
    void (*release)(key_t *key);
    void (*long_press)(key_t *key);
};

typedef enum {
    KEY_ACTION_NONE = 0,
    KEY_ACTION_UP,
    KEY_ACTION_DOWN,
    KEY_ACTION_LONG,
    KEY_ACTION_NUM
}key_action_t;

typedef struct _key_event{
    u8 id;
    u8 action;
}key_event_t;

#define key_irq_disable(key) NVIC_DisableIRQ(key->irq) 
#define key_irq_enable(key) NVIC_EnableIRQ(key->irq) 

extern const char *key_action_string[KEY_ACTION_NUM];

int key_init(key_t *key, u8 id, GPIO_TypeDef *gpio_group, u16 pin, u8 active, u16 jitter_ms, fifo_t *fifo);
void key_irq_handler(key_t *key);
void key_state_machine(key_t *key);


#endif
