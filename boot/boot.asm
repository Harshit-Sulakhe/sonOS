section .multiboot
align 4
  dd 0x1BADB002
  dd 0
  dd -(0x1BADB002)

section .text

global _start
extern kernel_main
extern gdt_flush
extern idt_flush


gdt_flush:
  mov eax, [esp+4]
  lgdt [eax]

  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  jmp 0x08:.flush

.flush:
  ret


idt_flush:
  mov eax, [esp+4]
  lidt [eax]
  ret



%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push dword 0
    push dword %1
    jmp isr_common
%endmacro

%macro IRQ 2
global irq%1
irq%1:
    push dword 0
    push dword %2
    jmp irq_common
%endmacro

; CPU Exceptions
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 14

; Hardware IRQs
IRQ 0, 32
IRQ 1, 33



extern isr_handler

isr_common:
    pusha

    mov eax, esp
    push eax

    call isr_handler

    add esp, 4
    popa

    add esp, 8
    iret



extern irq_handler

irq_common:
    pusha

    mov eax, esp
    push eax

    call irq_handler

    add esp, 4
    popa

    add esp, 8
    iret


_start:
  mov esp, stack_top
  call kernel_main

.hang:
  cli
  hlt
  jmp .hang


section .bss
align 16

stack_bottom:
  resb 16384

stack_top: