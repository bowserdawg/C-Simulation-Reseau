#pragma once

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
    size_t _nb_devices;
};

struct optional_net_graph {
    struct net_graph graph;
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
 * Detruit le graphe du réseau.
 * 
 *      graph : Le graphe qui va etre détruit.
 * 
 */
void deinit_graph(struct net_graph* graph);