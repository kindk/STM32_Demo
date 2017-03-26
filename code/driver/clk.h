#ifndef _CLK_H
#define _CLK_H
#include "stm32f10x.h"
#include "stdio.h"
/*
 *  Chose: printf or utPrt
 *  Must init print function!
*/

/*  clk tick configuration  */
typedef struct
{
	uint8_t UIF :	1;
	uint8_t CC1IF : 1;
	uint8_t CC2IF : 1;
	uint8_t CC3IF : 1;
	uint8_t b : 	1;
	uint8_t TIF :	1;
	uint8_t c : 	1;
	uint8_t d : 	1;
} TIM_SR;

#define CLK_TICK_TIM_GROUP	TIM4
#define CLK_TICK_TIM_CLK_ON	RCC_TIM4_ON
#define TIM4_SR 	((TIM_SR*)(TIM4_BASE + 0x10))
#define ut1ms  		(TIM4->CNT)
#define FlagTim	  	(TIM4_SR->UIF)

#define utNow1ms(x)	((u16)(ut1ms-x))

#define RCC_AFIO_ON  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE)
#define RCC_GPIOA_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)
#define RCC_GPIOB_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE)
#define RCC_GPIOC_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)
#define RCC_GPIOD_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE)
#define RCC_GPIOE_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE)
#define RCC_GPIOF_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE)
#define RCC_GPIOG_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE)

#define RCC_UART1_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define RCC_UART2_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE)
#define RCC_UART3_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE)
#define RCC_UART4_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE)
#define RCC_UART5_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE)

#define RCC_UART1_OFF RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE)
#define RCC_UART2_OFF RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE)
#define RCC_UART3_OFF RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE)
#define RCC_UART4_OFF RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, DISABLE)
#define RCC_UART5_OFF RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, DISABLE)

#define RCC_TIM1_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE)
#define RCC_TIM2_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE)
#define RCC_TIM3_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE)
#define RCC_TIM4_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE)
#define RCC_TIM5_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE)
#define RCC_TIM6_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE)
#define RCC_TIM7_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE)
#define RCC_TIM8_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE)
#define RCC_TIM9_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE)
#define RCC_TIM10_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE)
#define RCC_TIM11_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE)
#define RCC_TIM12_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE)
#define RCC_TIM13_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE)
#define RCC_TIM14_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE)
#define RCC_TIM15_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM15, ENABLE)
#define RCC_TIM16_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM16, ENABLE)
#define RCC_TIM17_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM17, ENABLE)

#define RCC_ADC1_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE)
#define RCC_ADC2_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE)
#define RCC_ADC3_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE)

#define RCC_SPI1_ON RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE)
#define RCC_SPI2_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE)
#define RCC_SPI3_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE)

#define RCC_I2C1_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE)
#define RCC_I2C2_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE)


#define RCC_CAN1_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE)

#define RCC_DAC_ON RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE)

#define RCC_DMA1_ON RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE)
#define RCC_DMA2_ON RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE)


void clk_print(void);
void clk_tick_init(void);
void clk_tick_run(void);
int clk_test(void);

#endif
