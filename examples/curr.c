#include "_start.h"


int oi3(int *a) { return *a = 420;}
int oi2(int a, int b) { return (a-b);}
int oi(int a, int b, int c, int d, int e) { return (a+b+c+d-e);}

int main(void) {
	int a = 2;
  int b; 
	b = oi(1,2,8,16,5);
  // oi2(a, b);
  oi3(&b);
	return a + b;

}
