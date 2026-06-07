#include "net_graph.h"
#include "station.h"
#include "switch.h"
#include "errors.h"

enum device_type{
    NONE,
    STATION,
    SWITCH,
};

// Le sommet d'un graphe de réseau est juste une machine.
struct net_node {
    union device device;
    cost_t cost;
    struct net_node* _connected_nodes;
    size_t nb_connected_nodes;
    enum device_type _device_type;
};

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
    g._nb_devices = nb_devices;
    return (struct optional_net_graph){ .graph = g, .exit_code = SUCESS };
}

/**
 * Detruit le graphe du réseau.
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

    for(size_t i = 0; i < graph->_nb_devices; ++i)
    {
        if(graph->nodes[i]._connected_nodes == NULL) DEBUG_WARNING(WARNING_ITEM_NULL_FREEING);
        free(graph->nodes[i]._connected_nodes);
    }
    free(graph->nodes);
    graph->nodes = NULL;
}
