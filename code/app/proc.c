#define LOG_TAG "proc"
#include "log.h"

#include "proc.h"
#include "stdio.h"
#include "led.h"
#include "main.h"
#include "uart.h"
#include "key.h"

void key_process(key_t *key);

int ut1msProc(void)
{
	
	
	return 0;
}

int ut10msProc(void)
{
	uart.write_daemon(&uart);
    uart.read_daemon(&uart);
    key_state_machine(&key);
	//clk_test();
	return 0;
}

int ut100msProc(void)
{
	key_process(&key);
	return 0;
}

int ut1sProc(void)
{
	utBeginTask;
	
	led.toggle(&led);
	
	//uart.write(&uart, (u8 *)"123456", 6);

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

//FIXME: for all irq modified data, should add lock.
// Now, only irqHandler is async, need lock.
void DMA1_Channel4_IRQHandler()
{
	uart_dma_irqhandler(&uart);
}

void USART1_IRQHandler()
{
    uart_usart_irqhandler(&uart);
}

void EXTI9_5_IRQHandler()
{
    key_irq_handler(&key);
    LOGI(" ");
}

void key_process(key_t *key)
{
    key_event_t event;
	u8 len;
    if (!key->fifo->is_empty(key->fifo)) {
        key->fifo->out(key->fifo, (u8 *)&event, &len);
        LOGI("code: %d, action: %s", event.id, key_action_string[event.action]);
    }
}
