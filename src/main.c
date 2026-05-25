#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s config_file\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  init_net_from_file(argv[1]);
  return 0;
}
