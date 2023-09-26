#include "_start.h"


/* Parameter passing is working */
int oi2(int a, int b) { return (a-b);}
int oi(int a, int b, int c, int d, int e) { return (a+b+c+d-e);}

int main(void) {
	int a = 2;
	int b = oi(1,2,3,4,5) - 69;
  oi2(a, b);
	return a + b;

}
