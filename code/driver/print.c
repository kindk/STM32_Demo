/**
  ******************************************************************************
  * @file    print.c
  * @author  kindk
  * @version V1.0.0
  * @date    2017/03/21
  * @brief   Basic print function.
  
History: 
<author> <time> <version > <desc>
kindk  2017/03/21  1.0  build this moudle
  ****************************************************************************** 
 **/
#define LOG_TAG "print"
#include "log.h"
#include "print.h"
#include "config.h"

static void print_gpio_init(void);
static void print_uart_init(void);

void print_init(void)
{
	print_gpio_init();
	print_uart_init();
	LOGE("Welcome to STM32!");
}

static void print_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	PRINT_GPIO_CLK_ON;
	
	GPIO_InitStructure.GPIO_Pin = PRINT_GPIO_TXD_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PRINT_GPIO_GROUP,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PRINT_GPIO_RXD_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PRINT_GPIO_GROUP,&GPIO_InitStructure);	
}

static void print_uart_init(void)
{
	USART_InitTypeDef USART_InitStructure;

	PRINT_UART_CLK_ON;
	
	USART_InitStructure.USART_BaudRate = PRINT_BAUDRATE;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_Init(PRINT_UART_GROUP,&USART_InitStructure);
	
	USART_ClearFlag(PRINT_UART_GROUP, USART_FLAG_TC);
	USART_Cmd(PRINT_UART_GROUP,ENABLE);
}

void prta(void *array, u32 len)
{
    int i;
	u8* ptr = (u8 *)array;
    if (!array) {
        LOGE("array is null");
        return;
    }

    if (len > 65535) {
        LOGE("array is tooo long");
        return;
    }
        
    for (i = 0;i < len; i++)
        _PRINT("%x ", *ptr++);
    
    _PRINT("\r\n");
}

#if CONFIG_USE_PRINTF
int fputc(int ch,FILE *f)
{
	while(USART_GetFlagStatus(PRINT_UART_GROUP,USART_FLAG_TXE)==RESET);	
	USART_SendData(PRINT_UART_GROUP,(u8)ch);
	return ch;
}
#endif
