/**
******************************************************************************
* @file    	utprt.c
* @author  	kindk
* @version 	V1.0.0
* @date    	2017/03/21
* @brief    ʹ��IO��ģ�⴮�ڴ�ӡ������Ϣ
			Ĭ�ϲ�����460800
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
//�ڲ�����Ԥ����
#define PT_TIMER	(PRT_TIM_NUM->SR)
#define TXF			TIM_IT_Update	//��������prt��ʱ�������־λ

#define IS_TXF		(PT_TIMER & TXF)
#define TXF_OFF 	(PT_TIMER &= ~TXF)

#define TXP_HIGH	((*GPIOX_PT) |= GPIOX_Pin)		//((GPIOC->ODR) |= BIT12)	//
#define TXP_LOW		((*GPIOX_PT) &= ~GPIOX_Pin)	//((GPIOC->ODR) &= ~BIT12)	//
#define TXP_TOG		((*GPIOX_PT) ^= GPIOX_Pin)
//==============================================================================
//�ڲ����ñ���

//==============================================================================
//�ڲ����ú�������
static char hex2cha(char hex);

//==============================================================================
//�ⲿ���ú���
void (*utpPtb)(char c);		//ͨ�ã��ҽ�ָ������ĳ������ptb


//##############################################################################
//prtģ��
//##############################################################################
//------------------------------------------------------------------------------
void utPtbx(char c) 			//���һ���ֽ�,����ͨ��
{	
	unsigned char i;

//	utEnterCrt;
	
	while (!IS_TXF);		//ͬ��	
	TXF_OFF; 				//�����ǰ�ı�־λ
	//(1)���ٶ���һ��bit,�������죬��Ҫ�ǵȴ����һλ�����⣬����ʱһλҲ�����

	//��ʼλ
	while (!IS_TXF);		//ͬ��
	TXP_LOW;
	TXF_OFF; 				//�����־λ

	//8λ����
	for (i = 0; i < 8; i++)
	{
		while (!IS_TXF);	//ͬ��

		#if 1
		if (c & 0x01)		//�ȴ���λ
			TXP_HIGH; 		//���ܼӷֺ�
		else
			TXP_LOW;		//���ܼӷֺ�
		#endif
		
		c >>= 1;			//����һλ

		TXF_OFF; 			//�����־λ
	}

	//��ֹλ
	while (!IS_TXF);
	TXP_HIGH; 				//����λ
	TXF_OFF; 				//�����־λ

	//���(1)�����ȴ����˴���Ҫ�ȴ�

//	utExitCrt;
}

//------------------------------------------------------------------------------
void utPtb(char c)			//���һ���ַ�
{
	(*utpPtb)(c);
}


//------------------------------------------------------------------------------
void utPtc(char c)			//���һ���ַ�
{
	if(c == '\n')			//�����'n'�ȼ�һ��'r'
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
		//��ʼλ
		while (!IS_TXF);	//ͬ��
		TXF_OFF;			//�����־λ
		TXP_LOW;

		//��ʼλ
		while (!IS_TXF);	//ͬ��
		TXF_OFF;			//�����־λ
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
