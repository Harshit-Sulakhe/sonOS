#ifndef ISR_H
#define ISR_H

#include <stdint.h>
#include "process.h"

extern void isr0();
extern void isr1();
extern void isr14();


extern void irq0();
extern void irq1();




void isr_handler(interrupt_frame_t *r);
void irq_handler(interrupt_frame_t *r);

#endif