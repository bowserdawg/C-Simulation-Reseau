#include "errors.h"
#include "trame.h"

#include <stddef.h>

typedef size_t tick_t;

struct neighbourhood;

struct neighbourhood_ptr_optional {
    struct neighbourhood* n_ptr;
    enum EXIT_CODES exit_code;
};

/**
 * Creation de la structure, doit etre free avec deinit_neighbourhood
 */
struct neighbourhood_ptr_optional init_neighbourhood(size_t capacity);

struct optional_tick {
    tick_t time;
    enum EXIT_CODES exit_code;
};

#define INVALID_TIME UINT64_MAX

/**
 * Ajoute une trame dans le réseau.
 *
 * Ne pas ajouter deux fois le même trame dans la structure !!
 *
 *      - neighbourhood : Structure qui contient les trames.
 *      - trame         : La trame à rajouter.
 *
 * Retourne : Un code de sortie indiquant si l'opération s'est bien aboutie.
 */
enum EXIT_CODES neighbourhood_push_trame(struct neighbourhood* neighbourhood, 
    struct trame *trame, size_t remaining_time, size_t dest_idx, size_t src_idx);

/**
 * Enleve une trame du réseau.
 * 
 *      - neighbourhood : Structure qui contient les trames.
 * 
 *      - trame         : L'index du a enlever.
 * 
 * Retourne : Un code de sortie indiquant si l'opération s'est bien abouti.
 */
enum EXIT_CODES neighbourhood_pop_trame(struct neighbourhood* neighbourhood, size_t index);


/**
 * Retourne la taille de la structure.
 */
size_t neighbourhood_get_len(struct neighbourhood* neighbourhood);


/**
 * Retourne une trame spécifique.
 * 
 * Renvoie un WARNING_ITEM_NOT_FOUND si il n'existe pas, utiliser neighbourhood_is_present pour etre sur.
 */
struct optional_tram neighbourhood_get_trame(struct neighbourhood* neighbourhood, size_t index);


/**
 * Indique si quelqu'un est présent sur l'index ou non.
 */
bool neighbourhood_is_present(struct neighbourhood* neighbourhood, size_t index);

/**
 * Le temps qu'il reste pour une case donné.
 * 
 * Renvoie INVALID_TIME en cas d'erreur.
 */
size_t neighbourhood_get_remaining_time(struct neighbourhood* neighbourhood, size_t index);

/**
 * La source du message dans le noeud.
 * 
 * Renvoie INVALID_INDEX en cas d'erreur.
 */
size_t neighbourhood_get_src(struct neighbourhood* neighbourhood, size_t index);

/**
 * La destination du message dans le noeud.
 * 
 * Renvoie INVALID_INDEX en cas d'erreur.
 */
size_t neighbourhood_get_dest(struct neighbourhood* neighbourhood, size_t index);


/**
 * Fait baisser le temps qu'il reste pour un paquet pour arriver à destination.
 */
struct optional_tick neighbourhood_decrease_time(struct neighbourhood* neighbourhood, size_t index);


/**
 * Détruit le quartier.
 */
void deinit_neighbourhood(struct neighbourhood* neighbourhood);