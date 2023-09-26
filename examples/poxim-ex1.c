#include "_start.h"

// char* hello = "dd"; /* this will cause a assert failed*/
int oi(int a, int b, int c, int d, int e) { return (a+b+c+d-e);}

int main(void) {
	int a = 2;
	int b = oi(1,2,3,4,5) - 69;
	return a + b;

}
