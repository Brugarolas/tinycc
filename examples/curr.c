#include "_start.h"

// endianess is fucked
int terminal = 0xcafebabe;
int main(void) {
  int a = terminal;
  int b = 0xfafe;
	return a+b;

}
