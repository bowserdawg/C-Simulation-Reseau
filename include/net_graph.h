#pragma once

#include "errors.h"
#include "global.h"

#include <stdint.h>
#include "station.h"
#include "switch.h"

// Le cout d'un lien.
typedef uint32_t cost_t;

// Pratique, permet de simuler du polymorphisme en C.
union device {
    struct device_station d_station;
    struct device_switch d_switch;
};

// Le graphe n'as pas besoin d'etre dynamique car on sait deja que le nombre de 
// machine est déterminé dans le fichier de config.
struct net_graph {
    struct net_node* nodes;
    size_t nb_devices;
    size_t cap_devices;
};

struct optional_net_graph {
    struct net_graph graph;
    enum EXIT_CODES exit_code;
}; 

struct optional_size {
    size_t size;
    enum EXIT_CODES exit_code;
};

struct optional_bool {
    bool boolean;
    enum EXIT_CODES exit_code;
};

struct optional_device {
    union device* device;
    enum DEVICE_TYPE type;
    enum EXIT_CODES exit_code;
};

/**
 * Crée une nouvelle graphe de réseau.
 * 
 *       nb_devices : Nombre de machine dans le réseau.
 * 
 * Retourne : Un optional qui peut contenir un graphe ou un méssage d'erreur.
 */
struct optional_net_graph init_net_graph(size_t nb_devices);


/**
 * Ajoute une station au réseau.
 * Ne cheque pas si deux stations ont les memes adresses mac ou ip.
 * 
 *      graph : Le graphe du réseau.
 * 
 *      station : La station à rajouter.
 * 
 * Retourne : un optional de l'indice ou l'élement à été rajouté.
 */
struct optional_size net_graph_add_station(struct net_graph* graph, struct device_station station);

/**
 * Ajoute un switch au réseau.
 * Ne cheque pas si deux switchs ont la meme adresse mac.
 * 
 *      graph : Le graphe du réseau.
 * 
 *      swtch : Le switch à rajouter.
 * 
 * Retourne : un optional de l'indice ou l'élement à été rajouté.
 */
struct optional_size net_graph_add_switch(struct net_graph* graph, struct device_switch swtch);

/**
 * Ajoute une arête (lien) au graphe du réseau.
 * 
 *      - graph  : Le graphe ou il faut ajouter le lien.
 * 
 *      - index1 : L'index d'un des sommets.
 *      
 *      - index2 : L'index d'un autre sommet distinct à index1.
 * 
 *      - cost   : Le cout (le poid) d'un lien.*
 * 
 * Retourne : Un code d'erreur.
 */
enum EXIT_CODES net_graph_add_node(const struct net_graph* graph, size_t index1, size_t index2, cost_t cost);

/**
 * Cherche si deux machines sont connectés.
 */
struct optional_bool net_graph_devices_are_connected(const struct net_graph* graph, size_t index1, size_t index2);

/**
 * Renvoie la machine demandé en tant que union.
 */
struct optional_device net_graph_get_machine(const struct net_graph* graph, size_t index);

/**
 * Rare fonction qui renvoit un pointeur nulle. 
 * 
 * Le pointeur donne doit etre free aprés
 */
size_t net_graph_surrounding_indexes(struct net_graph* graphe, size_t** indexes, size_t index);


/**
 * Detruit le graphe du réseau.
 * 
 *      graph : Le graphe qui va etre détruit.
 * 
 */
void deinit_graph(struct net_graph* graph);