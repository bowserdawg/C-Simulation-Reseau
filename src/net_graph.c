#include "net_graph.h"
#include "station.h"
#include "switch.h"
#include "errors.h"

#include <stdio.h>

enum device_type{
    NONE,
    STATION,
    SWITCH,
};


// Le sommet d'un graphe de réseau est juste une machine.
struct net_node {
    union device       device;
    struct connections** d_connections;
    size_t             nb_connected_nodes;
    size_t             cap_connected_nodes;
    enum device_type   device_type;
};

struct connections {
    cost_t          costs;
    struct net_node connected_nodes;
};

#define INITIAL_NUM_CONNEXIONS 4

/**
 * Crée une nouvelle graphe de réseau.
 * 
 *       nb_devices : Nombre de machine dans le réseau.
 * 
 * Retourne : Un optional qui peut contenir un graphe ou un méssage d'erreur.
 */
struct optional_net_graph init_net_graph(size_t nb_devices)
{
    struct net_graph g = {0};

    // Calloc permet que tous les pointeurs soit null au debut.
    g.nodes = calloc(nb_devices, sizeof(struct net_node));
    if(g.nodes == NULL)
    {
        return (struct optional_net_graph){ .graph = {0}, .exit_code = ERROR_BAD_ALLOC };
    }
    g.nb_devices = 0;
    g.cap_devices = nb_devices;

    return (struct optional_net_graph){ .graph = g, .exit_code = SUCESS };
}

/************************************************************************
 * Fonctions d'aides pour net_graph_add_station et net_graph_add_switch *
 ************************************************************************/
static enum EXIT_CODES check_size(struct net_graph* graph)
{
    if(graph->nb_devices+1 >= graph->cap_devices)
    {
        DEBUG_WARNING(ERROR_CAPACITY_REACHED);
        return ERROR_CAPACITY_REACHED;
    }
    return SUCESS;
}

/**
 * Ce fonction prend un pointeur de pointeur de pointeur, assez brutale.
 */
static enum EXIT_CODES get_connections_ptr_array(struct connections*** connecs, size_t size)
{
    *connecs = calloc(size, sizeof(struct connections*));
    if(*connecs == NULL)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return ERROR_BAD_ALLOC;
    }
    return SUCESS;
}
/************************************************************************
 *                                                                      *
 ************************************************************************/

/**
 * Ajoute une station au réseau.
 * Ne cheque pas si deux stations ont les memes adresses mac ou ip.
 * 
 *      graph : Le graphe du réseau.
 * 
 *      station : La station à rajouter.
 * 
 * Retourne : un optional de l'indice ou l'élement a été rajouté.
 */
struct optional_size net_graph_add_station(struct net_graph* graph, struct device_station station)
{
    // Check si il reste de la capaicté dans le graphe.
    enum EXIT_CODES exit_code = check_size(graph);
    if(exit_code != SUCESS) goto end;

    // Alloc l'emplacement pour tous les pointeurs nécessaires
    struct connections** connecs = NULL;
    exit_code = get_connections_ptr_array(&connecs, INITIAL_NUM_CONNEXIONS);
    if(exit_code != SUCESS) goto end;
    
    struct net_node node = 
    {
        .device.d_station    = station,
        .d_connections       = connecs,
        .nb_connected_nodes  = 0,
        .cap_connected_nodes = INITIAL_NUM_CONNEXIONS,
        .device_type         = STATION
    };

    // augmente la taille du graph et ajoute le sommet
    ++(graph->nb_devices);
    graph->nodes[graph->nb_devices-1] = node;

    end:
    return (struct optional_size) {(graph->nb_devices)-1, exit_code};
}

/**
 * Ajoute un switch au réseau.
 * Ne cheque pas si deux switchs ont la meme adresse mac.
 * 
 *      graph : Le graphe du réseau.
 * 
 *      swtch : Le switch à rajouter.
 * 
 * Retourne : un optional de l'indice ou l'élement a été rajouté.
 */
struct optional_size net_graph_add_switch(struct net_graph* graph, struct device_switch swtch)
{
    // Check si il reste de la capaicté dans le graphe.
    enum EXIT_CODES exit_code = check_size(graph);
    if(exit_code != SUCESS) goto end;

    // Alloc l'emplacement pour tous les pointeurs nécessaires
    struct connections** connecs = NULL;
    exit_code = get_connections_ptr_array(&connecs, INITIAL_NUM_CONNEXIONS);
    if(exit_code != SUCESS) goto end;

    struct net_node node = 
    {
        .device.d_switch     = swtch,
        .d_connections       = connecs,
        .nb_connected_nodes  = 0,
        .cap_connected_nodes = INITIAL_NUM_CONNEXIONS,
        .device_type         = SWITCH
    };

    // augmente la taille du graph et ajoute le sommet
    ++(graph->nb_devices);
    graph->nodes[graph->nb_devices-1] = node;

    end:
    return (struct optional_size) {(graph->nb_devices)-1, exit_code};
}

/**
 * Detruit le graphe du réseau. Free aussi les switchs du réseau.
 * 
 *      graph : Le graphe qui va etre détruit.
 * 
 */
void deinit_graph(struct net_graph* graph)
{
    if(graph->nodes == NULL)
    {
        DEBUG_WARNING(WARNING_ITEM_NULL_FREEING);
        return;
    }

    struct net_node * node_array = graph->nodes;
    for(size_t i = 0; i < graph->nb_devices; ++i)
    {
        // Si c'est un switch il faut free sa table de routage
        if(node_array[i].device_type == SWITCH)
        {
            deinit_switch(&node_array[i].device.d_switch);
        }

        // Pas besoin de free les pointeurs pointés car ils pointent vers le graphe        
        free(node_array[i].d_connections);
        node_array[i].d_connections = NULL;
    }
    free(graph->nodes);
    graph->nodes = NULL;
}
