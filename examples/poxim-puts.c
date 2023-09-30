#define DEBUG 1
#include "_start.h"

int *terminal32 = (int *)(0x88888888 >> 2);
void putchar(int c) { *terminal32 = c; }


/* @Attention, to garantee puts will work the str has to be 4 byte aligned :) 
  That means every string will in the whole program have to be padded with zero to ensure 
  every string is a multiple of 4 hehe, remember that last byte of str literal is always zero 
*/
void puts(const char *str) {
    int *ptr = (int *)str;
    int word;

    int len = 0;
    while (1) {
        word = *ptr++;

        len += 4;
        // Verifica se algum byte no int é zero (terminador nulo).
        if (((word >> 24) & 0xFF) == 0) {
            return;
        }
        putchar((word >> 24) & 0xFF);
        if (((word >> 16) & 0xFF)== 0) {
            return;
        }
        putchar((word >> 16) & 0xFF);
        if (((word >> 8) & 0xFF)== 0) {
            return;
        }
        putchar((word >> 8) & 0xFF);
        if (((word >> 0) & 0xFF)== 0) {
            return;
        }
        putchar((word >> 0) & 0xFF);
      
    }
}

unsigned int strlen(const char *str) {
    int *ptr = (int *)str;
    int word;

    int len = 0;
    while (1) {
        word = *ptr++;

        len += 4;
        // Verifica se algum byte no int é zero (terminador nulo).
        if (((word >> 24) & 0xFF) == 0) {
            return len - 1;
        }
        if (((word >> 16) & 0xFF)== 0) {
            return len - 2;
        }
        if (((word >> 8) & 0xFF)== 0) {
            return len - 3;
        }
        if (((word >> 0) & 0xFF)== 0) {
            return len  - 4;
        }
    }
}


int main(void) {
    puts("acc");
    puts("bcc");
    return strlen("123");
   // int* ptr = (int*) (4 >> 2);
}
