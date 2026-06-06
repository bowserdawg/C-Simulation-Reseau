#include "address_mac.h"
#include "errors.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define int64ToMac(INT_MAC, X) (INT_MAC >> (X * 8)) & 0xFF

struct mac init_mac(octet_mac_t o1, octet_mac_t o2, octet_mac_t o3, octet_mac_t o4, octet_mac_t o5, octet_mac_t o6)
{
    return (struct mac) {
       { 
        o1,
        o2,
        o3,
        o4,
        o5,
        o6 
       }
    };
}

void display_mac(struct mac mac) {
  char mac_string[SIZE_MAC_STRING];
  for (size_t i = 0; i < 6; i++) 
  {
    printf("%02X:", mac.mac_array[i]);
    if (i == 5) 
    {
      printf("%02X\n", mac.mac_array[i]);
    }
  }
}

// Doit etre free !!
enum EXIT_CODES mac_to_string(char** string, struct mac mac)
{
    char* new_str = calloc(SIZE_MAC_STRING + 1, sizeof(char));
    if(new_str == NULL)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return ERROR_BAD_ALLOC;
    }

    uint8_t arr[] = {mac.mac_array[0],mac.mac_array[1],
                    mac.mac_array[2], mac.mac_array[3],
                    mac.mac_array[4],mac.mac_array[5]};

    snprintf(new_str, SIZE_MAC_STRING + 1, 
        "%02X:%02X:%02X:%02X:%02X:%02X",
        arr[0],
        arr[1],
        arr[2],
        arr[3],
        arr[4],
        arr[5]
    );
    // AA:a:a:a:a:a

    // 86:1A:A1:74:C0:11

    new_str[SIZE_MAC_STRING] = '\0';

    *string = new_str;
    
    return SUCESS;
}

struct mac mac_operation_and(struct mac mac, struct mac mask)
{
    struct mac new_mac;
    for(size_t i = 0; i < 6; ++i)
    {
        new_mac.mac_array[i] = mac.mac_array[i] & mask.mac_array[i];
    }
    return new_mac;
}

struct mac mac_from_string(char *mac_string) {
  // on initialise une struct de mac vide
  struct mac mac = {{0, 0, 0, 0, 0, 0}};

  size_t byteIndex = 0;
  // on parcours les char de la ligne pour récuperer le mac
  // on utilise SIZE_MAC_STRING parce que le mac est toujours de la même taille
  // (6 octet)
  for (size_t i = 0; i < SIZE_MAC_STRING; i += 3) 
  {

    // on initialise un tableau de 3 char pour les 2 char de l'octet en hexa et
    // le char de fin '\0'
    char byte_string[3];

    // on recupère les deux lettres en hexa
    strncpy(byte_string, mac_string + i, 2);
    // on ajoute le char de fin de chaine
    byte_string[2] = '\0';

    // on ajoute le byte dans la struct
    mac.mac_array[byteIndex] = (uint8_t)strtol(byte_string, NULL, 16);

    byteIndex++;
  }

  return mac;
}