#include "_start.h"

// endianess is fucked
static int terminal32 = (int)(0xcafebabe);
int lol = (int)              (0xabcdef);


int main(void) {
  int a = terminal32;
}
