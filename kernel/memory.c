#include "memory.h"
#include "print.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

/*
 * Estrutura Multiboot 1.
 *
 * Precisamos chegar até mmap_length e mmap_addr,
 * então os campos anteriores precisam estar
 * exatamente na ordem definida pelo Multiboot.
 */
struct multiboot_info
{
    unsigned int flags;

    unsigned int mem_lower;
    unsigned int mem_upper;

    unsigned int boot_device;
    unsigned int cmdline;

    unsigned int mods_count;
    unsigned int mods_addr;

    /*
     * Informações a.out / ELF.
     * O campo ocupa 16 bytes.
     */
    unsigned int syms[4];

    unsigned int mmap_length;
    unsigned int mmap_addr;

} __attribute__((packed));


/*
 * Cada entrada do mapa de memória.
 *
 * Evitamos unsigned long long por enquanto
 * para manter tudo simples no kernel de 32 bits.
 */
struct multiboot_mmap_entry
{
    unsigned int size;

    unsigned int addr_low;
    unsigned int addr_high;

    unsigned int len_low;
    unsigned int len_high;

    unsigned int type;

} __attribute__((packed));


static unsigned int memory_lower_kb = 0;
static unsigned int memory_upper_kb = 0;

static unsigned int memory_map_address = 0;
static unsigned int memory_map_length = 0;

static int memory_information_valid = 0;
static int memory_map_valid = 0;


void memory_init(
    unsigned int multiboot_magic,
    unsigned int multiboot_info_address
)
{
    memory_information_valid = 0;
    memory_map_valid = 0;

    if (multiboot_magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        return;
    }

    struct multiboot_info *info =
        (struct multiboot_info *)multiboot_info_address;


    /*
     * Bit 0:
     * mem_lower e mem_upper são válidos.
     */
    if (info->flags & (1 << 0))
    {
        memory_lower_kb = info->mem_lower;
        memory_upper_kb = info->mem_upper;

        memory_information_valid = 1;
    }


    /*
     * Bit 6:
     * mapa de memória disponível.
     */
    if (info->flags & (1 << 6))
    {
        memory_map_address = info->mmap_addr;
        memory_map_length = info->mmap_length;

        memory_map_valid = 1;
    }
}


unsigned int memory_get_lower_kb(void)
{
    return memory_lower_kb;
}


unsigned int memory_get_upper_kb(void)
{
    return memory_upper_kb;
}


void memory_print_info(void)
{
    if (!memory_information_valid)
    {
        set_color(12, 0);

        print(
            "Nao foi possivel obter "
            "informacoes da RAM.\n"
        );

        set_color(15, 0);

        return;
    }


    set_color(10, 0);

    print("=== Memoria do NovaCoreOS ===\n");


    set_color(15, 0);


    print("Memoria inferior: ");

    print_uint(memory_lower_kb);

    print(" KB\n");


    print("Memoria superior: ");

    print_uint(memory_upper_kb / 1024);

    print(" MB\n");


    unsigned int approximate_total_mb =
        (memory_upper_kb + 1024) / 1024;


    print("RAM aproximada: ");

    print_uint(approximate_total_mb);

    print(" MB\n");
}


static const char *memory_type_name(
    unsigned int type
)
{
    switch (type)
    {
        case 1:
            return "LIVRE";

        case 2:
            return "RESERVADA";

        case 3:
            return "ACPI";

        case 4:
            return "ACPI NVS";

        case 5:
            return "DEFEITUOSA";

        default:
            return "DESCONHECIDA";
    }
}


void memory_print_map(void)
{
    if (!memory_map_valid)
    {
        set_color(12, 0);

        print(
            "Mapa de memoria nao disponivel.\n"
        );

        set_color(15, 0);

        return;
    }


    set_color(10, 0);

    print("=== MAPA DE MEMORIA ===\n\n");


    unsigned int current =
        memory_map_address;

    unsigned int end =
        memory_map_address +
        memory_map_length;


    unsigned int region = 0;


    while (current < end)
    {
        struct multiboot_mmap_entry *entry =
            (struct multiboot_mmap_entry *)current;


        set_color(11, 0);

        print("Regiao ");

        print_uint(region);

        print(":\n");


        set_color(15, 0);


        print("  Base: ");

        if (entry->addr_high != 0)
        {
            print("> 4 GB");
        }
        else
        {
            print_hex(entry->addr_low);
        }

        print("\n");


        print("  Tamanho: ");

        if (entry->len_high != 0)
        {
            print("> 4 GB");
        }
        else
        {
            /*
             * Mostramos KB para regiões pequenas
             * e MB para regiões maiores.
             */
            if (entry->len_low >= 1024 * 1024)
            {
                print_uint(
                    entry->len_low /
                    (1024 * 1024)
                );

                print(" MB");
            }
            else
            {
                print_uint(
                    entry->len_low / 1024
                );

                print(" KB");
            }
        }

        print("\n");


        print("  Tipo: ");

        if (entry->type == 1)
        {
            set_color(10, 0);
        }
        else
        {
            set_color(14, 0);
        }

        print(
            memory_type_name(entry->type)
        );

        set_color(15, 0);

        print("\n\n");


        /*
         * O campo size NÃO inclui os próprios
         * 4 bytes do campo size.
         */
        current +=
            entry->size +
            sizeof(entry->size);


        region++;
    }
}
