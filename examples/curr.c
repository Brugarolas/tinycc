int main(void);
void oi(int a, int b, int c, int d, int e) { }
int main(void) {
	oi(1,2,3,4,5);
}
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
void _start(void) {
   main(); int0();
} 



