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

; GDT flush
gdt_flush:
  mov eax, [esp+4]
  lgdt [eax]
  mov ax, 0x10      ; data segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  jmp 0x08:.flush   ; far jump to code segment
.flush:
  ret

; IDT flush
idt_flush:
  mov eax, [esp+4]
  lidt [eax]
  ret

; Macro for ISR stubs without error code
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
  cli
  push dword %1
  jmp isr_common
%endmacro

; Macro for IRQ stubs
%macro IRQ 1
global irq%1
irq%1:
  cli
  push dword %1
  jmp irq_common
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 14
IRQ 0
IRQ 1

extern isr_handler
isr_common:
  pusha
  call isr_handler
  popa
  add esp, 4
  sti
  iret

extern irq_handler
irq_common:
  pusha
  call irq_handler
  popa
  add esp, 4
  sti
  iret

_start:
  mov esp, stack_top
  call kernel_main
  cli
  hlt

section .bss
align 16
stack_bottom: resb 16384
stack_top: