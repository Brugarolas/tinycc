#include "_start.h"

int main(void);
typedef struct {
  int x, y;
} vector2i;

// int dot(vector2i v1, vector2i v2) { return v1.x * v2.x + v1.y * v2.y; }
int sum(vector2i v) { return v.x + v.y;}


int main(void) {
  // vector2i v1 = {.x = 1, .y = 2}, v2 = {.x = 7, .y = 11};
  vector2i v1 = {.x = 0xc1c1, .y = 0xc2};
  vector2i v2;
  memmove(&v2, &v1, sizeof(vector2i));
  return sum(v1); // r2 should have 0xc1c1 + 0xc2
}
