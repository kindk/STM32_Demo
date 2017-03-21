/**
******************************************************************************
* @file    	utprt.c
* @author  	kindk
* @version 	V1.0.0
* @date    	2017/03/21
* @brief    使用IO口模拟串口打印调试信息
			默认波特率460800
			utPrt_init();
			utPrt(); utPtn();
History: 
<author> <time> <version > <desc>
kindk   2017/03/21  1.0  build this moudle
******************************************************************************
**/

#include "utprt.h"
#include "stdarg.h"

static GPIO_TypeDef *GPIOX;
unsigned short 	GPIOX_Pin;
volatile uint32_t *GPIOX_PT;

void (*utpPtb)(char c);	

//==============================================================================
//内部符号预定义
#define PT_TIMER	(PRT_TIM_NUM->SR)
#define TXF			TIM_IT_Update	//定义用于prt定时的溢出标志位

#define IS_TXF		(PT_TIMER & TXF)
#define TXF_OFF 	(PT_TIMER &= ~TXF)

#define TXP_HIGH	((*GPIOX_PT) |= GPIOX_Pin)		//((GPIOC->ODR) |= BIT12)	//
#define TXP_LOW		((*GPIOX_PT) &= ~GPIOX_Pin)	//((GPIOC->ODR) &= ~BIT12)	//
#define TXP_TOG		((*GPIOX_PT) ^= GPIOX_Pin)
//==============================================================================
//内部调用变量

//==============================================================================
//内部调用函数定义
static char hex2cha(char hex);

//==============================================================================
//外部调用函数
void (*utpPtb)(char c);		//通用，挂接指针后就用某个具体ptb


//##############################################################################
//prt模块
//##############################################################################
//------------------------------------------------------------------------------
void utPtbx(char c) 			//输出一个字节,万能通用
{	
	unsigned char i;

//	utEnterCrt;
	
	while (!IS_TXF);		//同步	
	TXF_OFF; 				//清除先前的标志位
	//(1)至少多了一个bit,宁慢不快，主要是等待最后一位的问题，连续时一位也不会多

	//起始位
	while (!IS_TXF);		//同步
	TXP_LOW;
	TXF_OFF; 				//清除标志位

	//8位数据
	for (i = 0; i < 8; i++)
	{
		while (!IS_TXF);	//同步

		#if 1
		if (c & 0x01)		//先传低位
			TXP_HIGH; 		//不能加分号
		else
			TXP_LOW;		//不能加分号
		#endif
		
		c >>= 1;			//右移一位

		TXF_OFF; 			//清除标志位
	}

	//终止位
	while (!IS_TXF);
	TXP_HIGH; 				//结束位
	TXF_OFF; 				//清除标志位

	//如果(1)处不等待，此处就要等待

//	utExitCrt;
}

//------------------------------------------------------------------------------
void utPtb(char c)			//输出一个字符
{
	(*utpPtb)(c);
}


//------------------------------------------------------------------------------
void utPtc(char c)			//输出一个字符
{
	if(c == '\n')			//如果是'n'先加一个'r'
	{
		(*utpPtb)('\r');		
	}

	(*utpPtb)(c);	
}

//------------------------------------------------------------------------------
void utPts(char *str)
{
	char *p = str;
	
	while(*p)
	{
		(*utpPtb)(*p);
		p++;
	}
}

//------------------------------------------------------------------------------
void utPtn(char *str)
{
	char *p = str;
	
	while(*p)
	{
		utPtc(*p);
		p++;
	}
}

//------------------------------------------------------------------------------
char hex2cha(char hex)
{
	if(hex>9)
		return (char)(hex-10+'A');
	else
		return (char)(hex+'0');
}

//------------------------------------------------------------------------------
void utPrt(char *fmt, ...)
{
    char *s;
    unsigned int d;
    char buf[11];
    va_list ap;
	
    va_start(ap, fmt); 
    while (*fmt) 
	{
		if (*fmt != '%') 
		{
			utPtc(*fmt++);
			continue;
		}
        switch (*++fmt) 
		{
            case 's':
                s = va_arg(ap, char*);
                for ( ; *s; s++) 
				{
                    utPtc(*s);
                }
                break;

			#if 0
            case 'c':
                s = va_arg(ap, unsigned char*);
                utPtb(*s);
                break;
			#endif

			#if 1
            case 'c':
                d = va_arg(ap, int);
                utPtb(((unsigned char)d));
                break;
			#endif
			
							
            case 'd':
                d = va_arg(ap, int);	//WORD
				if(d>99999)
				{
					buf[10] = 0;
					buf[9] = '0'+d%10;
					d /= 10;
					buf[8] = '0'+d%10;
					d /= 10;
					buf[7] = '0'+d%10;
					d /= 10;
					buf[6] = '0'+d%10;
					d /= 10;
					buf[5] = '0'+d%10;
					d /= 10;
					buf[4] = '0'+d%10;
					d /= 10;
					buf[3] = '0'+d%10;
					d /= 10;
					buf[2] = '0'+d%10;
					d /= 10;
					buf[1] = '0'+d%10;
					d /= 10;
					buf[0] = '0'+d%10;
				}
				else if (d>9999)
				{
					buf[5] = 0;
					buf[4] = '0'+d%10;
					d /= 10;
					buf[3] = '0'+d%10;
					d /= 10;
					buf[2] = '0'+d%10;
					d /= 10;
					buf[1] = '0'+d%10;
					d /= 10;
					buf[0] = '0'+d%10;
				}
				else if (d>999)
				{
					buf[4] = 0;
					buf[3] = '0'+d%10;
					d /= 10;
					buf[2] = '0'+d%10;
					d /= 10;
					buf[1] = '0'+d%10;
					d /= 10;
					buf[0] = '0'+d%10;
				}
				else if (d>99)
				{
					buf[3] = 0;
					buf[2] = '0'+d%10;
					d /= 10;
					buf[1] = '0'+d%10;
					d /= 10;
					buf[0] = '0'+d%10;
				}
				else if (d>9)
				{
					buf[2] = 0;
					buf[1] = '0'+d%10;
					d /= 10;
					buf[0] = '0'+d%10;
				}
				else
				{
					buf[1] = 0;
					buf[0] = '0'+d%10;
				}
					
                for (s = buf; *s; s++) 
				{
                    utPtc(*s);
                }
                break;

			case 'x':
				d = va_arg(ap, int);	//WORD
				if(d & 0xff00)
				{
					buf[4] = 0;
					buf[3] = hex2cha(d&0xf);
					d >>= 4;
					buf[2] = hex2cha(d&0xf);
					d >>= 4;
					buf[1] = hex2cha(d&0xf);
					d >>= 4;
					buf[0] = hex2cha(d&0xf);
				}
				else
				{
					buf[2] = 0;
					buf[1] = hex2cha(d&0xf);
					d >>= 4;
					buf[0] = hex2cha(d&0xf);
				}

                for (s = buf; *s; s++) 
				{
                    utPtc(*s);
                }				
				break;
				
            /* Add other specifiers here... */              
            default:  
                utPtc(*fmt);
                break;
        }
        fmt++;
    }
    va_end(ap);
}
void utDelay1us(unsigned char x)
{
	unsigned char i;
	
	while(x--)
	{
		i = 6;
		while(i--);
	}
}
void utDelay1ms(unsigned char x)
{
	unsigned char i;
	
	while(x--)
	{
		i = 20;
		while(i--)
			utDelay1us(50);
	}
}
void utDelay1s(unsigned char x)
{	
	while(x--)
	{
		utDelay1ms(250);
		utDelay1ms(250);
		utDelay1ms(250);
		utDelay1ms(250);
	}
}
void utPrtwrg(unsigned char x)
{
	while(1)
	{	
		utPrt("wrg%s\n",x);
		utDelay1ms(50);	
	}
}

void utCalClk1(void)
{
	while (1)
	{
		//起始位
		while (!IS_TXF);	//同步
		TXF_OFF;			//清除标志位
		TXP_LOW;

		//起始位
		while (!IS_TXF);	//同步
		TXF_OFF;			//清除标志位
		TXP_HIGH;
	}
}

void utPrt_init(void)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(PRT_TIM_RCC, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = PRT_BUS_CLOCK / PRT_BAUD;
	TIM_TimeBaseStructure.TIM_Prescaler = 0x00; 	  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(PRT_TIM_NUM, &TIM_TimeBaseStructure);  

	TIM_ARRPreloadConfig(PRT_TIM_NUM, ENABLE);
	TIM_Cmd(PRT_TIM_NUM, ENABLE);

	GPIOX = PRT_PORT;
	GPIOX_Pin = PRT_PIN;
	GPIOX_PT = &(GPIOX->ODR);
	
	utpPtb = utPtbx;
	
	RCC_APB2PeriphClockCmd(PRT_PORT_RCC, ENABLE);
	GPIO_SetBits(GPIOX,GPIOX_Pin);	 	
	GPIO_InitStructure.GPIO_Pin =	GPIOX_Pin;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
	GPIO_Init(GPIOX, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOX,GPIOX_Pin);	
}
