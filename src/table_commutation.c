#include "switch.h"
#include "address_mac.h"
#include "errors.h"
#include "table_commutation.h"


#include <stdlib.h>

/**
 * Creation d'une nouvelle table de commutation avec taille dynamique pour un switch.
 * 
 *      - initial_size : Capacite initiale du table de commutation.
 * 
 * retourne : un "struct optional_table_commutation" qui peut contenir un code d'erreur en cas d'erreur 
 *  pendant la création.
 */
struct optional_table_commutation init_table_commutation(size_t initial_size)
{
    struct table_commutation new_table;

    new_table.data = malloc(sizeof(struct table_entry) * initial_size);
    if(new_table.data == NULL)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return (struct optional_table_commutation){.table = new_table, .exit_code = ERROR_BAD_ALLOC};
    }
        
    new_table.size = initial_size;

    return (struct optional_table_commutation){ .table = new_table, .exit_code = SUCESS};
}

/**
 * Ajoute un élement à la table de commutation.
 */
enum EXIT_CODES table_commutation_add(struct table_commutation table, port_t port, struct mac address)
{
    if(!(table.data))
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return ERROR_NULL_POINTER_PARAM;
    }

    uint64_t num = mac_to_long(address);
    // Ajoute une nouvelle entrée à la table de commutation, peut écraser les anciennes et c'est ok.
    table.data[num % table.size] = (struct table_entry){.nbPort = port,
                                                          .addrs  = address};

    return SUCESS;
}

/**
 * Retourne "INVALID_PORT" si l'addresse ne se trouve pas dans la table.
 */
struct optional_port table_commutation_lookup(struct table_commutation table, struct mac mac)
{
    if(!(table.data))
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return (struct optional_port){INVALID_PORT, ERROR_NULL_POINTER_PARAM};
    }

    uint64_t num = mac_to_long(mac);
    // Peut etre uninitalisé mais ce n'est pas grave.
    struct mac mac2 = table.data[num % table.size].addrs;

    if( mac_equals(mac, mac2) )
    {
        return (struct optional_port){table.data[num % table.size].nbPort, SUCESS};
    }

    return (struct optional_port){INVALID_PORT, SUCESS};
}


/**
 * Destruction d'une table de commutation.
 * 
 *      - table : Capaicte initiale du table de commutation.
 */
void deinit_table_commutation(struct table_commutation* table)
{
    // passe par addresse car struct table_commutation > 16 octet, et cela nous donne le doit de 
    // mettre les valeurs à null.

    // Juste pour s'assurer au debuggage si on ne fait pas de free inutile.
    if(!table->data)
        DEBUG_WARNING(WARNING_ITEM_NULL_FREEING);

    free(table->data);
    table->data = NULL;
    table->size = 0;
}
