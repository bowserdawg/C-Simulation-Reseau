#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "address_ip.h"

int main(int argc, char *argv[]) {
  
  struct ip ip = init_ip(192, 122, 111, 123);

  char* s = NULL;
  ip_to_string(&s, ip);

  printf("notre ip : %s\n", s);
  printf("o1 : %d\n", getOctet(ip, OCTET_1));
  printf("o2 : %d\n", getOctet(ip, OCTET_2));
  printf("o3 : %d\n", getOctet(ip, OCTET_3));
  printf("o4 : %d\n", getOctet(ip, OCTET_4));

  free(s);

  return 0;
}
