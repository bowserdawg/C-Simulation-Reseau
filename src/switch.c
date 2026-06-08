#include "address_ip.h"
#include "address_mac.h"
#include "switch.h"

#define SWITCH_COMMUTATION_TABLE_SIZE 16384

/**
 * Initialise un nouveau switch. 
 */
struct optional_device_switch init_switch(struct mac mac, port_t nb_port, priority_t priority)
{
    enum EXIT_CODES ex_code = SUCESS;

    struct optional_table_commutation table = init_table_commutation(SWITCH_COMMUTATION_TABLE_SIZE);
    if(table.exit_code != SUCESS)
    {
        DEBUG_WARNING(table.exit_code);
        ex_code = table.exit_code;
    }

    return (struct optional_device_switch)
                {
                    .dev_switch = (struct device_switch)
                    {
                        .table_commutation = table.table,
                        ._mac     = mac, 
                        ._nbPort  = nb_port,
                        .nb_port_utilise = 0,
                        .priority = priority
                    }, 

                    .exit_code = ex_code
                };
}

// Les switchs sont passés par adresse car ils sont assez gros (32 octets)


/**
 * Ajoute une nouvelle entré à la table de commutation.
 * 
 *      swtch : Le switch ou l'entré est ajouté.     
 * 
 *      port : Le port qui mene à la machine.
 * 
 *      mac  : L'adresse mac de la machine.
 * 
 * Retourne : Un code d'erreur indiquant le status de sortie.
 */
enum EXIT_CODES switch_add_table(struct device_switch* swtch, port_t port, struct mac mac)
{
    if(port > swtch->_nbPort)
    {
        DEBUG_WARNING(ERROR_INVALID_PORT_PARAM);
        return ERROR_INVALID_PORT_PARAM;
    }

    return table_commutation_add(swtch->table_commutation, port, mac);
}

/**
 * Trouve une machine dans la tabke de commuttation si elle y est.
 * 
 *      swtch : Le switch ou l'entré est ajouté.     
 * 
 *      mac  : L'adresse mac de la machine.
 * 
 * Retourne : Le port qui méne jusqu'à la machine, dans un optional, 
 *            "INVALID_PORT" dans l'optional si le port est introuvable, et
 *            un code d'erreur indiquant si quelque chose de mal c'est passé.
 */
struct optional_port switch_look_table(struct device_switch* swtch, struct mac mac)
{
    struct optional_port port = table_commutation_lookup(swtch->table_commutation, mac);

    if(port.exit_code != SUCESS)
    {
        DEBUG_WARNING(port.exit_code);
        return (struct optional_port){INVALID_PORT, port.exit_code};
    }

    return (struct optional_port){port.port, SUCESS};
}

/*
* Attention : ne met pas l'adresse mac à 0, mais la priorité oui.
*/
void deinit_switch(struct device_switch* swtch)
{
    deinit_table_commutation(&swtch->table_commutation);
    swtch->table_commutation.data = NULL;
    swtch->priority = 0;
}

/*
* Augmente le nombre de port utilisé par un switch.
*/