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



int oi(int a, int b, int c, int d, int e) { return (a+b+c+d+e);}

int main(void) {
	int a = 2;
	int b = oi(1,2,3,4,5) - 69;
	return a + b;

}
