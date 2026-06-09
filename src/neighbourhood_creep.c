#include "errors.h"
#include "trame.h"
#include "global.h"

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
    if(!creep.indexes)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return (struct creep_optional){.creep = {0}, ERROR_BAD_ALLOC};
    }
    creep.cap = init_cap;
    creep.len = 0;

    return (struct creep_optional){.creep = creep, SUCESS};
}

static size_t creep_pop(struct creep* creep)
{
    if(creep->len > 0)
    {
        --(creep->len);
        return creep->indexes[creep->len];
    }
    return INVALID_INDEX;
}

static enum EXIT_CODES creep_push(struct creep* creep, size_t value)
{
    if(creep->len >= creep->cap)
    {
        creep->cap *= 2;
        size_t* new_indexes = malloc(sizeof(size_t) * creep->cap); // Malloc
        if(!new_indexes)
        {
            DEBUG_WARNING(ERROR_BAD_ALLOC);
            return ERROR_BAD_ALLOC;
        }
        memcpy(new_indexes, creep->indexes, sizeof(size_t) * creep->len);
        free(creep->indexes);  // Free
        creep->indexes = new_indexes;
    }

    creep->indexes[creep->len] = value;
    creep->len++;

    return SUCESS;
}

struct house {
    struct trame* trames;
    bool is_present;
};

/**
 *  Contient des cases vides qui peuvent etre peuple.
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
    /* Gestion des erreurs */
    struct neighbourhood* neighbourhood = malloc(sizeof(struct neighbourhood));
    // Calloc car bool
    neighbourhood->houses               = calloc(capacity, sizeof(struct house));
    if(!neighbourhood || !neighbourhood->houses)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return (struct neighbourhood_ptr_optional) { .n_ptr = NULL, .exit_code = ERROR_BAD_ALLOC };
    }

    struct creep_optional op_crep = creep_init(capacity);
    if(op_crep.ex_code != SUCESS)
    {
        free(neighbourhood->houses);
        free(neighbourhood);
        DEBUG_WARNING(op_crep.ex_code);
        return (struct neighbourhood_ptr_optional) { .n_ptr = NULL, .exit_code = op_crep.ex_code };
    }
    /**********************/

    // Neighbourhood tailles
    neighbourhood->len = 0;
    neighbourhood->cap = capacity;

    // Creep tailles
    neighbourhood->creep = op_crep.creep;

    return (struct neighbourhood_ptr_optional) { .n_ptr = neighbourhood, .exit_code = SUCESS };
}

/**
 * Ajouter une trame à la structure.
 * 
 *      - neighbourhood : La structure.
 * 
 *      - trame : La trame a rajouté.
 * 
 * Retourne : Un code de sortie indiquant si tous s'est bien passé.
 */
static enum EXIT_CODES push_trame(struct neighbourhood* neighbourhood, struct trame* trame)
{
    if(!neighbourhood || !neighbourhood->houses || !neighbourhood->creep.indexes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return ERROR_NULL_POINTER_PARAM;
    }

    // Resize si capacité trop petite.
    if(neighbourhood->len >= neighbourhood->cap)
    {
        neighbourhood->cap *= 2;
        // Reserve
        struct house* houses = malloc(sizeof(struct house) * neighbourhood->cap);  // Malloc
        if(!houses)
        {
            DEBUG_WARNING(ERROR_BAD_ALLOC);
            return ERROR_BAD_ALLOC;
        }
        // Copier
        memcpy(houses, neighbourhood->houses, neighbourhood->len * sizeof(struct house));   

        free(neighbourhood->houses);                // Free

        // Afféctation
        neighbourhood->houses = houses;
    }

    // Ajouté nouvelle maison
    neighbourhood->houses[neighbourhood->len] = (struct house){trame, true};
    neighbourhood->len++;

    return SUCESS;
}

/**
 * Ajoute une trame dans le réseau.
 * 
 * Ne pas ajouter deux fois le meme trame dans la structure!!
 * 
 *      - neighbourhood : Structure qui contient les trames.
 * 
 *      - trame         : La trame à rajouter.
 * 
 * Retourne : Un code de sortie indiquant si l'opération s'est bien abouti.
 */
enum EXIT_CODES neighbourhood_push_trame(struct neighbourhood* neighbourhood, struct trame* trame)
{
    /* Gestion des erreurs */
    if(!neighbourhood || !neighbourhood->houses || !neighbourhood->creep.indexes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return ERROR_NULL_POINTER_PARAM;
    }
    /**********************/

    if(neighbourhood->creep.len == 0)
    {
        enum EXIT_CODES ex_code = push_trame(neighbourhood, trame);
        if(ex_code != SUCESS)
        {
            DEBUG_WARNING(ex_code);
            return ex_code;
        }
    }
    else
    {
        neighbourhood->houses[creep_pop(&neighbourhood->creep)] = 
                    (struct house){.trames = trame, .is_present = true};
    }

    return SUCESS;
}

/**
 * Enleve une trame du réseau.
 * 
 *      - neighbourhood : Structure qui contient les trames.
 * 
 *      - trame         : L'index du a enlever.
 * 
 * Retourne : Un code de sortie indiquant si l'opération s'est bien abouti.
 */
enum EXIT_CODES neighbourhood_pop_trame(struct neighbourhood* neighbourhood, size_t index)
{
    /* Gestion des erreurs */
    if(!neighbourhood || !neighbourhood->houses || !neighbourhood->creep.indexes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return ERROR_NULL_POINTER_PARAM;
    }
    if(neighbourhood->len <= index)
    {
        DEBUG_WARNING(ERROR_INDEX_OUT_OF_RANGE);
        return ERROR_INDEX_OUT_OF_RANGE;
    }
    /**********************/


    if(neighbourhood->houses[index].is_present)
    {
        neighbourhood->houses[index].is_present = false;
        // L'index est libre et prioritisé.
        enum EXIT_CODES ex = creep_push(&neighbourhood->creep, index);

        if(ex != SUCESS) return ex;
        deinit_trame(neighbourhood->houses[index].trames);
        // neighbourhood->houses[index].trames devient du garbage data
    }

    return SUCESS;
}

/**
 * Retourne la taille de la structure.
 */
size_t neighbourhood_get_len(struct neighbourhood* neighbourhood)
{
    if(!neighbourhood) return INVALID_INDEX;
    return neighbourhood->len;
}

/**
 * Retourne une trame spécifique.
 * 
 * Renvoie un WARNING_ITEM_NOT_FOUND si il n'existe pas, utiliser neighbourhood_is_present pour etre sur.
 */
struct optional_tram neighbourhood_get_trame(struct neighbourhood* neighbourhood, size_t index)
{
    /* Gestion des erreurs */
    if(!neighbourhood || !neighbourhood->houses || !neighbourhood->creep.indexes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return  (struct optional_tram) 
            {.tr = {0}, ERROR_NULL_POINTER_PARAM};
    }
    if(index >= neighbourhood->len)
    {
        return (struct optional_tram)
            { .tr = {0}, .exit_code = ERROR_INDEX_OUT_OF_RANGE};
    }
    /*********************/

    /* Warning simple si */
    if(!neighbourhood->houses[index].is_present)
    {
        return (struct optional_tram)
            { .tr = {0}, .exit_code = WARNING_ITEM_NOT_FOUND};
    }

    return (struct optional_tram)
    { 
        .tr = *neighbourhood->houses[index].trames, 
        .exit_code = WARNING_ITEM_NOT_FOUND
    };
}

/**
 * Indique si quelqu'un est présent sur l'index ou non.
 */
bool neighbourhood_is_present(struct neighbourhood* neighbourhood, size_t index)
{
     /* Gestion des "erreurs" */
    if(!neighbourhood || !neighbourhood->houses || !neighbourhood->creep.indexes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return false;
    }
    if(index >= neighbourhood->len)
    {
        return false;
    }
    /*********************/

    return neighbourhood->houses[index].is_present;
}

/**
 * Détruit le quartier.
 */
void deinit_neighbourhood(struct neighbourhood* neighbourhood)
{
    if(neighbourhood == NULL)
    {
        DEBUG_WARNING(WARNING_ITEM_NULL_FREEING);
        return;
    }
    for(size_t i = 0; i < neighbourhood->len; ++i)
    {
        if(neighbourhood->houses[i].is_present)
        {
            deinit_trame(neighbourhood->houses[i].trames);
        }
    }

    if(!neighbourhood->creep.indexes) DEBUG_WARNING(WARNING_ITEM_NULL_FREEING);
    free(neighbourhood->creep.indexes);
    neighbourhood->creep.indexes = NULL;

    if(!neighbourhood->houses) DEBUG_WARNING(WARNING_ITEM_NULL_FREEING);
    free(neighbourhood->houses);
    neighbourhood->houses = NULL;

    free(neighbourhood);
}




