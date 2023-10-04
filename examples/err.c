// #include "_start.h" /* default init, calls main */
// #define PRINT_IMPLEMENTATION /* Makes print.h act as .c file */
// #include "print.h" /* puts, puti, strlen, print_arr, print_matrix */

void memset(){}
int main() {
  int arr[3] = {1,2,3};
  // puti((arr[0xffff + 0xf]));

  int a = 0xffff;
  unsigned int a1 = 0xffff;
  unsigned int a2 = 0xffff;
  unsigned int a3 = a1*a2;
  return 0;
}


