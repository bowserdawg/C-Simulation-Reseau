#include "../include/parser.h"
#include "errors.h"
#include "address_ip.h"
#include "address_mac.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  struct mac mac = init_mac(134, 26, 161, 116, 192, 17);

  char* ptr = NULL;
  enum EXIT_CODES code = mac_to_string(&ptr, mac);
  DEBUG_ABORT_IF_ERREUR(code);

  mac = mac_from_string(ptr);

  char* ptr2 = NULL;
  code = mac_to_string(&ptr2, mac);
  DEBUG_ABORT_IF_ERREUR(code);

  printf("%s\n", ptr);

  free(ptr);
  free(ptr2);

  return 0;
}
