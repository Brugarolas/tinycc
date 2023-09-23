#!/usr/local/bin/tcc -run
#include <tcclib.h>

int mycock;
int lol;
int main(argc, argv);

void _start(void){
  mycock = 69;
  main(0 , &"");
}

int main(argc, argv)
  int argc;
  char** argv;
{
    lol = 420;
    const int a = 2;
    const int b = 2;
    (void)a;
    (void)b;
    // printf("Hello World\n");
    return a+b;
}

