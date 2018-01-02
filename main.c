#include <stdlib.h>
#include <time.h>

#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    char* command;
    int shell_available;
    clock_t t1, t2;
    size_t addr_start, addr_end, swap_temporary;
    register size_t i;

    if (argc < 2) {
        puts("Syntax:");
        printf("\t%s [starting address] [ending address]\n", argv[0]);
        printf("\t%s [ending address] [starting address]\n", argv[0]);
        printf("\t%s [single address]\n", argv[0]);
        return 0;
    }
    if (argv[1][0] != '-') { /* branch */
    } else {
        unsigned char memory_byte;

        addr_start = (size_t)strtoul(argv[2], NULL, 16);
        memory_byte = *(unsigned char *)(addr_start);
        printf("0x%02X (%c)", memory_byte, memory_byte);
        return 0;
    }

    addr_start = (size_t)strtoul(argv[1], NULL, 0);
    addr_end   = addr_start;
    if (argc >= 3)
        addr_end = (size_t)strtoul(argv[2], NULL, 0);
    if (addr_end < addr_start) {
        swap_temporary = addr_start;
        addr_start = addr_end;
        addr_end = swap_temporary;
    }
    command = malloc(
        strlen(argv[0])
      + strlen(" -? ")
      + strlen("0x")
      + 2*sizeof(void *)
      + sizeof('\0')
    );

    shell_available = system(NULL);
    if (!shell_available) {
        fprintf(stderr, "No command shell available.\n");
        return 1;
    }

    t1 = clock();
    for (i = 0; addr_start + i <= addr_end; i++) {
        printf("RAM[0x%lX]:  ", addr_start + i);
        sprintf(command, "%s -- 0x%lX", argv[0], addr_start + i);
        fflush(NULL);
        system(command);
        putchar('\n');
    }
    t2 = clock();
    putchar('\n');
    printf("Time taken:  %f seconds.\n", (float)(t2 - t1) / CLOCKS_PER_SEC);
    return 0;
}
