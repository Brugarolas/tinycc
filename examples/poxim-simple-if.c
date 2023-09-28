#include "_start.h"

int main(void) {
  int a = 7;
  int b = 3;
  if (a > b) {
    b = 0x69;
    a = 420;
  }
  return b;
}
