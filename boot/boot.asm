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

; =========================
; GDT Flush
; =========================
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

; =========================
; IDT Flush
; =========================
idt_flush:
  mov eax, [esp+4]
  lidt [eax]
  ret

; =========================
; ISR Macros
; =========================
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
  cli
  push dword %1
  jmp isr_common
%endmacro

; =========================
; IRQ Macros
; =========================
%macro IRQ 1
global irq%1
irq%1:
  cli
  push dword %1
  jmp irq_common
%endmacro

; =========================
; CPU Exceptions
; =========================
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 14

; =========================
; Hardware IRQs
; =========================
IRQ 0
IRQ 1

; =========================
; ISR Common Handler
; =========================
extern isr_handler

isr_common:
  pusha

  ; Get interrupt number
  mov eax, [esp + 32]
  push eax

  call isr_handler

  add esp, 4
  popa

  ; Remove original interrupt number
  add esp, 4

  sti
  iretd

; =========================
; IRQ Common Handler
; =========================
extern irq_handler

irq_common:
  pusha

  ; Get IRQ number
  mov eax, [esp + 32]
  push eax

  call irq_handler

  add esp, 4
  popa

  ; Remove original IRQ number
  add esp, 4

  sti
  iretd

; =========================
; Kernel Entry
; =========================
_start:
  mov esp, stack_top
  call kernel_main

.hang:
  cli
  hlt
  jmp .hang

; =========================
; Stack
; =========================
section .bss
align 16

stack_bottom:
  resb 16384

stack_top: