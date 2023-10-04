#define NULL (void*)0
/* ATTENTION: 
  This has to be the first instructions in the final binary (address 0x0000),
  just include in the beginning of the main .c file
*/
asm (                                                                                
"_init:\n"
		".long 0xFC7FC003;\n"/* mov sp, 0x7FFC */
		".long 0x010000DC;\n" /* bun 1 ; jump the next instruction which is for quitting the program */
"int0:\n" /* Label for exiting */
		".long 0x000000FC;"/* int 0 */
);

/* size_t in this is just a unsigned int */
typedef  unsigned int size_t;

void int0(void);
int  main(void);

/* Needed for initializing an array on the stack */
void *memset(void *ptr, int value, size_t size); 
/* Needed for copying an struct on the stack for argument passing */
int   memmove(void *, const void *, size_t );

/* TCC will look for this label to start everything */
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

