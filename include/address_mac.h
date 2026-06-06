#pragma once

#include <stdint.h>
#include "errors.h"

#define SIZE_MAC_STRING 17

/* Type pour l adresse MAC sur 6 octets */
struct mac{
    uint8_t mac_array[6];
};


typedef uint8_t octet_mac_t;


struct mac init_mac(octet_mac_t o1, octet_mac_t o2, octet_mac_t o3, octet_mac_t o4, octet_mac_t o5, octet_mac_t o6);

// A effacer
void display_mac(struct mac mac);

// retourne l'adresse mac avec le masque appliqué
struct mac mac_operation_and(struct mac mac, struct mac mask);

// Creer une IP d'une adresse IP
struct mac mac_from_string(char *mac_string);

enum EXIT_CODES mac_to_string(char** string, struct mac mac);