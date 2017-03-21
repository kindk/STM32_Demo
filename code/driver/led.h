/**
******************************************************************************
* @file    Led.h
* @author  kindk
* @version V1.0.0
* @date    24-07-2015
* @brief   Led operation. 

History: 
<author> <time> <version > <desc>
kindk  05-07-24  1.0  build this moudle
******************************************************************************
**/

#ifndef _LED_H
#define _LED_H

#include "stm32f10x.h"
#include "clk.h"

#define LED_ACTIVE_HIGH	1

typedef struct _led led_t;
struct _led{
	GPIO_TypeDef *gpio_group;
	u16 pin;
	void (*on)(led_t *led);
	void (*off)(led_t *led);
	void (*toggle)(led_t *led);
};

void led_init(led_t *led, GPIO_TypeDef *gpio_group, u32 pin);
void led_test(led_t *led);

#endif
