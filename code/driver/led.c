/**
******************************************************************************
* @file    led.c
* @author  kindk
* @version V1.0.0
* @date    2017/03/21
* @brief   Led operation. 

History: 
<author> <time> <version > <desc>
kindk  2017/03/21  1.0  build this moudle
******************************************************************************
**/
#include "led.h"

static void led_on(led_t *led);
static void led_off(led_t *led);
static void led_toggle(led_t *led);
//TODO use gpio.h
void led_init(led_t *led, GPIO_TypeDef *gpio_group, u16 pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	led->gpio_group = gpio_group;
	led->pin = pin;
	led->on = led_on;
	led->off = led_off;
	led->toggle = led_toggle;
	
	if (GPIOA == gpio_group)
		RCC_GPIOA_ON;
	else if (GPIOB == gpio_group)
		RCC_GPIOB_ON;
	else if (GPIOC == gpio_group)
		RCC_GPIOC_ON;
	else if (GPIOD == gpio_group)
		RCC_GPIOD_ON;
	else if (GPIOE == gpio_group)
		RCC_GPIOE_ON;
	else if (GPIOF == gpio_group)
		RCC_GPIOF_ON;
	else if (GPIOG == gpio_group)
		RCC_GPIOG_ON;
	
	GPIO_InitStructure.GPIO_Pin = led->pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(led->gpio_group, &GPIO_InitStructure);
}

static void led_on(led_t *led)
{
#if LED_ACTIVE_HIGH
	GPIO_SetBits(led->gpio_group, led->pin);
#else	
	GPIO_ResetBits(led->gpio_group, led->pin);
#endif
}

static void led_off(led_t *led)
{
#if LED_ACTIVE_HIGH
	GPIO_ResetBits(led->gpio_group, led->pin);
#else	
	GPIO_SetBits(led->gpio_group, led->pin);
#endif
}

static void led_toggle(led_t *led)
{
	static int state = 0;
	if (state) {
		led->on(led);
		state = 0;
	} else {
		led->off(led);
		state = 1;
	}
}

void led_delay(void)
{
	int a = 10000000;
	while(a--);
}

void led_test(led_t *led)
{
	led->on(led);
	led_delay();
	led->off(led);
	led_delay();
}
