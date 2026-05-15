#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define PROCESS_STACK_SIZE  4096    /* 4KB stack per process  */
#define MAX_PROCESSES       16      /* max concurrent tasks   */

/* All possible states a process can be in */
typedef enum {
    PROCESS_UNUSED  = 0,  /* slot is empty             */
    PROCESS_READY   = 1,  /* ready to run              */
    PROCESS_RUNNING = 2,  /* currently running         */
    PROCESS_SLEEPING= 3,  /* waiting for something     */
    PROCESS_DEAD    = 4   /* finished, to be cleaned up*/
} process_state_t;

/* Saved CPU register state — must match pusha order */
typedef struct {
    uint32_t edi, esi, ebp, esp;
    uint32_t ebx, edx, ecx, eax;
    uint32_t eip, eflags;
} registers_t;

/* The process control block (PCB) */
typedef struct process {
    uint32_t        pid;          /* process ID              */
    process_state_t state;        /* current state           */
    registers_t     regs;         /* saved register state    */
    uint32_t        esp;          /* saved stack pointer     */
    uint8_t        *stack;        /* pointer to stack memory */
    uint32_t        sleep_ticks;  /* ticks remaining to sleep*/
    char            name[32];     /* process name            */
} process_t;

void process_init();
process_t *process_create(const char *name, void (*entry)());
void process_exit();
void process_sleep(uint32_t ticks);
process_t *process_current();

#endif