#define LOG_TAG "clk"
#include "log.h"
#include "clk.h"
#include "proc.h"
#include "config.h"
#include "common.h"

/*
Default external 8MHz:
	HCLK  :72000000
	PCLK1 :18000000
	PCLK2 :72000000
	SYSCLK:72000000
	ADCCLK:12000000
*/
void clk_print(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	
    RCC_GetClocksFreq(&RCC_Clocks);
	LOGD("HCLK  :%d\r\n",RCC_Clocks.HCLK_Frequency);
	LOGD("PCLK1 :%d\r\n",RCC_Clocks.PCLK1_Frequency);
	LOGD("PCLK2 :%d\r\n",RCC_Clocks.PCLK2_Frequency);
	LOGD("SYSCLK:%d\r\n",RCC_Clocks.SYSCLK_Frequency);
	LOGD("ADCCLK:%d\r\n",RCC_Clocks.ADCCLK_Frequency);
}

void clk_tick_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_DeInit(CLK_TICK_TIM_GROUP);
	CLK_TICK_TIM_GROUP->CR2 |= 0x10;				
	CLK_TICK_TIM_CLK_ON; 
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 
	TIM_TimeBaseStructure.TIM_Prescaler = (36000-1); // 36000 * 1ms -1 = 35999, 0.5ms per tick 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;																		 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(CLK_TICK_TIM_GROUP, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(CLK_TICK_TIM_GROUP, ENABLE); 
	TIM_Cmd(CLK_TICK_TIM_GROUP, ENABLE); 	
}

void clk_tick_run(void)
{
	static u16 bk1ms = 0;
	static u16 bk10ms=0;
	static u16 bk100ms=0;
	static u16 bk1s=0;
	static u32 bk1min=0;
	
	ut1ms = 0;
	FlagTim = 0;
	while(1)
	{	
		if (ut1ms == bk1ms)
			continue;
		
		bk1ms = ut1ms;
		ut1msProc();
		if (utNow1ms(bk10ms) > 10)
		{
			bk10ms += 10;
			ut10msProc();
			
			if (utNow1ms(bk100ms) > 100)
			{
				bk100ms += 100;
				ut100msProc();
				
				if (utNow1ms(bk1s) > 1000)
				{
					bk1s += 1000;
					ut1sProc();
					
					if (utNow1ms(bk1min) > 60000)
					{
						bk1min += 60000;
						ut1minProc();
					}
				}
			}
		}
	}	
}

/*
	This can test if the tick is accurate.
	Note: printf may spend too many time which
		  will influence the accuracy.
	This function should called from 10msProc
*/
int clk_test(void)
{
	utBeginTask;
	
	LOGD("%d\r\n", ut1ms);
	utWaitXms(1000);
	LOGD("%d\r\n", ut1ms);
	utWaitXms(250);
	LOGD("%d\r\n", ut1ms);
	utWaitXms(30);
	LOGD("%d\r\n", ut1ms);
	utWaitXms(100);
	LOGD("%d\r\n", ut1ms);

	utEndTask;
}
