#pragma once

#include "address_ip.h"
#include "address_mac.h"
#include "table_commutation.h" // port_t et priority_t 

#include <stdlib.h>

enum SWITCH_PORT_STATUS {
    ROOT,
    DESIGNATED,
    BLOCKED,
    INVALID_SWITCH
};

struct device_switch {
    struct table_commutation table_commutation;
    struct mac _mac;
    port_t _nbPort;
    size_t nb_port_utilise; 
    enum SWITCH_PORT_STATUS* port_status;
    priority_t priority;
};  

struct optional_device_switch {
    struct device_switch dev_switch;
    enum EXIT_CODES exit_code;
};

struct optional_port {
    port_t port;
    enum EXIT_CODES exit_code;
};

/**
 * Initialise un nouveau switch. 
 */
struct optional_device_switch init_switch(struct mac mac, port_t nb_port, priority_t priority);


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
enum EXIT_CODES switch_add_table(struct device_switch* swtch, port_t port, struct mac mac);


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
struct optional_port switch_look_table(struct device_switch* swtch, struct mac mac);


/**
 * Donne le status d'un port.
 * 
 * Renvoie INVALID_SWITCH si le port n'existe pas ou le switch est invalid
 */
enum SWITCH_PORT_STATUS switch_get_port_status(struct device_switch* swtch, size_t index);


/**
 * Change le status d'un port.
 * 
 * Renvoie false si le status n'existe pas ou le switch est invalid.
 */
bool switch_change_port_status(struct device_switch* swtch, size_t index, enum SWITCH_PORT_STATUS status);


/*
* Attention : ne met pas l'adresse mac à 0.
*/
void deinit_switch(struct device_switch* swtch);