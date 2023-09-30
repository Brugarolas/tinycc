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

typedef  unsigned int size_t;
void int0(void);

int  main(void);
void *memset(void *ptr, int value, size_t size);
int memmove(void *, const void *, int );
void _start(void) {
    main();
    int0();
} 


void *memset(void *ptr, int value, size_t size) {
  int *p = (int*)ptr;
  for (size_t i = 0; i < size / sizeof(int); i++) {
    *p++ = value;
  }
  return ptr;
}


int memmove(void * dst, const void *src, int  num) {
   unsigned char *pdst = (unsigned char *)dst;
   const unsigned char *psrc = (const unsigned char *)src;
   while (num--) {
        *pdst++ = *psrc++;
    }
    return dst;
}
