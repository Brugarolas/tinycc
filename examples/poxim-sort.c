#define DEBUG 0

#if !defined(DEBUG) || DEBUG == 0

#include "_start.h"
int *terminal32 = (int *)(0x88888888 >> 2);
void putchar(int c) { *terminal32 = c; }

// // // typedef struct {
// // //   int x, y;
// // // } vector2i ;
// // //
// // //
// // //
// // // int dot(vector2i v1, vector2i v2) {
// // //   return v1.x * v2.x + v1.y*v2.y;
// // // }
// // //
// // // int struc_play(void) {
// // //   vector2i v,r;
// // //   v.x = 0x1;
// // //   v.y = 0x1;
// // //
// // //   r.x = 0x2;
// // //   r.y = 0x2;
// // //
// // //   return dot(v, r);
// // // }
// //

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


int main() {
  int arr[] = {-1, -564, 420, 69, 1200 };
  int n = sizeof(arr) / sizeof(arr[0]);
  int i, a;

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
  return i;
}

#else


// void memset();

int main(void) {
  int arr[] = {0xf1, 0xf2, 0xf3, 0xf4, 0xf5};
  int n = sizeof(arr) / sizeof(arr[0]);

  int i, a;
  for (i = 0; i < n; i++) {
    a = arr[i];
  }
  return a;
}

void memset(void* ptr, int val, int size) {}

#endif
