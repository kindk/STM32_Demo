#include "mcu.h"
#include "stm32f10x.h"
#include "config.h"

void mcu_nvic_init(void)
{
#if CONFIG_USE_IAP
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);
#else
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
#endif
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
}

void mcu_clk_init(void)
{ 
	ErrorStatus HSEStartUpStatus;

	/* RCC system reset(for debug purpose) */
	RCC_DeInit();

#if CONFIG_USE_EXT_XTAl
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON); 
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus == SUCCESS)
#endif  	
	{
	/* Enable Prefetch Buffer */
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	/* Flash 2 wait state */
	FLASH_SetLatency(FLASH_Latency_2);
	/* HCLK = SYSCLK */
	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
	/* PCLK2 = HCLK */
	RCC_PCLK2Config(RCC_HCLK_Div1); 
	/* PCLK1 = HCLK/2 */
	RCC_PCLK1Config(RCC_HCLK_Div4);
	//PCLK2 1/4, ADC clk can't exceed 14M, convertion time should less than 1us
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   				

#if CONFIG_USE_EXT_XTAl
	/* PLLCLK = 8MHz * 9 = 72 MHz */
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#else
	//Internal, 8M /2 *16 = 64, can't exceed 64M
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);
#endif
	/* Enable PLL */ 
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08){}
	}
}

void mcu_reset(void)
{
	__set_FAULTMASK(1);      // close all interrupts
	NVIC_SystemReset();		
}
