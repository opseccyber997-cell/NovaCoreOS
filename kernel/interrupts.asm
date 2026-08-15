section .text

global idt_load
global irq0_stub
global irq1_stub
global isr6_stub

extern timer_ticks
extern keyboard_handler
extern invalid_opcode_handler

idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret


; =====================================================
; IRQ0 - TIMER
; =====================================================

irq0_stub:
    push eax

    ; Incrementa o contador do timer
    inc dword [timer_ticks]

    ; EOI para o PIC principal
    mov al, 0x20
    out 0x20, al

    pop eax

    iretd


; =====================================================
; IRQ1 - TECLADO
; =====================================================

irq1_stub:
    ; Salva os registradores gerais
    pushad

    ; C espera Direction Flag limpo
    cld

    ;
    ; Mantém a pilha alinhada antes de chamar código C.
    ;
    mov ebp, esp
    and esp, 0xFFFFFFF0

    call keyboard_handler

    ; Recupera a posição original da pilha
    mov esp, ebp

    ; Informa ao PIC que terminamos a IRQ1
    mov al, 0x20
    out 0x20, al

    ; Recupera os registradores
    popad

    ; Volta para onde a CPU estava
    iretd

; =====================================================
; EXCECAO 6 - INVALID OPCODE
; =====================================================

isr6_stub:
    cli
    cld

    ; Alinha a pilha para chamar codigo C.
    and esp, 0xFFFFFFF0

    call invalid_opcode_handler

.hang:
    hlt
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite progbits
