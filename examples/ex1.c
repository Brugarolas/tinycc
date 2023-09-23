#!/usr/local/bin/tcc -run
#include <tcclib.h>

int main(void);

// void _start(void){
//   main();
// }

int main(void)
{
    const int a = 1;
    const int b = 2;
    (void)a;
    (void)b;
    // printf("Hello World\n");
    return a+b;
}

