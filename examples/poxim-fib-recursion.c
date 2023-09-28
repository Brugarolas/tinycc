#include "_start.h"

int fibonnacci(int n) {
  if (n < 2) {
    return n;
  }
  return fibonnacci(n - 1) +  fibonnacci(n - 2);
}

int main(void) {
  int a = fibonnacci(6);// = 8 
  int b = fibonnacci(5); // = 5
  return a+b; // r2 should have 13, 0xd
}

