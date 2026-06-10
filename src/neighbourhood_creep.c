#include "errors.h"
#include "trame.h"
#include "global.h"
#include "net_graph.h" // pour cost_t

#include <stddef.h>
#include <string.h> // Pour memcpy
#include <stdio.h>

/**
 * Regarde si il y'as des "maisons" libres dans le quartier...
 *
 * Index les indexes libres.
 */
struct creep {
    size_t* indexes;
    size_t len;
    size_t cap;
};

struct creep_optional {
    struct creep creep;
    enum EXIT_CODES ex_code;  
};

static struct creep_optional creep_init(int init_cap)
{
    struct creep creep;
    creep.indexes = malloc(sizeof(size_t) * init_cap);
    if (!creep.indexes)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return (struct creep_optional){ .creep = {0}, .ex_code = ERROR_BAD_ALLOC };
    }
    creep.cap = init_cap;
    creep.len = 0;

    return (struct creep_optional){ .creep = creep, .ex_code = SUCESS };
}

static size_t creep_pop(struct creep* creep)
{
    if (creep->len > 0)
    {
        --(creep->len);
        return creep->indexes[creep->len];
    }
    return INVALID_INDEX;
}

static enum EXIT_CODES creep_push(struct creep* creep, size_t value)
{
    if (creep->len >= creep->cap)
    {
        size_t new_cap = creep->cap * 2;
        size_t* new_indexes = malloc(sizeof(size_t) * new_cap);
        if (!new_indexes)
        {
            DEBUG_WARNING(ERROR_BAD_ALLOC);
            return ERROR_BAD_ALLOC;
        }
        memcpy(new_indexes, creep->indexes, sizeof(size_t) * creep->len);
        free(creep->indexes);
        creep->indexes = new_indexes;
        creep->cap = new_cap;   // mise à jour après le free, sans risque
    }

    creep->indexes[creep->len] = value;
    creep->len++;

    return SUCESS;
}

struct house {
    struct trame trames;
    bool is_present;
};

/**
 * Contient des cases vides qui peuvent etre peuplées.
 */
struct neighbourhood {
    struct creep creep;
    struct house* houses;
    size_t len;
    size_t cap;
};

struct neighbourhood_ptr_optional {
    struct neighbourhood* n_ptr;
    enum EXIT_CODES exit_code;
};

/**
 * Creation de la structure, doit etre free avec deinit_neighbourhood
 */
struct neighbourhood_ptr_optional init_neighbourhood(size_t capacity)
{
    struct neighbourhood* neighbourhood = malloc(sizeof(struct neighbourhood));
    if (!neighbourhood)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return (struct neighbourhood_ptr_optional){ .n_ptr = NULL, .exit_code = ERROR_BAD_ALLOC };
    }

    // calloc : initialise is_present à false pour toutes les maisons
    neighbourhood->houses = calloc(capacity, sizeof(struct house));
    if (!neighbourhood->houses)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        free(neighbourhood);
        return (struct neighbourhood_ptr_optional){ .n_ptr = NULL, .exit_code = ERROR_BAD_ALLOC };
    }

    struct creep_optional op_crep = creep_init(capacity);
    if (op_crep.ex_code != SUCESS)
    {
        free(neighbourhood->houses);
        free(neighbourhood);
        DEBUG_WARNING(op_crep.ex_code);
        return (struct neighbourhood_ptr_optional){ .n_ptr = NULL, .exit_code = op_crep.ex_code };
    }

    neighbourhood->len = 0;
    neighbourhood->cap = capacity;
    neighbourhood->creep = op_crep.creep;

    return (struct neighbourhood_ptr_optional){ .n_ptr = neighbourhood, .exit_code = SUCESS };
}

/**
 * Fonction d'aide qui ajoute une trame à la fin du tableau houses.
 * Redimensionne houses ET creep si nécessaire.
 */
static enum EXIT_CODES push_trame(struct neighbourhood* neighbourhood, struct trame* trame)
{
    if (!neighbourhood || !neighbourhood->houses || !neighbourhood->creep.indexes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return ERROR_NULL_POINTER_PARAM;
    }

    if (neighbourhood->len >= neighbourhood->cap)
    {
        size_t new_cap = neighbourhood->cap * 2;

        struct house* houses = calloc(new_cap, sizeof(struct house));
        if (!houses)
        {
            DEBUG_WARNING(ERROR_BAD_ALLOC);
            return ERROR_BAD_ALLOC;
        }
        memcpy(houses, neighbourhood->houses, neighbourhood->len * sizeof(struct house));
        free(neighbourhood->houses);
        neighbourhood->houses = houses;

        size_t* new_indexes = malloc(sizeof(size_t) * new_cap);
        if (!new_indexes)
        {
            DEBUG_WARNING(ERROR_BAD_ALLOC);
            // houses a déjà été remplacé, on ne peut pas annuler proprement ici
            neighbourhood->cap = new_cap; // cohérence minimale
            return ERROR_BAD_ALLOC;
        }
        memcpy(new_indexes, neighbourhood->creep.indexes,
               neighbourhood->creep.len * sizeof(size_t));
        free(neighbourhood->creep.indexes);
        neighbourhood->creep.indexes = new_indexes;
        neighbourhood->creep.cap = new_cap;

        neighbourhood->cap = new_cap;
    }

    neighbourhood->houses[neighbourhood->len] = (struct house){ *trame, true };
    neighbourhood->len++;

    return SUCESS;
}

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
enum EXIT_CODES neighbourhood_push_trame(struct neighbourhood* neighbourhood, struct trame *trame)
{
    if (!neighbourhood || !neighbourhood->houses || !neighbourhood->creep.indexes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return ERROR_NULL_POINTER_PARAM;
    }

    if (neighbourhood->creep.len == 0)
    {
        // Pas de slot libre : on agrandit le tableau
        enum EXIT_CODES ex_code = push_trame(neighbourhood, trame);
        if (ex_code != SUCESS)
        {
            DEBUG_WARNING(ex_code);
            return ex_code;
        }
    }
    else
    {
        // Réutilisation d'un slot libre
        size_t free_idx = creep_pop(&neighbourhood->creep);
        neighbourhood->houses[free_idx] = (struct house){ .trames = *trame, .is_present = true };
    }


    return SUCESS;
}

/**
 * Enlève une trame du réseau.
 *
 *      - neighbourhood : Structure qui contient les trames.
 *      - index         : L'index de la trame à enlever.
 *
 * Retourne : Un code de sortie indiquant si l'opération s'est bien aboutie.
 */
enum EXIT_CODES neighbourhood_pop_trame(struct neighbourhood* neighbourhood, size_t index)
{
    if (!neighbourhood || !neighbourhood->houses || !neighbourhood->creep.indexes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return ERROR_NULL_POINTER_PARAM;
    }
    if (index >= neighbourhood->len)
    {
        DEBUG_WARNING(ERROR_INDEX_OUT_OF_RANGE);
        return ERROR_INDEX_OUT_OF_RANGE;
    }

    if (neighbourhood->houses[index].is_present)
    {
        neighbourhood->houses[index].is_present = false;
        deinit_trame(&neighbourhood->houses[index].trames);

        neighbourhood->houses[index].trames.trame = NULL;

        enum EXIT_CODES ex = creep_push(&neighbourhood->creep, index);
        if (ex != SUCESS) return ex;
    }

    return SUCESS;
}

/**
 * Retourne la taille du tableau interne (positions allouées, actives ou non).
 * Pour le nombre d'éléments actifs, itérer avec neighbourhood_is_present.
 */
size_t neighbourhood_get_len(struct neighbourhood* neighbourhood)
{
    if (!neighbourhood) return INVALID_INDEX;
    return neighbourhood->len;
}

/**
 * Retourne un pointeur vers la trame à l'index donné.
  */
struct optional_tram neighbourhood_get_trame(struct neighbourhood* neighbourhood, size_t index)
{
    if (!neighbourhood || !neighbourhood->houses || !neighbourhood->creep.indexes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return (struct optional_tram){.tr = {.trame = NULL,.size = 0}, .exit_code = ERROR_NULL_POINTER_PARAM};
    }
    if (index >= neighbourhood->len)
    {
        DEBUG_WARNING(ERROR_INDEX_OUT_OF_RANGE);
        return (struct optional_tram){.tr = {.trame = NULL,.size = 0}, .exit_code = ERROR_INDEX_OUT_OF_RANGE};
    }
    if (!neighbourhood->houses[index].is_present)
    {
        DEBUG_WARNING(WARNING_ITEM_NOT_FOUND);
        return (struct optional_tram){.tr = {.trame = NULL,.size = 0}, .exit_code = ERROR_INDEX_OUT_OF_RANGE};
    }

    return (struct optional_tram){.tr = neighbourhood->houses[index].trames, .exit_code = ERROR_INDEX_OUT_OF_RANGE};
}

/**
 * Indique si quelqu'un est présent sur l'index ou non.
 */
bool neighbourhood_is_present(struct neighbourhood* neighbourhood, size_t index)
{
    if (!neighbourhood || !neighbourhood->houses || !neighbourhood->creep.indexes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return false;
    }
    if (index >= neighbourhood->len)
    {
        return false;
    }

    return neighbourhood->houses[index].is_present;
}

/**
 * Détruit le quartier.
 */
void deinit_neighbourhood(struct neighbourhood* neighbourhood)
{
    if (neighbourhood == NULL)
    {
        DEBUG_WARNING(WARNING_ITEM_NULL_FREEING);
        return;
    }

    for (size_t i = 0; i < neighbourhood->len; ++i)
    {
        if (neighbourhood->houses[i].is_present)
        {
            //trame_get_data(*neighbourhood->houses[i].trames->size,&data);
            deinit_trame(&neighbourhood->houses[i].trames);
            neighbourhood->houses[i].trames.trame = NULL;
            neighbourhood->houses[i].is_present = false;
        }
    }

    if (!neighbourhood->creep.indexes) DEBUG_WARNING(WARNING_ITEM_NULL_FREEING);
    free(neighbourhood->creep.indexes);
    neighbourhood->creep.indexes = NULL;

    if (!neighbourhood->houses) DEBUG_WARNING(WARNING_ITEM_NULL_FREEING);
    free(neighbourhood->houses);
    neighbourhood->houses = NULL;

    free(neighbourhood);
}