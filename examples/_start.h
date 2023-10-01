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
int memmove(void *, const void *, size_t );

void _start(void) {
    main();
    int0();
} 


void *memset(void *ptr, int value, size_t size) {
  int *p = (int*)ptr;
  for (size_t _ = 0; _ < size / sizeof(int); _++) {
    *p++ = value;
  }
  return ptr;
}

int memmove(void * dst, const void *src, size_t size) {
  int *idst = (int*)dst;
  int *isrc = (int*)src;
  for (size_t i = 0; i < size / sizeof(int); i++) {
     *idst++ = *isrc++;
  }
  return size;
}

