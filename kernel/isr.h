#ifndef ISR_H
#define ISR_H

#include <stdint.h>


extern void isr0();
extern void isr1();
extern void isr14();


extern void irq0();
extern void irq1();


void isr_handler(uint32_t num);
void irq_handler(uint32_t num);

#endif