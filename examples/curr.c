#define DEBUG 0

#if !defined(DEBUG) || DEBUG == 0
#include "_start.h"
int *terminal32 = (int *)(0x88888888 >> 2);
unsigned int strlen(const char *str);
void putchar(int c) { *terminal32 = c; }

/* @Attention, to garantee puts will work the str has to be 4 byte aligned :)
  That means every string will in the whole program have to be padded with zero
  to ensure every string is a multiple of 4 hehe, remember that last byte of str
  literal is always zero
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
    if (((word >> 16) & 0xFF) == 0) {
      return;
    }
    putchar((word >> 16) & 0xFF);
    if (((word >> 8) & 0xFF) == 0) {
      return;
    }
    putchar((word >> 8) & 0xFF);
    if (((word >> 0) & 0xFF) == 0) {
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

typedef struct {
  int x, y;
} vector2i;

int dot(vector2i v1, vector2i v2) { return v1.x * v2.x + v1.y * v2.y; }

void bubble_sort(int arr[], int n) {
  int temp;
  int swapped;

  for (int i = 0; i < n - 1; i++) {
    swapped = 0;

    for (int j = 0; j < n - 1 - i; j++) {
      // Compare adjacent elements
      if (arr[j] > arr[j + 1]) {
        // Swap them if they are in the wrong order
        temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
        swapped = 1; // Set the swapped flag
      }
    }

    // If no two elements were swapped in this pass, the array is sorted
    if (swapped == 0) {
      break;
    }
  }
}

void printi(int num) {
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
    // putchar('+');
  }
  // Extract and print each digit
  while (divisor > 0) {
    int digit = (num / divisor) % 10;
    putchar('0' + digit);
    divisor /= 10;
  }
}

int struc_play(void) {
  vector2i v, r;
  v.x = 0x1;
  v.y = 0x2;

  r.x = 0x3;
  r.y = 0x4;

  puts("\nstruct -> ");
  printi(r.x);
  int d = dot(v, r);
  puts("\ndot    -> ");
  printi(d);
  return d;
}

int main() {
  int arr[] = {-1, -564, 420, 69, 1200};
  int n = sizeof(arr) / sizeof(arr[0]);
  int i, a;

  printi(arr[2]);
  for (i = 0; i < n; i++) {
    a = arr[i];
    printi(a);
    putchar(' ');
  }

  putchar('\n');
  bubble_sort(arr, n);

  for (i = 0; i < n; i++) {
    a = arr[i];
    printi(a);
    putchar(' ');
  }
  puts("ccc");
  puts("aaabbbb");
  // return struc_play();
}

#else
#include "_start.h"

int *terminal32 = (int *)(0x88888888 >> 2);
void putchar(int c) { *terminal32 = c; }
// void memmove(void){}
// void memset(void){}

int main(void);
typedef struct {
  int x, y;
} vector2i;

// int dot(vector2i v1, vector2i v2) { return v1.x * v2.x + v1.y * v2.y; }
int sum(vector2i v1) { return v1.x + v1.x;}


int main(void) {
  // vector2i v1 = {.x = 1, .y = 2}, v2 = {.x = 7, .y = 11};
  vector2i v1 = {.x = 1, .y = 2};
  return sum(v1);
}

#endif
