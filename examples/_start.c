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

int main(void) {
    int a = 420;
    int b = 69;
    oi();
    int0();
    return a+b;
}

int oi(void) {  return 0x69;}
