#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "address_ip.h"

int main(int argc, char *argv[]) {
  
  struct ip* ip = init_ip(192, 12, 21, 0);

  char* s = NULL;
  ip_to_string(&s, ip);

  printf("notre ip : %s\n", s);

  deinit_ip(&ip);

  free(s);

  return 0;
}
