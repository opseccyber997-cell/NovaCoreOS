#include "exceptions.h"
#include "interrupts.h"
#include "print.h"

void invalid_opcode_handler(void)
{
    interrupts_disable();

    clear_screen();

    set_color(12, 0);

    print("========================================\n");
    print("       NovaCoreOS - KERNEL PANIC\n");
    print("========================================\n\n");

    set_color(15, 0);

    print("Excecao: Invalid Opcode\n");
    print("Vetor: 6\n\n");

    set_color(14, 0);

    print("Uma instrucao invalida foi executada.\n");
    print("O kernel foi interrompido.\n");
    print("Reinicie o NovaCoreOS para continuar.\n");

    disable_cursor();

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}
