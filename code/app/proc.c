#include "proc.h"
#include "stdio.h"
#include "led.h"
#include "main.h"
#include "uart.h"

int ut1msProc(void)
{
	
	
	return 0;
}

int ut10msProc(void)
{
	uart.write_daemon(&uart);
    uart.read_daemon(&uart);
	//clk_test();
	return 0;
}

int ut100msProc(void)
{
	
	
	return 0;
}

int ut1sProc(void)
{
	utBeginTask;
	
	led.toggle(&led);
	
	uart.write(&uart, (u8 *)"123456", 6);

	utEndTask;
}

int ut1minProc(void)
{
	
	
	return 0;	
}

void main_loop(void)
{
	clk_tick_run();
}

void DMA1_Channel4_IRQHandler()
{
	uart_dma_irqhandler(&uart);
}

void USART1_IRQHandler()
{
    uart_usart_irqhandler(&uart);
}
