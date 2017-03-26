#define LOG_TAG "main"
#include "log.h"
#include "main.h"
#include "key.h"
#include "string.h"
#include "irq.h"

/* Private functions ---------------------------------------------------------*/
void main_init(void);
void main_delay(void);

led_t led;
uart_t uart;
key_t key;
fifo_t key_fifo;

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
  
void uart_read_proc(uart_t *uart, u8 *data, u8 len);
int main(void)
{
    irq_disable_all();
	main_init();
	
	//clk_print();
	irq_enable_all();
	main_loop();
	/* Infinite loop, never go here */
	while (1){}
}

void main_init(void) 
{
	mcu_nvic_init();
	mcu_clk_init();
	clk_tick_init();
	print_init();
	led_init(&led, GPIOC, GPIO_Pin_15);
	uart_init(&uart, USART1, 115200, 20, 20, 10, 20);
    uart.set_read_callback(&uart, uart_read_proc);
    key_init(&key,0, GPIOB, GPIO_Pin_9, 0, 100, &key_fifo);
    LOGD("main_delay");
	//while(1){led_test(&led);}
	main_delay();
}

void main_delay(void)
{
	int a = 10000000;
	while(a--);
}

void uart_read_proc(uart_t *uart, u8 *data, u8 len)
{
    data[len] = '\0';
    LOGI("%d %s", len, data);
    if (strcmp((char *)data, "disable") == 0) {
        LOGD("disable irq");
    }
}

