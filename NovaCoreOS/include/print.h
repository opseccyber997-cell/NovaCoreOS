#ifndef PRINT_H
#define PRINT_H

void clear_screen(void);
void set_color(unsigned char foreground, unsigned char background);
void print(const char *message);
void print_char(char character);
void print_backspace(void);

void enable_cursor(void);
void disable_cursor(void);
void update_cursor(void);

void print_uint(unsigned int number);

#endif
