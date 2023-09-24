asm (                                                                                
"_init:\n"
		/* mov sp, 0x7FFC */
		".long 0xFC7FC003;\n"
		".long 0x010000DC;\n"
);
void int0(void); asm (                                                                                
"int0:\n"
		/* int 0 */
		".long 0x000000FC;"
);
int main(void);
void _start(void) {
   main(); int0();
} 

int oi(void);

int main(void)
{
    // const int a = 1;
    // const int b = 2;
    // (void)a;
    // (void)b;
    // int0();
    // oi();
    // return a+b;
}

int oi(void) {  return 0x69;}
