#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "address_ip.h"
#include "errors.h"

int main(int argc, char *argv[]) {
  
  struct ip ip = init_ip(192, 12, 11, 13);

  char* s = NULL;
  ip_to_string(&s, ip);

  struct optional_ip ip_o = ip_from_string(s);
  error_handler_crash(ip_o.exit_code);

  struct ip ip2 = ip_o.ip;

  printf("notre ip : %s\n", s);
  printf("o1 : %d\n", ip_get_octet(ip2, OCTET_1));
  printf("o2 : %d\n", ip_get_octet(ip2, OCTET_2));
  printf("o3 : %d\n", ip_get_octet(ip2, OCTET_3));
  printf("o4 : %d\n", ip_get_octet(ip2, OCTET_4));


  struct optional_ip ip_o2 = ip_from_string("192.0.34.10");
  error_handler_crash(ip_o2.exit_code);

  struct ip ip3 = ip_o2.ip;

  printf("notre ip : 192.0.34.10\n");
  printf("o1 : %d\n", ip_get_octet(ip3, OCTET_1));
  printf("o2 : %d\n", ip_get_octet(ip3, OCTET_2));
  printf("o3 : %d\n", ip_get_octet(ip3, OCTET_3));
  printf("o4 : %d\n", ip_get_octet(ip3, OCTET_4));

  free(s);

  return 0;
}
