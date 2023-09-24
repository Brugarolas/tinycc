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
// Loading immediate integer is actually 20bits max
int main(void)
{
    int a = 420;
    int b = 0x69;
    int c = a+b;
    c += oi();
    return 0xcafe;
}

int oi(void) {  return 0x69;}
