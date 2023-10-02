/*
  If we're not using clang not gcc then we define poxim
  this is for us to run both on tcc-poxim and gcc and not change the code
*/
#if !defined(__clang__) && !defined(__GNUC__) && !defined(__GNUG__)
// other checks __clang_major__, __clang_minor__ and __clang_patchlevel__
#define __poxim__
#endif

#if defined(__poxim__)
#include "_start.h"
int *terminal32 = (int *)(0x88888888 >> 2);

unsigned int strlen(const char *str);
void putchar(int c) { *terminal32 = c; }
void puti(int num);
void puts(const char *str);

//>>********************** Printing ********************* //
/* @Attention, to garantee puts will work the str has to be 4 byte aligned :)
  That means every string will in the whole program have to be padded with zero
  to ensure every string is a multiple of 4 hehe, remember that last byte of str
  literal is always zero -> '\0'
*/
void puts(const char *str) {
  int *ptr = (int *)str;
  int word;

  int len = 0;
  while (1) {
    word = *ptr++;

    len += 4;
    // Verifica se algum byte no int  zero (terminador nulo).
    int b1 = ((word >> 24) & 0xFF), b2 = ((word >> 16) & 0xFF),
        b3 = ((word >> 8) & 0xFF), b4 = ((word >> 0) & 0xFF);
    if (b1 == 0 && len != 4) {
      return;
    }
    if (b1) {
      putchar((word >> 24) & 0xFF);
    }

    if (b2 == 0 && len != 4) {
      return;
    }
    if (b2) {
      putchar((word >> 16) & 0xFF);
    }

    if (b3 == 0 && len != 4) {
      return;
    }
    if (b3) {
      putchar((word >> 8) & 0xFF);
    }
    if (b4 == 0 && len != 4) {
      return;
    }
    if (b4) {
      putchar((word >> 0) & 0xFF);
    }
  }
}

unsigned int strlen(const char *str) {
  int *ptr = (int *)str;
  int word;

  int len = 0;
  while (1) {
    word = *ptr++;

    len += 4;
    // Verifica se algum byte no int ├⌐ zero (terminador nulo).
    if (((word >> 24) & 0xFF) == 0) {
      return len - 1;
    }
    if (((word >> 16) & 0xFF) == 0) {
      return len - 2;
    }
    if (((word >> 8) & 0xFF) == 0) {
      return len - 3;
    }
    if (((word >> 0) & 0xFF) == 0) {
      return len - 4;
    }
  }
}

void puti(int num) {
  int divisor = 1;
  int is_negative = 0;

  // Handle negative numbers
  if (num < 0) {
    is_negative = 1;
    num = -num;
  }

  // Find the divisor for the largest digit place
  while (num / divisor > 9) {
    divisor *= 10;
  }

  // If the number was negative, print the minus sign
  if (is_negative > 0) {
    putchar('-');
  } else {
    // putchar('+');
  }
  // Extract and print each digit
  while (divisor > 0) {
    int digit = (num / divisor) % 10;
    putchar('0' + digit);
    divisor /= 10;
  }
}
#else
#include <stdio.h>
#include <stdlib.h>
#define puti(x) printf("%d", x)
#define puts(input) fputs(input, stdout) // no new line just line mine
#endif
#define POOL_SIZE 216 // Adjust this to your desired memory pool size
#define BLOCK_SIZE 32 // Adjust this to your desired block size

typedef struct {
  int memory_pool[POOL_SIZE / sizeof(int)];
  int *free_ptr;
} MemAlloc;

MemAlloc mem;

void meminit(MemAlloc *m) { m->free_ptr = m->memory_pool; }

int *memalloc(MemAlloc *mem, size_t size) {
  if (size > BLOCK_SIZE) {
    // Requested size is too large or out of memory
    puts("We only operate in block size of ");
    puti(BLOCK_SIZE);
    puts(" bytes ");
    return NULL;
  }
  #ifdef __poxim__
  if (mem->free_ptr + (size) > mem->memory_pool + (POOL_SIZE)) {
  #else
  if (mem->free_ptr + (size >> 2) > mem->memory_pool + (POOL_SIZE >> 2)) {
  #endif
    puts("\nIt's over, we ain't gonna give you no more memmory. It's full");
    return NULL;
  }

  void *ptr = mem->free_ptr;
  #ifdef __poxim__
  mem->free_ptr += size; //NOTE: This is different from gcc and clang beucase of how we handle pointers
  #else
  mem->free_ptr += size>>2; //NOTE: This is different from gcc and clang beucase of how we handle pointers
  #endif
  return mem->free_ptr;
}

int main() {
  // Initialize the memory pool
  meminit(&mem);
  (void)(int *)memalloc(&mem, BLOCK_SIZE + 1);
  // Allocate memory
  int *ptr, previous = 0;
  while (1) {
    ptr = (int *)memalloc(&mem, 32);
    if (ptr) {
      *ptr = (int)ptr & 0xff; // Truncado
      puts(" \nAllocated memory: \n ptr = ");
      puti(*ptr);
      if (*ptr) {
        if (previous != 0) {
          puts(" \n ptr - previous_ptr = ");
          puti((int)(*ptr - previous));
        }
      }
    } else {
      puts("\nMemory allocation failed\n");
      break;
    }
    if (ptr) {
      previous = *ptr;
    }
  }

  return 0;
}
