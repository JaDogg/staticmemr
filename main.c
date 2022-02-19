#define STMMR_IMPL
#include "stmmr.h"
#include <stdio.h>
struct point {
  int64_t x;
  int64_t y;
  int64_t a;
  int64_t b;
};
int main(void) {
  stmmr_init();
  for (int i = 0; i < 30000; i++) {
    struct point *p = stmmr_alloc(sizeof(struct point) * 5);
    if (p) {
      p[0].a = 1;
      p[0].b = 2;
      p[0].x = 3;
      p[0].y = 4;
      struct point *px = stmmr_realloc(p, 10);
      if (p != px) { printf("realloc did not allocate memory \n"); }
      p = stmmr_realloc(p, sizeof(struct point) * 10);
      if (p) {
        printf("%lld\n", p[0].x);
        p[2].a = 10;
        p[2].b = 20;
        p[2].x = 30;
        p[2].y = 40;
        printf("%lld, ", p[2].a);
        printf("%lld, ", p[2].b);
        printf("%lld, ", p[2].x);
        printf("%lld\n", p[2].y);
      }
      stmmr_free(p);
    } else {
      printf("Failed to memory allocate\n");
    }
    uint8_t *pp = stmmr_calloc(1, 100);
    if (pp) {
      for (int j = 0; j < 100; j++) {
        if (pp[j] != 0) { printf("j=%d\n", j); }
      }
    }
    if (i % 2 == 0) { stmmr_free(pp); }
  }
  return 0;
}
