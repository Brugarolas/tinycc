#include "_start.h"

int loop(int n) {
  int i = 0;
  while ( i < n ) {
    i += 1;
  }
  return i;
}

int main(void) {
  int n = 0x69;
  return loop(n); // r2 should have 0x69
}

