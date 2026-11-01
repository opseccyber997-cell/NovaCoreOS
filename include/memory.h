#ifndef MEMORY_H
#define MEMORY_H

void memory_init(
    unsigned int multiboot_magic,
    unsigned int multiboot_info_address
);

void memory_print_info(void);
void memory_print_map(void);

unsigned int memory_get_lower_kb(void);
unsigned int memory_get_upper_kb(void);

#endif
