#!/usr/local/bin/tcc -run
#include <tcclib.h>

void _start(void); asm (                                                                                
"_start:\n"
		/* mov sp, 0x7FFC */
		".long 0xFC7FC003;\n"
);

void int0(void); asm (                                                                                
"int0:\n"
		/* int 0 */
		".long 0x000000FC;"
);

int main(void)
{
    const int a = 1;
    const int b = 2;
    (void)a;
    (void)b;
    // printf("Hello World\n");
    return a+b;
}

