# NovaCoreOS
### Sistema Operacional de Cria

Um mini sistema operacional **educacional** de 32 bits (x86), escrito em **C** e **Assembly (NASM)**. Ele é iniciado pelo GRUB, roda num terminal em modo texto (VGA) e pode ser testado no QEMU.

## Sobre o projeto

Esse sistema operacional está sendo desenvolvido por 3 pessoas no **PURO CHAT GPT**, então nada que está sendo feito aqui foi feito na sabedoria extrema, porque eu acho que nenhuma pessoa que tem de 13 a 14 anos merece saber programar em C e Assembly.

## O que ele já faz

- Inicia pelo **GRUB** (padrão Multiboot)
- Terminal de texto VGA, com cores e cursor
- **Teclado**: leitura das teclas, com suporte ao Shift
- **Interrupções**: tabela IDT e controlador PIC
- **Timer** a 100 Hz, usado para mostrar o tempo ligado
- **Kernel Panic**: tela de erro quando uma instrução inválida é executada
- **Memória**: lê as informações e o mapa de memória entregues pelo GRUB
- **Shell** com vários comandos

## Comandos do shell

| Comando | O que faz |
| --- | --- |
| `help` | Mostra os comandos |
| `clear` | Limpa a tela |
| `about` | Mostra informações do sistema |
| `version` | Mostra a versão do sistema |
| `echo TEXTO` | Escreve uma mensagem |
| `color COR` | Altera a cor do texto (branco, verde, ciano, amarelo ou vermelho) |
| `uptime` | Mostra há quanto tempo o sistema está ligado |
| `meminfo` | Mostra informações da memória |
| `memmap` | Mostra o mapa da memória |
| `reboot` | Reinicia o sistema |
| `shutdown` | Desliga o QEMU |
| `crashtest` | Testa o Kernel Panic |

## Estrutura do projeto

```
boot/grub/grub.cfg        configuração do GRUB
kernel/boot.asm           ponto de entrada (_start), cabeçalho Multiboot e pilha
kernel/kernel.c           kernel_main: inicializa tudo na ordem certa
kernel/interrupts.asm     código Assembly das interrupções (IRQ0, IRQ1 e exceção 6)
kernel/interrupts.c       monta a IDT e configura o PIC
kernel/exceptions.c       tela de Kernel Panic
kernel/timer.c            timer (PIT)
kernel/keyboard.c         teclado
kernel/print.c            texto na tela VGA
kernel/memory.c           informações e mapa de memória
kernel/shell.c            shell de comandos
kernel/linker.ld          script do linker (onde cada parte vai na memória)
include/                  arquivos de cabeçalho (.h)
Makefile                  receita de compilação
```

## Como compilar e rodar

Num Linux baseado em Debian/Ubuntu, instale as ferramentas:

```bash
sudo apt install build-essential gcc-multilib nasm grub-pc-bin xorriso mtools qemu-system-x86
```

Depois, na pasta do projeto:

```bash
make        # compila o kernel e gera NovaCoreOS.iso
make run    # abre a ISO no QEMU
make clean  # apaga os arquivos gerados
```

Os arquivos `.o`, `kernel.bin` e a ISO são gerados pelo `make`, por isso não ficam no repositório.
