#include "errors.h"
#include "trame.h"

#include <stddef.h>

struct neighbourhood;

struct neighbourhood_ptr_optional {
    struct neighbourhood* n_ptr;
    enum EXIT_CODES exit_code;
};

/**
 * Creation de la structure, doit etre free avec deinit_neighbourhood
 */
struct neighbourhood_ptr_optional init_neighbourhood(size_t capacity);


/**
 * Ajoute une trame dans le réseau.
 * 
 *      - neighbourhood : Structure qui contient les trames.
 * 
 *      - trame         : La trame à rajouter.
 * 
 * Retourne : Un code de sortie indiquant si l'opération s'est bien abouti.
 */
enum EXIT_CODES neighbourhood_push_trame(struct neighbourhood* neighbourhood, struct trame* trame);



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
 * Détruit le quartier.
 */
void deinit_neighbourhood(struct neighbourhood* neighbourhood);