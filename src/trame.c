#include "address_mac.h"
#include "address_ip.h"
#include "trame.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BOURRAGE        46
#define TAILLE_PREAMBULE 7
/*
   Offset dans la trame brute où commencent les données couvertes par le FCS
   (après préambule 7 octets + SFD 1 octet) 
*/
#define OFFSET_FCS_START (TAILLE_PREAMBULE + 1)

struct trame_before_data {
    octet preambule[TAILLE_PREAMBULE];
    octet SFD;
    struct mac destination;
    struct mac source;
    octet type[2];
};

struct trame_after_data {
    octet FCS[4];
};

/* ---------- CRC-32 (polynôme IEEE 802.3 : 0xEDB88320) ---------- */
static uint32_t crc32(const void *buf, size_t len)
{
    const uint8_t *p = (const uint8_t *)buf;
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < len; i++) {
        crc ^= p[i];
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;   /* polynôme réfléchi */
            else
                crc >>= 1;
        }
    }
    return crc ^ 0xFFFFFFFF;   /* complément final */
}
/* --------------------------------------------------------------- */


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
                        protocol_t protocol)
{
    size_t original_data_size = data_size;

    /* Bourrage minimal Ethernet : la charge utile doit faire au moins 46 octets */
    if (data_size < BOURRAGE)
        data_size = BOURRAGE;

    struct trame tr;
    tr.size = sizeof(struct trame_before_data)
            + data_size
            + sizeof(struct trame_after_data);

    // tr.trame
    tr.trame = malloc(tr.size);

    /* --- En-tête --- */
    struct trame_before_data *trame_part1 = (struct trame_before_data *)tr.trame;

    // Preambule
    memset(trame_part1->preambule, 0xAA, TAILLE_PREAMBULE);  /* 10101010 × 7 */
    // SFD
    trame_part1->SFD = 0xAB;                                 /* 10101011     */
    // Mac destination
    trame_part1->destination = dest;
    // Mac source
    trame_part1->source      = src;

    trame_part1->type[0] = (protocol >> 8) & 0xFF;
    trame_part1->type[1] =  protocol       & 0xFF;

    /* --- Données (+ zero-padding si bourrage) --- */
    octet *payload = (octet *)tr.trame + sizeof(struct trame_before_data);
    memset(payload, 0, data_size);                    /* zero-fill le bourrage */
    memcpy(payload, data, original_data_size);        /* copie les vraies données */

    /* --- FCS : CRC-32 sur tout ce qui suit le SFD --- */
    /* Zone couverte : destination + source + type + données  */
    const void  *fcs_start = tr.trame + OFFSET_FCS_START;
    size_t       fcs_len   = sizeof(struct trame_before_data) - OFFSET_FCS_START
                           + data_size;

    uint32_t crc = crc32(fcs_start, fcs_len);

    struct trame_after_data *ftr =
        (struct trame_after_data *)(tr.trame
                                    + sizeof(struct trame_before_data)
                                    + data_size);

    /* Stockage en little-endian (convention CRC Ethernet apparemment) */
    ftr->FCS[0] =  crc        & 0xFF;
    ftr->FCS[1] = (crc >>  8) & 0xFF;
    ftr->FCS[2] = (crc >> 16) & 0xFF;
    ftr->FCS[3] = (crc >> 24) & 0xFF;

    return tr;
}

/**   
*    Utilitaire interne : pointeur vers l'en-tête                      
*/
static const struct trame_before_data *get_header(const struct trame tr)
{
    return (const struct trame_before_data *)tr.trame;
}

/**
 * trame_get_destination                                                     
 *  Retourne l'adresse MAC de destination
 */
struct mac trame_get_destination(const struct trame tr)
{
    return get_header(tr)->destination;
}

/**
 * trame_get_source 
 * Retourne l'adresse MAC source. 
 */
struct mac trame_get_source(const struct trame tr)
{
    return get_header(tr)->source;
}


/**
 * get_protocol 
 * Retourne le protocole (2 octets, big-endian). 
 */
protocol_t trame_get_protocol(const struct trame tr)
{
    const struct trame_before_data *hdr = get_header(tr);
    return (protocol_t)((hdr->type[0] << 8) | hdr->type[1]);
}

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
size_t trame_get_data(const struct trame tr, const void **data)
{
    /* Le bloc DATA démarre juste après l'en-tête fixe. */
    *data = (const void *)(tr.trame + sizeof(struct trame_before_data));

    /* Taille = taille totale − en-tête − FCS */
    return tr.size
           - sizeof(struct trame_before_data)
           - sizeof(struct trame_after_data);
}

/**
 * Retourne le FCS stocké dans la trame (valeur brute 32 bits,
 * little-endian tel que stocké par init_trame). 
 */
uint32_t trame_get_fcs(const struct trame tr)
{
    const struct trame_after_data *ftr =
        (const struct trame_after_data *)(tr.trame
                                          + tr.size
                                          - sizeof(struct trame_after_data));

    return (uint32_t) ftr->FCS[0]
         | (uint32_t)(ftr->FCS[1] <<  8)
         | (uint32_t)(ftr->FCS[2] << 16)
         | (uint32_t)(ftr->FCS[3] << 24);
}

/**
 *  Fonction bonus généré par l'IA.
 *  "Vérifie l'intégrité de la trame en recalculant le CRC-32 et en   
 *  le comparant au FCS stocké. Retourne true si la trame est saine"
 */
bool trame_is_fcs_valid(const struct trame tr)
{
    /* Même zone que dans init_trame */
    const void *fcs_start = tr.trame + OFFSET_FCS_START;
    size_t fcs_len = tr.size
                     - OFFSET_FCS_START
                     - sizeof(struct trame_after_data);

    return crc32(fcs_start, fcs_len) == trame_get_fcs(tr);
}

/**
 * Détruit la trame!
 */
void deinit_trame(struct trame tr)
{
    free(tr.trame);
}