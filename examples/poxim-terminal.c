#include "_start.h"

// endianess is fucked
int* terminal32 = (int*)(0x88888888 >> 2);

int main(void) {
  *terminal32 = 'e';
  *terminal32 = 'u';
  *terminal32 = ' ';
  *terminal32 = 't';
  *terminal32 = 'e';
  *terminal32 = ' ';
  *terminal32 = 'a';
  *terminal32 = 'm';
  *terminal32 = 'o';
  *terminal32 = '!';

}
