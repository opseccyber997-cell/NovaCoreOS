#include "keyboard.h"
#include "shell.h"
#include "print.h"
#include "io.h"

#define KEYBOARD_DATA_PORT 0x60
#define COMMAND_BUFFER_SIZE 128

#define LEFT_SHIFT_PRESS    0x2A
#define RIGHT_SHIFT_PRESS   0x36
#define LEFT_SHIFT_RELEASE  0xAA
#define RIGHT_SHIFT_RELEASE 0xB6

static char command_buffer[COMMAND_BUFFER_SIZE];
static int command_length = 0;

static int shift_pressed = 0;
static int extended_scancode = 0;

static const char normal_table[128] =
{
    0,
    27,
    '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b',
    '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k',
    'l', ';', '\'', '`',
    0,
    '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm',
    ',', '.', '/',
    0,
    '*',
    0,
    ' '
};

static const char shift_table[128] =
{
    0,
    27,
    '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+', '\b',
    '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', '\n',
    0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K',
    'L', ':', '"', '~',
    0,
    '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M',
    '<', '>', '?',
    0,
    '*',
    0,
    ' '
};

static void handle_enter(void)
{
    command_buffer[command_length] = '\0';

    print("\n");

    shell_execute(command_buffer);

    command_length = 0;
    command_buffer[0] = '\0';

    shell_print_prompt();
}

static void handle_backspace(void)
{
    if (command_length <= 0)
    {
        return;
    }

    command_length--;

    command_buffer[command_length] = '\0';

    print_backspace();
}

static void handle_character(char character)
{
    if (command_length >= COMMAND_BUFFER_SIZE - 1)
    {
        return;
    }

    command_buffer[command_length] = character;

    command_length++;

    command_buffer[command_length] = '\0';

    print_char(character);
}

void keyboard_handler(void)
{
    /*
     * Como esta função só é chamada quando chega a IRQ1,
     * não precisamos ficar consultando a porta 0x64.
     */
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);

    /*
     * Algumas teclas especiais começam com 0xE0.
     * Por enquanto vamos ignorá-las.
     */
    if (scancode == 0xE0)
    {
        extended_scancode = 1;
        return;
    }

    if (extended_scancode)
    {
        extended_scancode = 0;
        return;
    }

    if (scancode == LEFT_SHIFT_PRESS ||
        scancode == RIGHT_SHIFT_PRESS)
    {
        shift_pressed = 1;
        return;
    }

    if (scancode == LEFT_SHIFT_RELEASE ||
        scancode == RIGHT_SHIFT_RELEASE)
    {
        shift_pressed = 0;
        return;
    }

    /*
     * Bit 7 ligado = tecla sendo solta.
     */
    if (scancode & 0x80)
    {
        return;
    }

    if (scancode >= 128)
    {
        return;
    }

    char character;

    if (shift_pressed)
    {
        character = shift_table[scancode];
    }
    else
    {
        character = normal_table[scancode];
    }

    if (character == 0)
    {
        return;
    }

    if (character == '\n')
    {
        handle_enter();
        return;
    }

    if (character == '\b')
    {
        handle_backspace();
        return;
    }

    if (character == '\t')
    {
        return;
    }

    handle_character(character);
}
