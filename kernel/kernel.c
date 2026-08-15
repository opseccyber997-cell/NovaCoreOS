#include "print.h"
#include "shell.h"
#include "interrupts.h"
#include "timer.h"
#include "memory.h"

void kernel_main(
    unsigned int multiboot_magic,
    unsigned int multiboot_info_address
)
{
    enable_cursor();

    /*
     * Descobre a memoria antes de começarmos
     * a criar nosso gerenciador.
     */
    memory_init(
        multiboot_magic,
        multiboot_info_address
    );

    interrupts_init();

    timer_init(100);

    shell_start();

    interrupts_enable();

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}
