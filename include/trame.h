#pragma once

#include "address_mac.h"
#include "errors.h"

#include <stdlib.h>
#include <stdint.h>

typedef unsigned char octet;
typedef uint16_t protocol_t;

struct trame {
    char  *trame;  // l'utilisateur peut modifier la trame mais on s'en fout. C'est leur probleme.
    size_t size;
};

struct optional_tram {
    struct trame tr;
    enum EXIT_CODES exit_code;
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
* Retourne : Une optional d'une nouvelle trame ethernet.
*/
struct optional_tram init_trame(size_t data_size, const void *data,
                        struct mac dest, struct mac src,
                        protocol_t protocol);

/**
 * Retoune une copie profonde d'une trame existante.
 */
struct optional_tram trame_copy(const struct trame src);

/**
 * trame_get_destination                                                     
 *  Retourne l'adresse MAC de destination
 */
struct mac trame_get_destination(struct trame tr);

/**
 * trame_get_source 
 * Retourne l'adresse MAC source. 
 */
struct mac trame_get_source(struct trame tr);

/**
 * get_protocol 
 * Retourne le protocole (2 octets, big-endian). 
 */
protocol_t trame_get_protocol(struct trame tr);

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
size_t trame_get_data(struct trame tr, const void **data);

/**
 * Retourne le FCS stocké dans la trame (valeur brute 32 bits,
 * little-endian tel que stocké par init_trame). 
 */
uint32_t trame_get_fcs(struct trame tr);

/**
 *  Fonction bonus généré par l'IA.
 *  "Vérifie l'intégrité de la trame en recalculant le CRC-32 et en   
 *  le comparant au FCS stocké. Retourne true si la trame est saine"
 */
bool trame_is_fcs_valid(struct trame tr);

/**
 * Donne le nom en chaine de caractére du protocol.
 */
const char* trame_get_protocol_str_name(protocol_t protocol);

/**
 *  Retourne une chaîne allouée dynamiquement contenant la             
 *  représentation hexadécimale de chaque octet de la trame,           
 *  séparés par des espaces.  
 *  Format : "AA BB CC DD ..."
 * 
 *  tr : La trame qui va etre affiché.
 * 
 *  chaine : La chaine qui va avoir la représentation en héxadécimale de la chaine.
 * 
 *  Retourne : le code de sortie de la fonction.        
 */
enum EXIT_CODES trame_to_hex(struct trame tr, char** chaine);

/**
 * La légende de la trame obtenu avec trame_to_hex_ansi(struct trame tr)
 * 
 */
const char* trame_get_colored_legend();

/**
 * Donne une chaine de caractére avec des couleurs différents pour chaque partie !
 * Utilise les caractéres ANSI.
 * 
 * Voir aussi TRAME_LEGENDE pour la légende du train.
 * 
 *      - tr : La trame qui va etre affiché. 
 * 
 *      - chaine : La chaine de la représentation de la trame en ANSI.
 * 
 * Retourne : Un code d'erreur.
 */
enum EXIT_CODES trame_to_hex_ansi(struct trame tr, char** string);

/**
 * Détruit la trame!
 */
void deinit_trame(struct trame* tr);