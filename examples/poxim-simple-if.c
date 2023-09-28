#include "_start.h"

int main(void) {
  int a = 7;
  int b = 3;
  if (a > b) {
    b = 0x69;
    a = 0x68;
  }

  if (a < b) { // true
    b = 7;// b = 7;
  }
  return b;
}


