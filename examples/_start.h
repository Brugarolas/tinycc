#define __poxim__
asm (                                                                                
"_init:\n"
		/* mov sp, 0x7FFC */
		".long 0xFC7FC003;\n"
		".long 0x010000DC;\n"
"int0:\n"
		/* int 0 */
		".long 0x000000FC;"
);

void int0(void);

int  main(void);
void *memset(void *ptr, int value, int size);
void _start(void) {
    main();
    int0();

} 

void *memset(void *ptr, int value, int size) {
  unsigned char *p = (unsigned char *)ptr;
  while (size--) {
    *p++ = value;
  }
  return ptr;
}
