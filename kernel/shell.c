#include "shell.h"
#include "print.h"
#include "io.h"
#include "timer.h"
#include "memory.h"

static int strings_equal(const char *first, const char *second)
{
    int index = 0;

    while (first[index] != '\0' && second[index] != '\0')
    {
        if (first[index] != second[index])
        {
            return 0;
        }

        index++;
    }

    return first[index] == '\0' &&
           second[index] == '\0';
}

static int starts_with(const char *text, const char *prefix)
{
    int index = 0;

    while (prefix[index] != '\0')
    {
        if (text[index] != prefix[index])
        {
            return 0;
        }

        index++;
    }

    return 1;
}

static void reboot_system(void)
{
    print("Reiniciando o sistema...\n");

    /*
     * Espera o controlador do teclado ficar pronto.
     */
    while (inb(0x64) & 0x02)
    {
    }

    /*
     * Envia o comando de reinicialização para a CPU.
     */
    outb(0x64, 0xFE);

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}

static void shutdown_system(void)
{
    print("Desligando o sistema...\n");

    /*
     * Porta de desligamento ACPI usada pelo QEMU.
     */
    outw(0x604, 0x2000);

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}

static void change_color(const char *color)
{
    if (strings_equal(color, "branco"))
    {
        set_color(15, 0);
        print("Cor alterada para branco.\n");
        return;
    }

    if (strings_equal(color, "verde"))
    {
        set_color(10, 0);
        print("Cor alterada para verde.\n");
        return;
    }

    if (strings_equal(color, "ciano"))
    {
        set_color(11, 0);
        print("Cor alterada para ciano.\n");
        return;
    }

    if (strings_equal(color, "amarelo"))
    {
        set_color(14, 0);
        print("Cor alterada para amarelo.\n");
        return;
    }

    if (strings_equal(color, "vermelho"))
    {
        set_color(12, 0);
        print("Cor alterada para vermelho.\n");
        return;
    }

    set_color(12, 0);
    print("Cor desconhecida.\n");

    set_color(15, 0);
    print("Use: branco, verde, ciano, amarelo ou vermelho.\n");
}

void shell_print_prompt(void)
{
    set_color(11, 0);
    print("NovaCoreOS> ");

    set_color(15, 0);
}

void shell_start(void)
{
    clear_screen();

    set_color(10, 0);
    print("Bem-vindo ao NovaCoreOS!\n");

    set_color(15, 0);
    print("-------------------------\n");
    print("Digite 'help' para ver os comandos.\n\n");
    shell_print_prompt();
}

void shell_execute(const char *command)
{
    if (command[0] == '\0')
    {
        return;
    }

    if (strings_equal(command, "help"))
    {
        set_color(14, 0);
        print("Comandos disponiveis:\n");

        set_color(15, 0);
        print("  help           - mostra os comandos\n");
        print("  clear          - limpa a tela\n");
        print("  about          - mostra informacoes do sistema\n");
        print("  version        - mostra a versao do sistema\n");
        print("  echo TEXTO     - escreve uma mensagem\n");
        print("  color COR      - altera a cor do texto\n");
        print("  reboot         - reinicia o sistema\n");
        print("  uptime         - mostra o tempo ligado\n");
        print("  shutdown       - desliga o QEMU\n");
        print("  crashtest      - testa o Kernel Panic\n");
        print("  meminfo        - mostra informacoes da memoria\n");
	print("  memmap         - mostra o mapa da memoria\n");
        return;
    }

    if (strings_equal(command, "crashtest"))
    {
        print("Testando Kernel Panic...\n");

        __asm__ volatile ("ud2");

        return;
    }

    /* O RESTO DOS SEUS COMANDOS CONTINUA AQUI */

    if (strings_equal(command, "clear"))
    {
        clear_screen();
        return;
    }

    if (strings_equal(command, "about"))
    {
        set_color(10, 0);
        print("NovaCoreOS\n");

        set_color(15, 0);
        print("Sistema operacional educacional criado em C e Assembly.\n");
        print("Kernel de 32 bits iniciado pelo GRUB.\n");
        return;
    }

    if (strings_equal(command, "version"))
    {
        set_color(10, 0);
        print("NovaCoreOS 0.1.0\n");

        set_color(15, 0);
        print("Arquitetura: x86 de 32 bits\n");
        print("Interface: terminal VGA\n");
        return;
    }

    if (strings_equal(command, "echo"))
    {
        print("\n");
        return;
    }

    if (starts_with(command, "echo "))
    {
        print(command + 5);
        print("\n");
        return;
    }

    if (strings_equal(command, "color"))
    {
        print("Uso: color COR\n");
        print("Cores: branco, verde, ciano, amarelo e vermelho.\n");
        return;
    }

    if (starts_with(command, "color "))
    {
        change_color(command + 6);
        return;
    }

    if (strings_equal(command, "reboot"))
    {
        reboot_system();
        return;
    }

    if (strings_equal(command, "shutdown"))
    {
        shutdown_system();
        return;
    }

    if (strings_equal(command, "uptime"))
{
    unsigned int seconds = timer_get_seconds();

    set_color(10, 0);
    print("NovaCoreOS ligado ha ");

    set_color(15, 0);
    print_uint(seconds);
    print(" segundos.\n");

    return;
}

if (strings_equal(command, "meminfo"))
{
    memory_print_info();
    return;
}

if (strings_equal(command, "memmap"))
{
    memory_print_map();
    return;
}

    set_color(12, 0);
    print("Comando desconhecido: ");

    set_color(15, 0);
    print(command);
    print("\n");
    print("Digite 'help' para ver os comandos.\n");
}
