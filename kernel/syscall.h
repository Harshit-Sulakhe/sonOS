#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

/* Syscall numbers */
#define SYS_WRITE   1
#define SYS_READ    2
#define SYS_OPEN    3
#define SYS_CLOSE   4
#define SYS_EXIT    5

void syscall_init();
uint32_t syscall_handler(uint32_t num,
                         uint32_t arg1,
                         uint32_t arg2,
                         uint32_t arg3);

#endif