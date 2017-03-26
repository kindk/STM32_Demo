#ifndef _IRQ_H
#define _IRQ_H
#include "type.h"

void CPU_IntDis(void);
void CPU_IntEn(void);
cpu_sr_t CPU_SR_Save(void);
void CPU_SR_Restore(cpu_sr_t cpu_sr);

// irq_disable_all should only used at cpu start up.
#define irq_disable_all CPU_IntDis
#define irq_enable_all CPU_IntEn

#define irq_alloc() cpu_sr_t cpu_sr = (cpu_sr_t) 0
#define irq_disable() do { cpu_sr = CPU_SR_Save(); } while (0)
#define irq_enable() do { CPU_SR_Restore(cpu_sr); } while (0)

#endif
