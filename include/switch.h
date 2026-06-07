#pragma once

#include "address_ip.h"
#include "address_mac.h"

#include <stdlib.h>

#include "table_commutation.h"

struct device_switch {
    struct table_commutation table_commutation;
    struct mac _mac;
    port_t _nbPort;
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
struct optional_device_switch init_switch(struct mac mac, port_t nbPort);


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

/*
* Attention : ne met pas l'adresse mac à 0.
*/
void deinit_switch(struct device_switch* swtch);