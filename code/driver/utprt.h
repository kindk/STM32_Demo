/**
******************************************************************************
* @file    	utprt.h
* @author  	kindk
* @version 	V1.0.0
* @date    	2017/03/21
* @brief   	Only use one pin to print sth.
		   	Need one timer to support time.

History: 
<author> <time> <version > <desc>
kindk  2017/03/21  1.0  build this moudle

Introduction:
1.	ʹ��IO��ģ��USART��printf
2.	�޸���������   	��utprt.h��
3.	��ʼ������		utPrt_init();
4.	Ӳ��ֻ������PRT_PIN��GND
5.	��printf�ϴ����(32bit)������ִ�ӡ�����0�ȣ��Ƽ�ʹ��printf
******************************************************************************
**/

#ifndef _UTPRT_H
#define _UTPRT_H
#include "stm32f10x.h"

/*********************************   pin   ***********************************/
#define	PRT_PORT_RCC	RCC_APB2Periph_GPIOA
#define PRT_PORT		GPIOA
#define PRT_PIN			GPIO_Pin_9
#define PRT_BUS_CLOCK	36000000    
#define PRT_BAUD		115200  //460800  
#define	PRT_TIM_RCC		RCC_APB1Periph_TIM3
#define PRT_TIM_NUM		TIM3

/*********************************   function   ******************************/
void SoftPrt_Inits(void);
void utPrt(char *fmt, ...);
void utPtn(char *str);
void utPtd(char *data);


#endif
