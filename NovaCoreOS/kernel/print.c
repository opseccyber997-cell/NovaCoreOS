#include "print.h"
#include "io.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY_ADDRESS 0xB8000

#define VGA_COMMAND_PORT 0x3D4
#define VGA_DATA_PORT 0x3D5

static volatile unsigned short *vga_memory =
    (volatile unsigned short *)VGA_MEMORY_ADDRESS;

static int cursor_row = 0;
static int cursor_column = 0;
static unsigned char text_color = 0x0F;

static unsigned short create_vga_entry(char character)
{
    return (unsigned short)character |
           ((unsigned short)text_color << 8);
}

void update_cursor(void)
{
    unsigned short position =
        (unsigned short)(cursor_row * VGA_WIDTH + cursor_column);

    /* Envia os 8 bits inferiores da posição. */
    outb(VGA_COMMAND_PORT, 0x0F);
    outb(VGA_DATA_PORT, (unsigned char)(position & 0xFF));

    /* Envia os 8 bits superiores da posição. */
    outb(VGA_COMMAND_PORT, 0x0E);
    outb(VGA_DATA_PORT, (unsigned char)((position >> 8) & 0xFF));
}

void enable_cursor(void)
{
    /*
     * Configura o formato do cursor.
     * Os valores 14 e 15 criam um cursor semelhante a um sublinhado.
     */
    outb(VGA_COMMAND_PORT, 0x0A);
    outb(VGA_DATA_PORT, 14);

    outb(VGA_COMMAND_PORT, 0x0B);
    outb(VGA_DATA_PORT, 15);

    update_cursor();
}

void disable_cursor(void)
{
    outb(VGA_COMMAND_PORT, 0x0A);
    outb(VGA_DATA_PORT, 0x20);
}

static void scroll_screen(void)
{
    for (int row = 1; row < VGA_HEIGHT; row++)
    {
        for (int column = 0; column < VGA_WIDTH; column++)
        {
            int current_position = row * VGA_WIDTH + column;
            int previous_position = (row - 1) * VGA_WIDTH + column;

            vga_memory[previous_position] =
                vga_memory[current_position];
        }
    }

    for (int column = 0; column < VGA_WIDTH; column++)
    {
        int position = (VGA_HEIGHT - 1) * VGA_WIDTH + column;
        vga_memory[position] = create_vga_entry(' ');
    }

    cursor_row = VGA_HEIGHT - 1;
    update_cursor();
}

static void new_line(void)
{
    cursor_column = 0;
    cursor_row++;

    if (cursor_row >= VGA_HEIGHT)
    {
        scroll_screen();
    }

    update_cursor();
}

void set_color(unsigned char foreground, unsigned char background)
{
    text_color = foreground | (background << 4);
}

void clear_screen(void)
{
    for (int row = 0; row < VGA_HEIGHT; row++)
    {
        for (int column = 0; column < VGA_WIDTH; column++)
        {
            int position = row * VGA_WIDTH + column;
            vga_memory[position] = create_vga_entry(' ');
        }
    }

    cursor_row = 0;
    cursor_column = 0;

    update_cursor();
}

void print_backspace(void)
{
    if (cursor_column > 0)
    {
        cursor_column--;
    }
    else if (cursor_row > 0)
    {
        cursor_row--;
        cursor_column = VGA_WIDTH - 1;
    }
    else
    {
        return;
    }

    int position = cursor_row * VGA_WIDTH + cursor_column;
    vga_memory[position] = create_vga_entry(' ');

    update_cursor();
}

void print_char(char character)
{
    if (character == '\n')
    {
        new_line();
        return;
    }

    if (character == '\b')
    {
        print_backspace();
        return;
    }

    int position = cursor_row * VGA_WIDTH + cursor_column;
    vga_memory[position] = create_vga_entry(character);

    cursor_column++;

    if (cursor_column >= VGA_WIDTH)
    {
        new_line();
        return;
    }

    update_cursor();
}

void print(const char *message)
{
    int index = 0;

    while (message[index] != '\0')
    {
        print_char(message[index]);
        index++;
    }
}

void print_uint(unsigned int number)
{
    char buffer[11];
    int index = 0;

    if (number == 0)
    {
        print_char('0');
        return;
    }

    while (number > 0)
    {
        buffer[index] = '0' + (number % 10);
        number /= 10;
        index++;
    }

    while (index > 0)
    {
        index--;
        print_char(buffer[index]);
    }
}
