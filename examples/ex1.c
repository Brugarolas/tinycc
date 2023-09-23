#!/usr/local/bin/tcc -run
#include <tcclib.h>
int mycock;
int main(argc, argv)
  int argc;
  char** argv;
{
    const int a = 2;
    (void)a;
    printf("Hello World\n");
    return 0;
}
