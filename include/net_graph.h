#pragma once

#include "errors.h"

#include <stdint.h>
#include "station.h"
#include "switch.h"

// Le cout d'un lien.
typedef uint32_t cost_t;

#define INVALID_INDEX UINT64_MAX 

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
 * Detruit le graphe du réseau.
 * 
 *      graph : Le graphe qui va etre détruit.
 * 
 */
void deinit_graph(struct net_graph* graph);