#include "_start.h" /* default init, calls main */
#define PRINT_IMPLEMENTATION /* Makes print.h act as .c file */
#include "print.h" /* puts, puti, strlen, print_arr, print_matrix */


typedef struct {
  int a1, a2;
} A;

typedef struct {
  A a;
  int b1, b2, b3;
} B;

typedef struct {
  B b;
  int c1, c2, c3, c4;
} C;

 // Does not Work correctly Don't use it
void wrong_init_c(C* c){
    c->b.a.a1 = 1;
    c->b.a.a2 = 2;
    c->b.b1 = 3;
    c->b.b2 = 4;
    c->b.b3 = 5;
    c->c1 = 6;
    c->c2 = 7;
    c->c3 = 8;
    c->c4 = 9;
}

// Works, but hack
void init_c(C* c){
    *(int*)c = 1;
    *(int*)(c+1) = 2;
    *(int*)(c+2) = 3;
    *(int*)(c+3) = 4;
    *(int*)(c+4) = 5;
    *(int*)(c+5) = 6;
    *(int*)(c+6) = 7;
    *(int*)(c+7) = 8;
    *(int*)(c+8) = 9;
}

void print_c(C c){
    puti(c.b.a.a1);
    puti(c.b.a.a2);
    puti(c.b.b1);
    puti(c.b.b2);
    puti(c.b.b3);
    puti(c.c1);
    puti(c.c2);
    puti(c.c3);
    puti(c.c4);
};

int main() {
  int cafe = 0xcafebabe;
  puti(cafe);
  puts("Main Struct C:\n");
  C c;
  c.b.a.a1 = ~1;
  c.b.a.a2 = 2;
  c.b.b1 = 3;
  c.b.b2 = 4;
  c.b.b3 = 5;
  c.c1 = 6;
  c.c2 = 7;
  c.c3 = 8;
  c.c4 = 9;
  print_c(c);
  int a = 2, b = 3;
  puti(a ^ b);
}
