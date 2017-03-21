/*
	This is uart demo.
	Want to hold this strong uart driver?
	Just follow this 7 simple step.
                    |---------> dma
                    |           ||
        |--> write --> fifo --> dma
uart<-->|
        |<-- read <-- fifo <-- dma
	
1. Define uart.

	uart_t uart;
	
2. Call void uart_init(uart_t *uart, USART_TypeDef *uart_group, u32 baudrate,\
         u32 tx_fifo_len, u32 tx_data_len, u32 rx_fifo_len, u32 rx_data_len)

   fifo_len: estimate the max num messages 
             may stored in fifo before sent out or receive at a time.
   data_len: message max len
   This function will allocate fifo_len * data_len buffer for fifo,
   you may need enlarge heap size.

    uart_init(&uart, USART1, 115200, 30, 30, 20, 20);

3. Call uart.write_daemon() & uart.read_daemon() every 10ms.

    uart.write_daemon(&uart)
    uart.read_daemon(&uart)

4. Declare your usart IRQHandler depending on your USARTx num.
   For USART1:

   	void USART1_IRQHandler()
	{
	    uart_usart_irqhandler(&uart);
	}

   All you can define are as below:
	void USART1_IRQHandler();
	void USART2_IRQHandler();
	void USART3_IRQHandler();
	void UART4_IRQHandler();
	void UART5_IRQHandler();
5. Declare your uart DMA (TX) channel IRQHandler depending on your USARTx num.
   For USART1:
   
	void DMA1_Channel4_IRQHandler()
	{
		uart_dma_irqhandler(&uart);
	}

    Note: The DMA num and Channel num can be found uart_dma_init() in uart.c
    And it MUST be TX channel. Don't make a mistake.

6. Call uart.write() when you want to send message, this will not block
   the process, only start DMA or copy your message to fifo.

    uart.write(&uart, (u8 *)"123456", 6);

7. For read, just set your read callback, this callback will call from
   user space.
   1> 
	   	void uart_read_proc(uart_t *uart, u8 *data, u8 len)
		{
		    data[len] = '\0';
		    LOGI("%d %s", len, data);
		}
   2> 
   		uart.set_read_callback(&uart, uart_read_proc);
*/


