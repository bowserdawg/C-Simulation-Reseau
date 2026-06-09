#pragma once

#include "errors.h"
#include "address_mac.h"

#include <stddef.h>

//typedef size_t port_t;
#define INVALID_PORT UINT64_MAX

// Faut faire les typedef avant l'include de table_commutation.h
// Sinon, ca fait une espece de dependence circulaire assez bizarre.
typedef size_t port_t;
typedef uint16_t priority_t;

struct table_entry {
    port_t nbPort;
    struct mac addrs;
};

struct table_commutation
{
    struct table_entry* data;
    size_t size;
    size_t nb_elems;
};

struct optional_table_commutation
{
    struct table_commutation table;
    enum EXIT_CODES exit_code;
};

/**
 * Creation d'une nouvelle table de commutation avec taille dynamique pour un switch.
 * 
 *      - inital_capacity : Capaicte initiale du table de commutation.
 * 
 * retourne : un "struct optional_table_commutation" qui peut contenir un code d'errur en cas d'erreur
 */
struct optional_table_commutation init_table_commutation(size_t inital_capacity);

/**
 * Ajoute un élement à la table de commutation.
 */
enum EXIT_CODES table_commutation_add(struct table_commutation* table, port_t port, struct mac address);

/**
 * Retourne "INVALID_PORT" si l'addresse ne se trouve pas dans la table.
 */
struct optional_port table_commutation_lookup(struct table_commutation table, struct mac mac);


/**
 * Destruction d'une table de commutation.
 * 
 *      - table : Capaicte initiale du table de commutation.
 */
void deinit_table_commutation(struct table_commutation* table);