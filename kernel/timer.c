#include "timer.h"
#include "io.h"

#define PIT_CHANNEL_ZERO 0x40
#define PIT_COMMAND_PORT 0x43
#define PIT_BASE_FREQUENCY 1193180

volatile unsigned int timer_ticks = 0;

static unsigned int timer_frequency = 100;

void timer_init(unsigned int frequency)
{
    if (frequency == 0)
    {
        frequency = 100;
    }

    timer_frequency = frequency;

    unsigned int divisor =
        PIT_BASE_FREQUENCY / frequency;

    outb(PIT_COMMAND_PORT, 0x36);

    outb(
        PIT_CHANNEL_ZERO,
        (unsigned char)(divisor & 0xFF)
    );

    outb(
        PIT_CHANNEL_ZERO,
        (unsigned char)((divisor >> 8) & 0xFF)
    );
}

void timer_handler(void)
{
    /*
     * Não precisamos mais fazer nada aqui.
     * A IRQ0 é tratada diretamente no Assembly.
     */
}

unsigned int timer_get_ticks(void)
{
    return timer_ticks;
}

unsigned int timer_get_seconds(void)
{
    return timer_ticks / timer_frequency;
}
