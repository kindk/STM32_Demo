#ifndef _UART_H
#define _UART_H

#include "stm32f10x.h"
#include "fifo.h"

// uart5 can't support dma
typedef struct _uart_gpio{
	GPIO_TypeDef *tx_group;
	GPIO_TypeDef *rx_group;
	u16 tx_pin;
	u16 rx_pin;
}uart_gpio_t;

typedef struct _uart_usart{
	enum IRQn irq;
}uart_usart_t;

typedef struct _uart_dma{
	enum IRQn irq;
	DMA_Channel_TypeDef *tx_channel;
	DMA_Channel_TypeDef *rx_channel;
	void *rx_buffer;
	u32 rx_buffer_len;
	u32 tc_flag;
	u32 it_flag;
    u8 tx_lock;
}uart_dma_t;

typedef struct _uart uart_t;
struct _uart{
	USART_TypeDef *uart_group;
	u32 baudrate;
	uart_gpio_t gpio;
	uart_usart_t usart;
	uart_dma_t dma;
	u8 opened;
	u32 tx_size;
	u32 rx_size;
	u8 tx_flag;
	u8 rx_flag;
	fifo_t tx_fifo;
	fifo_t rx_fifo;
	
	void (*write_daemon)(uart_t *uart);
	void (*read_daemon)(uart_t *uart);
	void (*write)(uart_t *uart, u8 *data, u32 len);
    void (*read_callback)(uart_t *uart, u8 *data, u8 len);
    void (*set_read_callback)(uart_t *uart, void (*read_callback)(uart_t *uart, u8 *data, u8 len));
};

void uart_init(uart_t *uart, USART_TypeDef *uart_group, u32 baudrate, 
               u32 tx_fifo_len, u32 tx_data_len, u32 rx_fifo_len, u32 rx_data_len);
void uart_dma_irqhandler(uart_t *uart);
void uart_usart_irqhandler(uart_t *uart);

#endif
