#include "print.h"
#include "shell.h"
#include "interrupts.h"
#include "timer.h"

void kernel_main(void)
{
    enable_cursor();

    /*
     * Configura IDT + PIC.
     */
    interrupts_init();

    /*
     * Timer em aproximadamente 100 Hz.
     */
    timer_init(100);

    /*
     * Inicia nosso shell.
     */
    shell_start();

    /*
     * A partir daqui IRQ0 e IRQ1 podem chegar.
     */
    interrupts_enable();


    /*
     * Não precisamos mais de keyboard_loop().
     *
     * A CPU fica esperando interrupções.
     */
    while (1)
    {
        __asm__ volatile ("hlt");
    }
}

