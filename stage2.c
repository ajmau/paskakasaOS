#include <stdint.h>
;
__attribute__((noreturn))
void loader_main() {

    char *vidmem = (char*)0xb8000;
    *vidmem = 'B';
    while (1) {
        asm volatile ("hlt");
    }
}
