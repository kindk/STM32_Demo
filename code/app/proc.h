#ifndef _PROC_H
#define _PROC_H
#include "clk.h"
#include "utOsal.h"

int ut1msProc(void);
int ut10msProc(void);
int ut100msProc(void);
int ut1sProc(void);
int ut1minProc(void);

void main_loop(void);

#endif
