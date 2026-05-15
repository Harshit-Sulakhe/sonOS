#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

void scheduler_init();
void scheduler_tick();    /* called by timer IRQ      */
void scheduler_yield();   /* voluntarily give up CPU  */
void scheduler_run();     /* start the first process  */

#endif