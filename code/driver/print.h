/**
  ******************************************************************************
  * @file    print.h
  * @author  kindk
  * @version V1.0.0
  * @date    2017/03/21
  * @brief   Basic print function.
  
History: 
<author> <time> <version > <desc>
kindk  2017/03/21  1.0  build this moudle
  ****************************************************************************** 
 **/
#ifndef _PRINT_H
#define _PRINT_H
#include "stm32f10x.h"

/*	 Pin Configuration  */
#define PRINT_GPIO_CLK_ON	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)
#define PRINT_GPIO_GROUP	GPIOA
#define PRINT_GPIO_TXD_PIN	GPIO_Pin_2
#define PRINT_GPIO_RXD_PIN	GPIO_Pin_3

/*	Uart Configuration	 */
#define PRINT_UART_CLK_ON	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE)
#define PRINT_UART_GROUP	USART2
#define PRINT_BAUDRATE		115200

void print_init(void);
#endif


