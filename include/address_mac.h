#pragma once

#include <stdint.h>
#include "errors.h"

#define MAC_NB_OCTET 6
#define SIZE_MAC_STRING 17

/* Type pour l adresse MAC sur 6 octets */
struct mac{
    uint8_t mac_array[MAC_NB_OCTET];
};

struct optional_mac{
    struct mac mac;
    enum EXIT_CODES exit_code;
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

/**
 * Convertie une représentation en chaine de caractére d'une adresse mac 
 * en struct optional_mac.
 */
struct optional_mac mac_from_str(const char* mac_str);

/**
 * Retourne une adresse mac en entier de 64 bit.
 */
uint64_t mac_to_long(struct mac mac);

/*
* Compare deux addresse mac et retourne true si les deux sont identiques.
*/
bool mac_equals(struct mac first, struct mac second);