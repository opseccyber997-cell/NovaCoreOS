#include "interrupts.h"
#include "io.h"

#define IDT_SIZE 256

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

struct idt_entry
{
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char attributes;
    unsigned short offset_high;
} __attribute__((packed));

struct idt_pointer
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

static struct idt_entry idt[IDT_SIZE];
static struct idt_pointer idt_ptr;

extern void idt_load(struct idt_pointer *pointer);

extern void irq0_stub(void);
extern void irq1_stub(void);


/*
 * Descobre automaticamente qual segmento de código
 * o kernel está usando.
 *
 * Isso evita o problema que tivemos usando 0x08 fixo.
 */
static unsigned short get_code_segment(void)
{
    unsigned short selector;

    __asm__ volatile (
        "mov %%cs, %0"
        : "=r"(selector)
    );

    return selector;
}


static void idt_set_gate(
    unsigned char number,
    unsigned int handler,
    unsigned short selector,
    unsigned char attributes
)
{
    idt[number].offset_low =
        (unsigned short)(handler & 0xFFFF);

    idt[number].selector = selector;

    idt[number].zero = 0;

    idt[number].attributes = attributes;

    idt[number].offset_high =
        (unsigned short)((handler >> 16) & 0xFFFF);
}


static void idt_clear(void)
{
    for (int index = 0; index < IDT_SIZE; index++)
    {
        idt[index].offset_low = 0;
        idt[index].selector = 0;
        idt[index].zero = 0;
        idt[index].attributes = 0;
        idt[index].offset_high = 0;
    }
}


static void pic_remap(void)
{
    /*
     * Começa a inicialização dos dois PICs.
     */
    outb(PIC1_COMMAND, 0x11);
    io_wait();

    outb(PIC2_COMMAND, 0x11);
    io_wait();


    /*
     * PIC principal:
     *
     * IRQ0 -> vetor 32
     * IRQ1 -> vetor 33
     * ...
     * IRQ7 -> vetor 39
     */
    outb(PIC1_DATA, 0x20);
    io_wait();


    /*
     * PIC secundário:
     *
     * IRQ8 -> vetor 40
     * ...
     * IRQ15 -> vetor 47
     */
    outb(PIC2_DATA, 0x28);
    io_wait();


    /*
     * PIC secundário conectado na IRQ2
     * do PIC principal.
     */
    outb(PIC1_DATA, 0x04);
    io_wait();

    outb(PIC2_DATA, 0x02);
    io_wait();


    /*
     * Modo 8086.
     */
    outb(PIC1_DATA, 0x01);
    io_wait();

    outb(PIC2_DATA, 0x01);
    io_wait();


    /*
     * 11111100 = 0xFC
     *
     * IRQ0 liberada -> TIMER
     * IRQ1 liberada -> TECLADO
     *
     * IRQ2-IRQ7 continuam bloqueadas.
     */
    outb(PIC1_DATA, 0xFC);


    /*
     * Todas as IRQs do PIC secundário
     * permanecem bloqueadas.
     */
    outb(PIC2_DATA, 0xFF);
}


void interrupts_disable(void)
{
    __asm__ volatile ("cli");
}


void interrupts_enable(void)
{
    __asm__ volatile ("sti");
}


void interrupts_init(void)
{
    interrupts_disable();

    idt_clear();

    pic_remap();

    unsigned short code_segment =
        get_code_segment();


    /*
     * IRQ0
     *
     * Timer
     *
     * PIC IRQ0 -> IDT 32
     */
    idt_set_gate(
        32,
        (unsigned int)irq0_stub,
        code_segment,
        0x8E
    );


    /*
     * IRQ1
     *
     * Teclado
     *
     * PIC IRQ1 -> IDT 33
     */
    idt_set_gate(
        33,
        (unsigned int)irq1_stub,
        code_segment,
        0x8E
    );


    idt_ptr.limit =
        (unsigned short)(sizeof(idt) - 1);

    idt_ptr.base =
        (unsigned int)&idt;


    idt_load(&idt_ptr);
}
