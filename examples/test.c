typedef int size_t;
void *memset(void *ptr, int value, size_t size) {
  int *p = (int*)ptr;
  for (size_t _ = 0; _ < size / sizeof(int); _++) {
    *p++ = value;
  }
  return ptr;
}

