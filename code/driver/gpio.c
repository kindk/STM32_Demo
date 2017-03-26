#include "gpio.h"
#include "clk.h"

void gpio_clk_on(GPIO_TypeDef *gpio_group)
{
    switch(gpio_group_to_portsource(gpio_group))
    {
        case GPIO_PortSourceGPIOA:
            RCC_GPIOA_ON;
        case GPIO_PortSourceGPIOB:
            RCC_GPIOB_ON;
        case GPIO_PortSourceGPIOC:
            RCC_GPIOC_ON;
        case GPIO_PortSourceGPIOD:
            RCC_GPIOD_ON;
        case GPIO_PortSourceGPIOE:
            RCC_GPIOE_ON;
        case GPIO_PortSourceGPIOF:
            RCC_GPIOF_ON;
        case GPIO_PortSourceGPIOG:
            RCC_GPIOG_ON;
    }
}

u8 gpio_group_to_portsource(GPIO_TypeDef *gpio_group)
{
    if (GPIOA == gpio_group)
        return GPIO_PortSourceGPIOA;
	else if (GPIOB == gpio_group)
        return GPIO_PortSourceGPIOB;
	else if (GPIOC == gpio_group)
        return GPIO_PortSourceGPIOC;
	else if (GPIOD == gpio_group)
        return GPIO_PortSourceGPIOD;
	else if (GPIOE == gpio_group)
        return GPIO_PortSourceGPIOE;
	else if (GPIOF == gpio_group)
        return GPIO_PortSourceGPIOF;
	else if (GPIOG == gpio_group)
        return GPIO_PortSourceGPIOG;
}

u8 gpio_pin_to_pinsource(u16 pin)
{
    switch(pin)
    {
        case GPIO_Pin_0:
            return GPIO_PinSource0;
        case GPIO_Pin_1:
            return GPIO_PinSource1;
        case GPIO_Pin_2:
            return GPIO_PinSource2;
        case GPIO_Pin_3:
            return GPIO_PinSource3;
        case GPIO_Pin_4:
            return GPIO_PinSource4;
        case GPIO_Pin_5:
            return GPIO_PinSource5;
        case GPIO_Pin_6:
            return GPIO_PinSource6;
        case GPIO_Pin_7:
            return GPIO_PinSource7;
        case GPIO_Pin_8:
            return GPIO_PinSource8;
        case GPIO_Pin_9:
            return GPIO_PinSource9;
        case GPIO_Pin_10:
            return GPIO_PinSource10;
        case GPIO_Pin_11:
            return GPIO_PinSource11;
        case GPIO_Pin_12:
            return GPIO_PinSource12;
        case GPIO_Pin_13:
            return GPIO_PinSource13;
        case GPIO_Pin_14:
            return GPIO_PinSource14;
        case GPIO_Pin_15:
            return GPIO_PinSource15;
        default:
            return 0;
    }
}
