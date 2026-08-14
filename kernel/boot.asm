section .multiboot
align 4

MULTIBOOT_MAGIC equ 0x1BADB002
MULTIBOOT_FLAGS equ 0
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

dd MULTIBOOT_MAGIC
dd MULTIBOOT_FLAGS
dd MULTIBOOT_CHECKSUM

section .bss
align 16

stack_bottom:
    resb 16384             ; Reserva 16 KB para a pilha
stack_top:

section .text
global _start
extern kernel_main

_start:
    cli                    ; Interrupções ainda desligadas

    mov esp, stack_top     ; Define o topo da pilha
    xor ebp, ebp           ; Limpa o ponteiro-base

    call kernel_main

.hang:
    hlt
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite progbits
