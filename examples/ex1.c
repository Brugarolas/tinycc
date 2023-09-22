#!/usr/local/bin/tcc -run
#include <tcclib.h>

int main(argc, argv)
  int argc;
  char** argv;
{
    int a = 2;
    (void)a;
    printf("Hello World\n");
    return 0;
}
