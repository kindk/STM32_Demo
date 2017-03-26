#ifndef _GPIO_H
#define _GPIO_H
#include "stm32f10x.h"

void gpio_clk_on(GPIO_TypeDef *gpio_group);
u8 gpio_group_to_portsource(GPIO_TypeDef *gpio_group);
u8 gpio_pin_to_pinsource(u16 pin);


#endif
