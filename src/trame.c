#include "address_mac.h"
#include "address_ip.h"
#include "trame.h"
#include "global.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define BOURRAGE        46
#define TAILLE_PREAMBULE 7
/*
   Offset dans la trame brute où commencent les données couvertes par le FCS
   (après préambule 7 octets + SFD 1 octet) 
*/
#define OFFSET_FCS_START (TAILLE_PREAMBULE + 1)

/* ------------------------------------------------------------------ */
/*  Codes couleur ANSI                                                  */
/* ------------------------------------------------------------------ */
#define ANSI_RESET      "\x1b[0m"
#define ANSI_BOLD       "\x1b[1m"

#define ANSI_PREAMBULE  "\x1b[38;5;240m"   /* Gris foncé               */
#define ANSI_SFD        "\x1b[38;5;214m"   /* Orange                   */
#define ANSI_DST_MAC    "\x1b[38;5;196m"   /* Rouge vif                */
#define ANSI_SRC_MAC    "\x1b[38;5;208m"   /* Orange clair             */
#define ANSI_TYPE       "\x1b[38;5;226m"   /* Jaune                    */
#define ANSI_DATA       "\x1b[38;5;82m"    /* Vert vif                 */
#define ANSI_FCS        "\x1b[38;5;21m"    /* Bleu vif                 */

/* ------------------------------------------------------------------ */
/*  Légende trame                                              */
/* ------------------------------------------------------------------ */
const char *TRAME_LEGENDE =
    "\n=== Legende de la trame Ethernet ===\n"
    "\x1b[38;5;240m" "██ Préambule " ANSI_RESET "(7 octets)  — Synchronisation (0xAA x 7)\n"
    "\x1b[38;5;214m" "██ SFD       " ANSI_RESET "(1 octet)   — Start Frame Delimiter (0xAB)\n"
    "\x1b[38;5;196m" "██ MAC dest  " ANSI_RESET "(6 octets)  — Adresse MAC de destination\n"
    "\x1b[38;5;208m" "██ MAC src   " ANSI_RESET "(6 octets)  — Adresse MAC source\n"
    "\x1b[38;5;226m" "██ Type      " ANSI_RESET "(2 octets)  — EtherType / protocole\n"
    "\x1b[38;5;82m"  "██ DATA      " ANSI_RESET "(46+ octets)— Charge utile (bourrage inclus)\n"
    "\x1b[38;5;21m"  "██ FCS       " ANSI_RESET "(4 octets)  — CRC-32 de contrôle d'intégrité\n"
    "=====================================\n";

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
* Retourne : Une optional d'une nouvelle trame ethernet.
*/
struct optional_tram init_trame(size_t data_size, const void *data,
                        struct mac dest, struct mac src,
                        protocol_t protocol)
{
    size_t original_data_size = data_size;

    /* Bourrage minimal Ethernet : la charge utile doit faire au moins 46 octets */
    if (data_size < BOURRAGE)
        data_size = BOURRAGE;

    struct trame tr = {0};
    tr.size = sizeof(struct trame_before_data)
            + data_size
            + sizeof(struct trame_after_data);

    // tr.trame
    tr.trame = malloc(tr.size);
    if(!tr.trame)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return (struct optional_tram){.tr = tr, .exit_code = ERROR_BAD_ALLOC};
    }

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

    return (struct optional_tram){.tr = tr, .exit_code = SUCESS};
}

/**
 * Retoune une copie profonde d'une trame existante.
 */
struct optional_tram trame_copy(const struct trame src)
{
    struct trame copy = { .trame = NULL, .size = 0 };

    if (!src.trame || src.size == 0)
        return (struct optional_tram) { .tr = copy, .exit_code = ERROR_NULL_POINTER_PARAM};

    copy.trame = malloc(src.size);
    if (!copy.trame)
        return (struct optional_tram) { .tr = copy, .exit_code = ERROR_BAD_ALLOC};

    memcpy(copy.trame, src.trame, src.size);
    copy.size = src.size;

    return (struct optional_tram) { .tr = copy, .exit_code = SUCESS};
}

/**   
*    Utilitaire interne : pointeur vers l'en-tête                      
*/
static const struct trame_before_data *get_header(struct trame tr)
{
    return (const struct trame_before_data *)tr.trame;
}

/**
 * trame_get_destination                                                     
 *  Retourne l'adresse MAC de destination
 */
struct mac trame_get_destination(struct trame tr)
{
    return get_header(tr)->destination;
}

/**
 * trame_get_source 
 * Retourne l'adresse MAC source. 
 */
struct mac trame_get_source(struct trame tr)
{
    return get_header(tr)->source;
}


/**
 * get_protocol 
 * Retourne le protocole (2 octets, big-endian). 
 */
protocol_t trame_get_protocol(struct trame tr)
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
size_t trame_get_data(struct trame tr, const void **data)
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
uint32_t trame_get_fcs(struct trame tr)
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
bool trame_is_fcs_valid(struct trame tr)
{
    /* Même zone que dans init_trame */
    const void *fcs_start = tr.trame + OFFSET_FCS_START;
    size_t fcs_len = tr.size
                     - OFFSET_FCS_START
                     - sizeof(struct trame_after_data);

    return crc32(fcs_start, fcs_len) == trame_get_fcs(tr);
}


/**
 * Donne le nom en chaine de caractére du protocol.
 */
const char* trame_get_protocol_str_name(protocol_t protocol)
{
    switch(protocol)
    {
    case 0x0800:
        return "IPv4";
    case 0x0806:
        return "ARP";
    case 0x86DD:
        return "IPv6";
    default:
        return "INVALID";
    }
}

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
enum EXIT_CODES trame_to_hex(struct trame tr, char** string)
{
    /*
     * Chaque octet est représenté par exactement 3 caractères :
     *   2 chiffres hex  +  1 espace (ou '\0' pour le dernier)
     * Taille totale : tr->size * 3 octets.
     */

    const size_t str_size = tr.size * 3 + 1;  // + 1 pour le \0 final

    *string = malloc(str_size);
    if (!*string)
    {
        *string = NULL;
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return ERROR_BAD_ALLOC;
    }

    const octet *bytes = (const octet *)tr.trame;

    for (size_t i = 0; i < tr.size; i++)
    {
        /* Écrit "XX " ou "XX\0" pour le dernier octet */
        snprintf(*string + i * 3, 4, "%02X%c",
                 bytes[i],
                 (i < tr.size - 1) ? ' ' : '\0');
    }

    return SUCESS;
}

/**
 * La légende de la trame obtenu avec trame_to_hex_ansi(struct trame tr)
 * 
 */
const char* trame_get_colored_legend()
{
    return TRAME_LEGENDE;
}

/**
 * Fonction d'aide pour écrire des bytes en couleur.
 */
static size_t write_colored_bytes(char *buf,
                                  const octet *src, size_t n,
                                  const char *color,
                                  bool last_group)
{
    size_t written = 0;

    /* Couleur d'entrée pour tout le groupe */
    written += (size_t)sprintf(buf + written, "%s", color);

    for (size_t i = 0; i < n; i++)
    {
        written += (size_t)sprintf(buf + written, "%02X", src[i]);

        /* Espace séparateur sauf après le tout dernier octet de la trame */
        if (!last_group || i < n - 1)
            buf[written++] = ' ';
    }

    /* Reset après le groupe */
    written += (size_t)sprintf(buf + written, "%s", ANSI_RESET);

    return written;
}

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
enum EXIT_CODES trame_to_hex_ansi(struct trame tr, char** string)
{
    /*
     * Estimation pessimiste de la taille du buffer :
     *   - chaque octet        : 3 caractères ("XX ")
     *   - codes ANSI par groupe : ~15 caractères × 7 groupes pour le code
     *                             + ~5 caractères × 7 groupes pour le reset
     * On alloue tr->size * 3 + 512 octets pour les séquences ANSI.
     */
    const size_t buf_size = tr.size * 3 + 512;

    char *hex = malloc(buf_size);
    if (!hex)
    {
        *string = NULL;
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return ERROR_BAD_ALLOC;
    }

    const octet *raw = (const octet *)tr.trame;
    size_t offset_raw = 0;   /* position dans les octets bruts de la trame */
    size_t offset_hex = 0;   /* position d'écriture dans la chaîne résultat */

    /* Taille des champs fixes */
    const size_t sz_pre  = TAILLE_PREAMBULE;
    const size_t sz_sfd  = 1;
    const size_t sz_mac  = sizeof(struct mac);           /* 6 octets */
    const size_t sz_type = 2;
    const size_t sz_fcs  = sizeof(struct trame_after_data); /* 4 octets */
    const size_t sz_data = tr.size
                           - sizeof(struct trame_before_data)
                           - sz_fcs;

    // Define qui permet de "colorier" facilement la chaine

#define WRITE(src, n, color, last) \
    offset_hex += write_colored_bytes(hex + offset_hex, (src), (n), (color), (last)); \
    offset_raw += (n);

    WRITE(raw + offset_raw, sz_pre,  ANSI_PREAMBULE, false)
    WRITE(raw + offset_raw, sz_sfd,  ANSI_SFD,       false)
    WRITE(raw + offset_raw, sz_mac,  ANSI_DST_MAC,   false)
    WRITE(raw + offset_raw, sz_mac,  ANSI_SRC_MAC,   false)
    WRITE(raw + offset_raw, sz_type, ANSI_TYPE,       false)
    WRITE(raw + offset_raw, sz_data, ANSI_DATA,       false)
    WRITE(raw + offset_raw, sz_fcs,  ANSI_FCS,        true)
#undef WRITE  // Undef car on n'en à plus besoin

    hex[offset_hex] = '\0';
    *string = hex;
    return SUCESS;
}


/**
 * Détruit la trame!
 */
void deinit_trame(struct trame* tr)
{
    if(!tr)
    {
        DEBUG_WARNING(WARNING_ITEM_NULL_FREEING);
        return;
    } 

    if(tr->trame != NULL)
    {
        free(tr->trame);
    }

    tr->trame = NULL;
    tr->size = 0;
}