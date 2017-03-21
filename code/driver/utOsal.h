#ifndef _UTOSAL_H
#define _UTOSAL_H
#include "clk.h"

#define false 	0
#define true 	!false

#define STLINE	((u16)(__LINE__))

#define utBeginTask			\
	static u16 st = 0;	\
	switch(st)				\
	{						\
		case 0:				\
	
		
#define utWait(proc)		\
		st = STLINE;		\
		case STLINE:		\
		if (!(proc))		\
			return false;	
		
		
#define utEndTask			\
		st = 0;				\
		return true;		\
	}						\
	return false;

			
#define utWaitXms(tms) 			\
	{							\
		static u16 nowms;	\
		nowms = ut1ms;			\
		utWait(((utNow1ms(nowms)) >= tms));	\
	}

#endif
