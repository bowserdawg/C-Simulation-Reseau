
#include <stdio.h>
#include <stdlib.h>
int main() {
  printf("%d\n", (int)strtol("1A", NULL, 16));
  return 0;
}