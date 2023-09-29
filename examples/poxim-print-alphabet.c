#include "_start.h"

int* terminal32 = (int*)(0x88888888 >> 2);

int loop(int n) {
  int i = 0;
  while ( i < n ) {
    int c = i + 'a';
    *terminal32 = c;
    i += 1;
  }
  return i;
}

int main(void) {

  int alphabet_size = 26; 
  // should print on the terminal abcdefghijklmnopqrstuvwxyz
  return loop(alphabet_size);
}

