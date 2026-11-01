section .multiboot
align 4

MULTIBOOT_MAGIC equ 0x1BADB002

; Bit 0 = alinhar modulos em paginas de 4 KB
; Bit 1 = pedir informacoes de memoria
MULTIBOOT_FLAGS equ 0x00000003

MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

dd MULTIBOOT_MAGIC
dd MULTIBOOT_FLAGS
dd MULTIBOOT_CHECKSUM


section .bss
align 16

stack_bottom:
    resb 16384

stack_top:


section .text

global _start
extern kernel_main


_start:
    cli

    ; Criamos nossa propria pilha.
    mov esp, stack_top
    xor ebp, ebp

    ;
    ; O GRUB entrega:
    ;
    ; EAX = magic Multiboot
    ; EBX = endereco da estrutura Multiboot
    ;
    ; kernel_main(magic, info)
    ;
    ; Em C/cdecl os argumentos sao empilhados
    ; da direita para a esquerda.
    ;

    push ebx
    push eax

    call kernel_main

    add esp, 8


.hang:
    hlt
    jmp .hang


section .note.GNU-stack noalloc noexec nowrite progbits
