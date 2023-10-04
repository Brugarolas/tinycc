#include "_start.h" /* default init, calls main */
#define PRINT_IMPLEMENTATION /* Makes print.h act as .c file */
#include "print.h" /* puts, puti, strlen, print_arr, print_matrix */

#define MAX_TEST 20
int main(void) {
  int bools[MAX_TEST] = { 0 };

  int a = 1;
  int b = 2;
  int c = 3;
  int d = 4;
  int e = 5;
  int f = 6;

  int idx = 0;
  bools[idx++] = (a > b);                     // false
  bools[idx++] = (a < b) && b + 2 == 4;       // true
  bools[idx++] = (a < b) && b + 2 != 4;       // false
  bools[idx++] = (b >= b) || e != 5 ;         // true
  bools[idx++] = (b >= b) && e != 5 ;         // false
  bools[idx++] = (e != 5 || d != 4 || 1);     // true
  bools[idx++] = (e != 5 || d != 4 && 1);     // false
  bools[idx++] = (e < 5 || d < 4 || a < b);   // true

  for (int i = 0; i < idx && i < MAX_TEST; i++) {
    putbool(bools[i]);
    putchar('\n');
  }

  return 0;
}


