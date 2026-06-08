#pragma once

#include "address_mac.h"

#include <stdlib.h>
#include <stdint.h>

typedef unsigned char octet;
typedef uint16_t protocol_t;

struct trame {
    char  *trame;  // l'utilisateur peut modifier la trame mais on s'en fout. C'est leur probleme.
    size_t size;
};

/**
* Création d'une nouvelle trame Ethernet.
*
*      data_size : Taille de la section DATA.
*
*      data      : Pointeur vers le DATA en question.
*
*      dest      : Adresse mac destination.
*
*      src       : Adresse mac source.
*
*      protcol_t : Le protocole de la trame.
*
* Retourne : Une nouvelle trame ethernet.
*/
struct trame init_trame(size_t data_size, const void *data,
                        struct mac dest, struct mac src,
                        protocol_t protocol);

/**
 * trame_get_destination                                                     
 *  Retourne l'adresse MAC de destination
 */
struct mac trame_get_destination(const struct trame tr);

/**
 * trame_get_source 
 * Retourne l'adresse MAC source. 
 */
struct mac trame_get_source(const struct trame tr);

/**
 * get_protocol 
 * Retourne le protocole (2 octets, big-endian). 
 */
protocol_t trame_get_protocol(const struct trame tr);

/**
 * Fournit un pointeur (en lecture seule) vers le bloc DATA de la     
 * trame et retourne sa taille en octets.
 * 
 *      tr - trame
 *      
 *      data : pointeur-par-référence (double pointeur vers const) qui sera       
 *             positionné sur le début du champ DATA
 * 
 * Retourne : La taille du bloc DATA.
 */
size_t trame_get_data(const struct trame tr, const void **data);

/**
 * Retourne le FCS stocké dans la trame (valeur brute 32 bits,
 * little-endian tel que stocké par init_trame). 
 */
uint32_t trame_get_fcs(const struct trame tr);

/**
 *  Fonction bonus généré par l'IA.
 *  "Vérifie l'intégrité de la trame en recalculant le CRC-32 et en   
 *  le comparant au FCS stocké. Retourne true si la trame est saine"
 */
bool trame_is_fcs_valid(const struct trame tr);

/**
 * Détruit la trame!
 */
void deinit_trame(struct trame tr);