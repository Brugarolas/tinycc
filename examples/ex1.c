#!/usr/local/bin/tcc -run
#include <tcclib.h>
int mycock;


int main(argc, argv)
  int argc;
  char** argv;
{
    const int a = 2;
    const int b = 2;
    (void)a;
    (void)b;
    // printf("Hello World\n");
    return a+b;
}

void _start(void){
  main(0 , &"");
}
