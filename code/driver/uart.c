#define LOG_TAG "uart"
#include "log.h"

#define UART_DEBUG 1
#include "uart.h"
#include "clk.h"
#include "common.h"
#include "stdlib.h"

//FIXME: read should not use callback, if user block the 
//read, which will block driver layer.
static void uart_clear_idle_flag(uart_t *t);
static int uart_dma_write(uart_t *uart, u32 add, u8 len);

u32 _add;
u8 _size;

void uart_write_daemon(uart_t *uart)
{
	if (uart->tx_flag) {
		uart->tx_flag = 0;
		uart->tx_fifo.get_out(&uart->tx_fifo, &_add, &_size);
        if (uart_dma_write(uart, _add, _size)) {
            LOGE("dma write error");
        }
	}
}

void uart_read_daemon(uart_t *uart)
{
#if UART_DEBUG
    u8 cnt = 0;
    u8 *data;
#endif
	if (uart->rx_flag) {
		uart->rx_flag = 0;

        while(!uart->rx_fifo.is_empty(&uart->rx_fifo)) {
            uart->rx_fifo.get_out(&uart->rx_fifo, &_add, &_size);
            if (uart->read_callback) {
                uart->read_callback(uart, (u8 *)_add, _size);
            }
#if UART_DEBUG
            cnt ++;
            data = (u8 *)_add;
            data[_size] = '\0';
            LOGD("len: %d, cnt: %d %s",_size, cnt, data);
#endif
        }
	}
}

void uart_write(uart_t *uart, u8 *data, u32 len)
{
    if (uart->tx_fifo.is_empty(&uart->tx_fifo)) {
        if (!uart_dma_write(uart, (u32)data, len)) {
            goto _prt;
        }
    }
    uart->tx_fifo.in(&uart->tx_fifo, data, len);
    uart->tx_flag = 1;    

_prt:
    LOGD("len: %d data: ", len);
    prta(data, len);
}

void uart_set_read_callback(uart_t *uart, void (*read_callback)(uart_t *uart, u8 *data, u8 len))
{
    uart->read_callback = read_callback;
}

void uart_open(uart_t *uart);
static void uart_gpio_init(uart_t *uart);
static void uart_gpio_uninit(uart_t *uart);
static void uart_usart_init(uart_t *uart);
static void uart_usart_uninit(uart_t *uart);
static void uart_dma_init(uart_t *uart);
static void uart_dma_uninit(uart_t *uart);

void uart_init(uart_t *uart, USART_TypeDef *uart_group, u32 baudrate, u32 tx_fifo_len, u32 tx_data_len, u32 rx_fifo_len, u32 rx_data_len)
{
	uart->baudrate = baudrate;
	uart->uart_group = uart_group;
    uart->write_daemon = uart_write_daemon;
    uart->read_daemon = uart_read_daemon;
    uart->write = uart_write;
    uart->set_read_callback = uart_set_read_callback;
    uart->dma.rx_buffer = malloc(tx_data_len);
    uart->dma.rx_buffer_len = tx_data_len;
    uart->dma.tx_lock = 1;
    if (!uart->dma.rx_buffer) {
        LOGD("uart_init: no mem for uart.dma.rx_buffer: %d\r\n", tx_data_len);
    }
	fifo_init(&uart->tx_fifo, tx_fifo_len, tx_data_len);
	fifo_init(&uart->rx_fifo, rx_fifo_len, rx_data_len);
	uart_open(uart);
}

void uart_open(uart_t *uart)
{
	
	uart->opened = 0;
	uart_gpio_init(uart);
	uart_usart_init(uart);
	uart_dma_init(uart);
	
	uart->opened = 1;
}

void uart_close(uart_t *uart)
{
	if (!uart->opened) {
		LOGD("uart_gpio_uninit: uart has not been initialized!");
		return;
	}
	uart_gpio_uninit(uart);
	uart_usart_uninit(uart);
	uart_dma_uninit(uart);
	uart->opened = 0;
}

static void uart_gpio_init(uart_t *uart)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	if (USART1 == uart->uart_group) {
		RCC_GPIOA_ON;
		uart->gpio.tx_group = GPIOA;
		uart->gpio.rx_group = GPIOA;
		uart->gpio.tx_pin = GPIO_Pin_9;
		uart->gpio.rx_pin = GPIO_Pin_10;
	} else if (USART2 == uart->uart_group) {
		RCC_GPIOA_ON;
		uart->gpio.tx_group = GPIOA;
		uart->gpio.rx_group = GPIOA;
		uart->gpio.tx_pin = GPIO_Pin_2;
		uart->gpio.rx_pin = GPIO_Pin_3;
	} else if (USART3 == uart->uart_group) {
		RCC_GPIOB_ON;
		uart->gpio.tx_group = GPIOB;
		uart->gpio.rx_group = GPIOB;
		uart->gpio.tx_pin = GPIO_Pin_10;
		uart->gpio.rx_pin = GPIO_Pin_11;
	} else if (UART4 == uart->uart_group) {
		RCC_GPIOC_ON;
		uart->gpio.tx_group = GPIOC;
		uart->gpio.rx_group = GPIOC;
		uart->gpio.tx_pin = GPIO_Pin_10;
		uart->gpio.rx_pin = GPIO_Pin_11;
	} else if (UART5 == uart->uart_group) {
		RCC_GPIOC_ON;
		RCC_GPIOD_ON;
		uart->gpio.tx_group = GPIOC;
		uart->gpio.rx_group = GPIOD;
		uart->gpio.tx_pin = GPIO_Pin_12;
		uart->gpio.rx_pin = GPIO_Pin_2;
	}

	GPIO_InitStructure.GPIO_Pin 	= uart->gpio.tx_pin;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(uart->gpio.tx_group,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin 	= uart->gpio.rx_pin;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	GPIO_Init(uart->gpio.rx_group, &GPIO_InitStructure);
    
    LOGD("OK");
}

static void uart_gpio_uninit(uart_t *uart)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin 	= uart->gpio.tx_pin;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	GPIO_Init(uart->gpio.tx_group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin 	= uart->gpio.rx_pin;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	GPIO_Init(uart->gpio.rx_group, &GPIO_InitStructure);
}

static void uart_usart_init(uart_t *uart)
{
	USART_InitTypeDef 	USART_InitStructure;
	NVIC_InitTypeDef  	NVIC_InitStructure;

	if (USART1 == uart->uart_group) {
		RCC_UART1_ON;
		uart->usart.irq = USART1_IRQn;
	} else if (USART2 == uart->uart_group) {
		RCC_UART2_ON;
		uart->usart.irq = USART2_IRQn;
	} else if (USART3 == uart->uart_group) {
		RCC_UART3_ON;
		uart->usart.irq = USART3_IRQn;
	} else if (UART4 == uart->uart_group) {
		RCC_UART4_ON;
		uart->usart.irq = UART4_IRQn;
	} else if (UART5 == uart->uart_group) {
		RCC_UART5_ON;
		uart->usart.irq = UART5_IRQn;
	}
	
	USART_InitStructure.USART_BaudRate = uart->baudrate;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_Init(uart->uart_group,&USART_InitStructure);
	
	USART_ClearFlag(uart->uart_group, USART_FLAG_TC);

	USART_ITConfig(uart->uart_group,USART_IT_RXNE, DISABLE);
	USART_ITConfig(uart->uart_group,USART_IT_TC, DISABLE);
	USART_ITConfig(uart->uart_group,USART_IT_IDLE, DISABLE);


	NVIC_InitStructure.NVIC_IRQChannel = uart->usart.irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(uart->uart_group,ENABLE);
    
    LOGD("OK");
}


static void uart_usart_uninit(uart_t *uart)
{
	NVIC_InitTypeDef  	NVIC_InitStructure;
	
	USART_ITConfig(uart->uart_group, USART_IT_RXNE,DISABLE);
	USART_ITConfig(uart->uart_group, USART_IT_TC,  DISABLE);
	USART_ITConfig(uart->uart_group, USART_IT_IDLE,DISABLE);
	USART_ClearFlag(uart->uart_group,USART_FLAG_TC | USART_IT_RXNE | USART_IT_IDLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = uart->usart.irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(uart->uart_group,DISABLE);
	
	if (USART1 == uart->uart_group) {
		RCC_UART1_OFF;
	} else if (USART2 == uart->uart_group) {
		RCC_UART2_OFF;
	} else if (USART3 == uart->uart_group) {
		RCC_UART3_OFF;
	} else if (UART4 == uart->uart_group) {
		RCC_UART4_OFF;
	} else if (UART5 == uart->uart_group) {
		RCC_UART5_OFF;
	}
}

static void uart_dma_init(uart_t *uart)
{
	DMA_InitTypeDef 	DMA_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	if (USART1 == uart->uart_group) {
		RCC_DMA1_ON;
		uart->dma.tx_channel = DMA1_Channel4;
		uart->dma.rx_channel = DMA1_Channel5;
		uart->dma.irq = DMA1_Channel4_IRQn;
		uart->dma.tc_flag = DMA1_FLAG_TC4;
		uart->dma.it_flag = DMA1_IT_GL4;
	} else if (USART2 == uart->uart_group) {
		RCC_DMA1_ON;
		uart->dma.tx_channel = DMA1_Channel7;
		uart->dma.rx_channel = DMA1_Channel6;
		uart->dma.irq = DMA1_Channel7_IRQn;
		uart->dma.tc_flag = DMA1_FLAG_TC7;
		uart->dma.it_flag = DMA1_IT_GL7;
	} else if (USART3 == uart->uart_group) {
		RCC_DMA1_ON;
		uart->dma.tx_channel = DMA1_Channel2;
		uart->dma.rx_channel = DMA1_Channel3;
		uart->dma.irq = DMA1_Channel2_IRQn;
		uart->dma.tc_flag = DMA1_FLAG_TC2;
		uart->dma.it_flag = DMA1_IT_GL2;
	} else if (UART4 == uart->uart_group) {
		RCC_DMA2_ON;
		uart->dma.tx_channel = DMA2_Channel5;
		uart->dma.rx_channel = DMA2_Channel3;
		uart->dma.irq = DMA2_Channel4_5_IRQn;
		uart->dma.tc_flag = DMA1_FLAG_TC5;
		uart->dma.it_flag = DMA1_IT_GL5;
	} else if (UART5 == uart->uart_group) {
		LOGD("uart_dma_init: UART5 can't use DMA!");
	}

	/*	Configure TX	*/
	DMA_DeInit(uart->dma.tx_channel);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&uart->uart_group->DR);
    //DMA_MemoryBaseAddr: Here can't set 0, this value should set when send
	DMA_InitStructure.DMA_MemoryBaseAddr	 = (u32)uart->dma.rx_buffer;  
	DMA_InitStructure.DMA_DIR				 = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize		 = 0;  //Only use in circular mode;
	DMA_InitStructure.DMA_PeripheralInc 	 = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc 		 = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize	 = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode				 = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority			 = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M				 = DMA_M2M_Disable;
	DMA_Init(uart->dma.tx_channel, &DMA_InitStructure);

	DMA_ITConfig(uart->dma.tx_channel, DMA_IT_TC, ENABLE);
	DMA_Cmd(uart->dma.tx_channel,DISABLE);
    
	/*	Configure RX	*/
	DMA_DeInit(uart->dma.rx_channel);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&uart->uart_group->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr	 = (uint32_t)uart->dma.rx_buffer;
	DMA_InitStructure.DMA_DIR				 = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize		 = uart->dma.rx_buffer_len;//The maximum number we can receive.
	DMA_InitStructure.DMA_PeripheralInc 	 = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc 		 = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize	 = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode				 = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority			 = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M				 = DMA_M2M_Disable;
	DMA_Init(uart->dma.rx_channel,&DMA_InitStructure);
	
	/*	Don't use DMA Receive Interrupt;Handle it in USART IDLE interrupt  */
	DMA_ITConfig(uart->dma.rx_channel,DMA_IT_TC,DISABLE);
	DMA_ITConfig(uart->dma.rx_channel,DMA_IT_TE,DISABLE);

	USART_DMACmd(uart->uart_group,USART_DMAReq_Tx,ENABLE);
	USART_DMACmd(uart->uart_group,USART_DMAReq_Rx,ENABLE);
	DMA_Cmd(uart->dma.rx_channel, ENABLE);

    uart_clear_idle_flag(uart);
	USART_ITConfig(uart->uart_group,USART_IT_IDLE, ENABLE);
    
	/*	Configure DMA TX interrupt	*/
	NVIC_InitStructure.NVIC_IRQChannel = uart->dma.irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

    uart->dma.tx_lock = 0;
    LOGD("OK");
}

void uart_usart_irqhandler(uart_t *uart)
{
	if (USART_GetITStatus(uart->uart_group, USART_IT_IDLE) != RESET)
	{
		DMA_Cmd(uart->dma.rx_channel, DISABLE);    
		uart->rx_size = uart->dma.rx_buffer_len - DMA_GetCurrDataCounter(uart->dma.rx_channel);
		uart->dma.rx_channel->CNDTR = uart->dma.rx_buffer_len;
		DMA_Cmd(uart->dma.rx_channel, ENABLE);
        uart_clear_idle_flag(uart);
        //TODO fifo in.
        uart->rx_fifo.in(&uart->rx_fifo, uart->dma.rx_buffer, uart->rx_size);
		uart->rx_flag = 1;
        LOGD("IDLE IRQ");
	}
}

static void uart_dma_uninit(uart_t *uart)
{
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	DMA_Cmd(uart->dma.tx_channel, DISABLE);
	DMA_Cmd(uart->dma.rx_channel, DISABLE);
	
	DMA_ITConfig(uart->dma.tx_channel,DMA_IT_TC,DISABLE);
	DMA_ITConfig(uart->dma.rx_channel,DMA_IT_TC,DISABLE);
	DMA_ITConfig(uart->dma.rx_channel,DMA_IT_TE,DISABLE);
	
	USART_DMACmd(uart->uart_group, USART_DMAReq_Tx, DISABLE);
	USART_DMACmd(uart->uart_group, USART_DMAReq_Rx, DISABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = uart->dma.irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

static int uart_dma_write(uart_t *uart, u32 add, u8 len)
{
    if (!uart) {
        LOGE("null uart");
        return -EINVAL;
    }

    if (uart->dma.tx_lock) {
        LOGE("dma is busy");
        return -EAGAIN;
    }

    if (!add) {
        LOGE("null address");
        return -EINVAL;
    }
    
    if (!len) {
        LOGE("null len");
        return -EINVAL;
    }

    uart->dma.tx_lock = 1;
	uart->dma.tx_channel->CMAR = add;
	uart->dma.tx_channel->CNDTR = len;
	DMA_Cmd(uart->dma.tx_channel, ENABLE);

    return 0;
}

void uart_dma_irqhandler(uart_t *uart)
{	
	DMA_ClearFlag(uart->dma.tc_flag);
	DMA_ClearITPendingBit(uart->dma.it_flag);
	DMA_Cmd(uart->dma.tx_channel, DISABLE);
    uart->dma.tx_lock = 0;
}


static void uart_clear_idle_flag(uart_t *uart)
{
	u16 i;
	i = uart->uart_group->DR;
	i = uart->uart_group->SR; 
	i++;	
}
