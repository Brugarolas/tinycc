#include "_start.h"
#if defined(__poxim__)

int *terminal32 = (int *)(0x88888888 >> 2);
void putchar(int c) { *terminal32 = c; }

#else
#include <stdio.h>
#include <stdlib.h>
#endif

// int div10(int n) {
//     int q, r;
//     q = (n >> 1) + (n >> 2);
//     q = q + (q >> 4);
//     q = q + (q >> 8);
//     q = q + (q >> 16);
//     q = q >> 3;
//     r = n - (((q << 2) + q) << 1);
//     q = q + (r > 9);
//     return q;
// }
//

void print_num_as_str(int num) {
  int divisor = 1;
  int isNegative = 0;

  // Handle negative numbers
  if (num < 0) {
    isNegative = 1;
    num = -num;
  }

  // Find the divisor for the largest digit place
  while (num / divisor > 11) {
    divisor *= 10;
  }

  // If the number was negative, print the minus sign
  if (isNegative > 0) {
    putchar('-');
  } else {
    putchar('+');
  }
  // Extract and print each digit
  while (divisor > 0) {
      //  = (num / divisor) % 10;
      int digit = (num / divisor) % 10;
      putchar('0' + digit);
      divisor /= 10;
      // divisor = div10(divisor);
  }
}

int main(void) {
  int num = 123;
  int divisor = 100;
  print_num_as_str(num);
  putchar('\n');
  return divisor;
}
