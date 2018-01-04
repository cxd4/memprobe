#include <stdlib.h>
#include <time.h>

#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <setjmp.h>

static jmp_buf exc_pt;
void exception_handler(int parameter)
{
    putchar('\n');
    longjmp(exc_pt, parameter);
}

int main(int argc, char* argv[])
{
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
/*
 * We will be attempting to dereference user-defined pointers in global RAM.
 * We can expect segmentation faults from memory access violations.
 *
 * The rumor that C doesn't portably support catching exceptions like C++
 * does is mostly true.  signal() and longjmp() prove the rumor false only on
 * platforms where the environment generates automatic signals.
 */
    signal(SIGSEGV, exception_handler);

    addr_start = (size_t)strtoul(argv[1], NULL, 0);
    addr_end   = addr_start;
    if (argc >= 3)
        addr_end = (size_t)strtoul(argv[2], NULL, 0);
    if (addr_end < addr_start) {
        swap_temporary = addr_start;
        addr_start = addr_end;
        addr_end = swap_temporary;
    }

    shell_available = system(NULL);
    if (!shell_available) {
        fprintf(stderr, "No command shell available.\n");
        return 1;
    }

    t1 = clock();
    for (i = 0; addr_start + i <= addr_end; i++) {
        unsigned char memory_byte;
        int recovered_from_exception;

        printf("RAM[0x%lX]:  ", addr_start + i);
        recovered_from_exception = setjmp(exc_pt);
        if (recovered_from_exception) {
            signal(SIGSEGV, exception_handler); /* Reschedule the handler. */
            continue;
        }
        memory_byte = *(unsigned char *)(addr_start + i);
        printf("0x%02X (%c)\n", memory_byte, memory_byte);
    }
    t2 = clock();
    putchar('\n');
    printf("Time taken:  %f seconds.\n", (float)(t2 - t1) / CLOCKS_PER_SEC);
    return 0;
}
