#include "process.h"
#include "kheap.h"
#include "scheduler.h"

static process_t processes[MAX_PROCESSES];
static uint32_t  next_pid = 1;
static process_t *current_process = 0;

void process_init() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].state = PROCESS_UNUSED;
        processes[i].pid   = 0;
        processes[i].regs.eip = 0;
    }
}

process_t *process_create(const char *name, void (*entry)()) {
    process_t *proc = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_UNUSED) {
            proc = &processes[i];
            break;
        }
    }
    if (!proc) return 0;

    proc->stack = (uint8_t*)kmalloc(PROCESS_STACK_SIZE);
    if (!proc->stack) return 0;

    /* Store entry point in regs.eip */
    proc->regs.eip = (uint32_t)entry;

    proc->pid   = next_pid++;
    proc->state = PROCESS_READY;
    proc->sleep_ticks = 0;

    int i = 0;
    while (name[i] && i < 31) {
        proc->name[i] = name[i];
        i++;
    }
    proc->name[i] = '\0';

    return proc;
}

void process_exit() {
    if (current_process) {
        current_process->state = PROCESS_DEAD;
        scheduler_yield();
    }
}

void process_sleep(uint32_t ticks) {
    if (current_process) {
        current_process->sleep_ticks = ticks;
        current_process->state = PROCESS_SLEEPING;
        scheduler_yield();
    }
}

process_t *process_current() {
    return current_process;
}

void process_set_current(process_t *proc) {
    current_process = proc;
}

process_t *process_get_table() {
    return processes;
}