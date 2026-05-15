#include "scheduler.h"
#include "process.h"

extern void process_set_current(process_t *proc);
extern process_t *process_get_table();

static uint32_t timer_ticks = 0;
static uint32_t current_idx = 0;

static char *vga = (char*)0xB8000;

static void show_running(const char *name) {
    /* Clear bottom row */
    for (int i = 0; i < 80; i++) {
        vga[(24 * 80 + i) * 2]     = ' ';
        vga[(24 * 80 + i) * 2 + 1] = 0x0F;
    }
    const char *label = "Running: ";
    int col = 0;
    for (int i = 0; label[i]; i++) {
        vga[(24 * 80 + col) * 2]     = label[i];
        vga[(24 * 80 + col) * 2 + 1] = 0x0B;
        col++;
    }
    for (int i = 0; name[i]; i++) {
        vga[(24 * 80 + col) * 2]     = name[i];
        vga[(24 * 80 + col) * 2 + 1] = 0x0E;
        col++;
    }
}

void scheduler_init() {
    timer_ticks = 0;
    current_idx = 0;
}

void scheduler_tick() {
    timer_ticks++;

    process_t *table = process_get_table();

    /* Wake sleeping processes */
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (table[i].state == PROCESS_SLEEPING) {
            if (table[i].sleep_ticks > 0)
                table[i].sleep_ticks--;
            if (table[i].sleep_ticks == 0)
                table[i].state = PROCESS_READY;
        }
    }

    /* Switch every 20 ticks */
    if (timer_ticks % 20 != 0) return;

    /* Find next ready process */
    for (int i = 1; i <= MAX_PROCESSES; i++) {
        uint32_t idx = (current_idx + i) % MAX_PROCESSES;
        if (table[idx].state == PROCESS_READY ||
            table[idx].state == PROCESS_RUNNING) {

            /* Mark old process ready */
            if (table[current_idx].state == PROCESS_RUNNING)
                table[current_idx].state = PROCESS_READY;

            /* Switch to new process */
            current_idx = idx;
            table[idx].state = PROCESS_RUNNING;
            process_set_current(&table[idx]);
            show_running(table[idx].name);

            /* Actually jump to the task's entry point */
            void (*entry)() = (void(*)())table[idx].regs.eip;
            if (entry) entry();
            return;
        }
    }
}

void scheduler_yield() {
    asm volatile("int $0x20");
}

void scheduler_run() {
    process_t *table = process_get_table();
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (table[i].state == PROCESS_READY) {
            current_idx = i;
            table[i].state = PROCESS_RUNNING;
            process_set_current(&table[i]);
            show_running(table[i].name);

            /* Jump directly to task entry */
            void (*entry)() = (void(*)())table[i].regs.eip;
            if (entry) entry();
            return;
        }
    }
}