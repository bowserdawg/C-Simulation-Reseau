#include "net_graph.h"
#include "station.h"
#include "switch.h"
#include "errors.h"
#include "global.h" // pour INVALID_INDEX et DEVICE_TYPE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Le sommet d'un graphe de réseau est juste une machine.
struct net_node {
    union device       device;
    struct connections* d_connections;
    size_t             nb_connected_nodes;
    size_t             cap_connected_nodes;
    enum DEVICE_TYPE   device_type;
};

struct connections {
    cost_t           cost;
    struct net_node* connected_node;
};

#define INITIAL_NUM_CONNEXIONS 4

/**
 * Crée une nouvelle graphe de réseau.
 * 
 *       nb_devices : Nombre de machine dans le réseau.
 * 
 * Retourne : Un optional qui peut contenir un graphe ou un méssage d'erreur.
 */
struct optional_net_graph init_net_graph(size_t devices_capacity)
{
    struct net_graph g = {0};

    // Calloc permet que tous les pointeurs soit null au debut.
    g.nodes = calloc(devices_capacity, sizeof(struct net_node));
    if(g.nodes == NULL)
    {
        return (struct optional_net_graph){ .graph = {0}, .exit_code = ERROR_BAD_ALLOC };
    }
    g.nb_devices = 0;
    g.cap_devices = devices_capacity;

    return (struct optional_net_graph){ .graph = g, .exit_code = SUCESS };
}

/************************************************************************
 * Fonctions d'aides pour net_graph_add_station et net_graph_add_switch *
 ************************************************************************/

static enum EXIT_CODES check_null(struct net_graph* graph)
{
    if(!graph)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return ERROR_NULL_POINTER_PARAM;
    }
    return SUCESS;
}

static enum EXIT_CODES check_size(struct net_graph* graph)
{
    if(graph->nb_devices+1 > graph->cap_devices)
    {
        DEBUG_WARNING(ERROR_CAPACITY_REACHED);
        return ERROR_CAPACITY_REACHED;
    }
    return SUCESS;
}


static enum EXIT_CODES get_connections_ptr_array(struct connections** connecs, size_t size)
{
    *connecs = calloc(size, sizeof(struct connections));
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
    // Check si le graphe est null ou non
    enum EXIT_CODES exit_code = check_null(graph);
    if(exit_code != SUCESS) goto end;

    // Check si il reste de la capaicté dans le graphe.
    exit_code = check_size(graph);
    if(exit_code != SUCESS) goto end;

    // Alloc l'emplacement pour tous les pointeurs nécessaires
    struct connections* connecs = NULL;
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
    // Check si le graphe est null ou non
    enum EXIT_CODES exit_code = check_null(graph);
    if(exit_code != SUCESS) goto end;

    // Check si il reste de la capaicté dans le graphe.
    exit_code = check_size(graph);
    if(exit_code != SUCESS) goto end;

    // Alloc l'emplacement pour tous les pointeurs nécessaires
    struct connections* connecs = NULL;
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
 * Fonction d'aide pour ajouter une connection à une machine.
 */
static enum EXIT_CODES add_node(const struct net_graph* graph, size_t device_index, size_t adj_index, cost_t cost)
{
    // Raccourcis vers le sommet.
    struct net_node* node = graph->nodes+device_index;
    if(!node)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return ERROR_NULL_POINTER_PARAM;
    }
    if(!node->d_connections)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return ERROR_NULL_POINTER_PARAM;
    }

    // Raccourcis vers la taille et la capacité.
    size_t capacity = node->cap_connected_nodes;
    size_t nb_nodes = node->nb_connected_nodes;

    // Si il n'a pas assez de place...
    if(nb_nodes >= capacity)
    {
        // Allocation d'espace.
        struct connections* connecs = NULL;
        enum EXIT_CODES ex_code = get_connections_ptr_array(&connecs, capacity*2);  // calloc
        if(ex_code != SUCESS)
        {
            DEBUG_WARNING(ex_code);
            return ex_code;
        }
        // Copie dans le nouveau tableau et libere l'ancien.
        memcpy(connecs, node->d_connections, nb_nodes*sizeof(struct connections));
        free(node->d_connections);                                  // free
        // Affecter le nouveau bloc mémoire à la connection.
        node->d_connections = connecs;
        node->cap_connected_nodes = capacity*2;
    }

    // Ajouter le pointeur vers l'autre sommet
    node->d_connections[nb_nodes].connected_node = graph->nodes+adj_index;
    // Le cout de la connection
    node->d_connections[nb_nodes].cost = cost;
    // Augmenter le nombre de sommet adjacent (degré).
    node->nb_connected_nodes++;

    // Il faut faire des chose specials au switchs
    if(node->device_type == SWITCH)
    {
        if(node->device.d_switch.nb_port_utilise >= node->device.d_switch._nbPort) return ERROR_SWITCH_SEMANTIC_ERROR;   
        node->device.d_switch.nb_port_utilise++;   
    }

    return SUCESS;
}

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
enum EXIT_CODES net_graph_add_node(const struct net_graph* graph, size_t index1, size_t index2, cost_t cost)
{
    // Le graph ne doit pas etre null.
    if(!graph)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return ERROR_NULL_POINTER_PARAM;
    }
    // L'indexe ne peut pas dépasser la taille.
    size_t len = graph->nb_devices;
    if(index1 >= len || index2 >= len)
    {
        DEBUG_WARNING(ERROR_INDEX_OUT_OF_RANGE);
        return ERROR_INDEX_OUT_OF_RANGE;
    }
    // Un sommet ne peut pas etre connecté à elle meme
    if(index1 == index2)
    {
        DEBUG_WARNING(ERROR_NODE_SEMANTIC_ERROR_SAME_INDEX);
        return ERROR_NODE_SEMANTIC_ERROR_SAME_INDEX;
    }

    enum EXIT_CODES ex_code1 = add_node(graph, index1, index2, cost);
    enum EXIT_CODES ex_code2 = add_node(graph, index2, index1, cost);

    if(ex_code1 != SUCESS) return ex_code1;
    if(ex_code2 != SUCESS) return ex_code2;
    return SUCESS;
}

/**
 * Cherche si deux machines sont connectés.
 */
struct optional_bool net_graph_devices_are_connected(const struct net_graph* graph, size_t index1, size_t index2)
{   
    // On laisse le programme crash si le graph est null car c'est une erreur grave.
    if(!graph)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return (struct optional_bool){false, ERROR_NULL_POINTER_PARAM};
    }
    if(graph->nb_devices <= index1 || graph->nb_devices <= index2)
    {
        // L'index est "out of range".
        DEBUG_WARNING(ERROR_INDEX_OUT_OF_RANGE);
        return (struct optional_bool){false, ERROR_INDEX_OUT_OF_RANGE};
    }

    const struct net_node* searching_node = graph->nodes+index1;
    const struct net_node* searched_node  = graph->nodes+index2;

    for(size_t i = 0; i < searching_node->nb_connected_nodes; ++i)
    {
        if( searching_node->d_connections[i].connected_node == searched_node )
        {
            return (struct optional_bool){true, SUCESS};
        }
    }

    return (struct optional_bool){false, SUCESS};
}

/**
 * Renvoie la machine demandé en tant que union.
 */
struct optional_device net_graph_get_machine(const struct net_graph* graph, size_t index)
{
    if(!graph || !graph->nodes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return (struct optional_device)
        {
            .device    = NULL,
            .type      = NONE,
            .exit_code = ERROR_NULL_POINTER_PARAM
        };
    }

    if(graph->nb_devices <= index)
    {
        DEBUG_WARNING(ERROR_INDEX_OUT_OF_RANGE);
        return (struct optional_device)
        {
            .device    = NULL,
            .type      = NONE,
            .exit_code = ERROR_INDEX_OUT_OF_RANGE
        };
    }

    return (struct optional_device)
    {
        .device    = &graph->nodes[index].device,
        .type      = graph->nodes[index].device_type,
        .exit_code = SUCESS
    };
}

/**
 * Donne l'index de tous les machines adjacents.
 * 
 * Donne INVALID_INDEX si une erreur est arrivé. 
 * 
 * Le pointeur donne doit etre free aprés.
 */
port_t net_graph_surrounding_indexes(struct net_graph* graphe, size_t** indexes, size_t index)
{
    if(!graphe || !graphe->nodes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return INVALID_INDEX;
    }

    if(index >= graphe->nb_devices)
    {
        DEBUG_WARNING(ERROR_INDEX_OUT_OF_RANGE);
        return INVALID_INDEX;
    }

    *indexes = malloc(sizeof(size_t) * graphe->nodes[index].nb_connected_nodes);
    if(!*indexes)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return INVALID_INDEX;
    }

    for(size_t i = 0; i < graphe->nodes[index].nb_connected_nodes; ++i)
    {
        // Based ptr arithmetic
        ptrdiff_t idx = (graphe->nodes[index].d_connections[i].connected_node) - graphe->nodes;

        if(idx < 0)
        {
            DEBUG_WARNING(ERROR_UNREACHABLE_STATE);
            free(*indexes);
            *indexes = NULL;
            return INVALID_INDEX;
        }

        (*indexes)[i] = (size_t)idx;
    }

    return graphe->nodes[index].nb_connected_nodes;
}

/**
 * Donne l'index de la machine grace à une adressse mac.
 * 
 * Donne INVALID_INDEX si une erreur est arrivé ou le mac est invalide. 
 */
size_t net_graph_find_index_from_mac(struct net_graph* graphe, struct mac mac)
{
    if(!graphe)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return INVALID_INDEX;
    }

    for(size_t i = 0; i < graphe->nb_devices; ++i)
    {
        if(graphe->nodes[i].device_type == STATION)
        {
            if(mac_equals(graphe->nodes[i].device.d_station.mac, mac))
            {
                return i;
            }
        }
        else if(graphe->nodes[i].device_type == SWITCH)
        {
            if(mac_equals(graphe->nodes[i].device.d_switch._mac, mac))
            {
                return i;
            }
        }
        else
        {
            DEBUG_WARNING(ERROR_UNREACHABLE_STATE);
            return INVALID_INDEX;
        }
    }

    return INVALID_INDEX;
}

/**
 * Trouve une machine avec une adresse spécifique.
 */
struct optional_device net_graph_get_device_from_mac(struct net_graph* graph, struct mac mac)
{
    union device* device = NULL;
    enum DEVICE_TYPE type = NONE;
    enum EXIT_CODES ex_code = SUCESS;

    if(!graph || !graph->nodes)
    {
        ex_code = ERROR_NULL_POINTER_PARAM;
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        goto end;
    }

    for(size_t i = 0; i < graph->nb_devices; ++i)
    {
        if(graph->nodes[i].device_type == STATION)
        {
            if(mac_equals(mac, graph->nodes[i].device.d_station.mac))
            {
                device = &graph->nodes[i].device;
                type = STATION;
                break;
            }
        }
        else if(graph->nodes[i].device_type == SWITCH)
        {
            if(mac_equals(mac, graph->nodes[i].device.d_switch._mac))
            {
                device = &graph->nodes[i].device;
                type = SWITCH;
                break;
            }
        }
    }

    end:
    return (struct optional_device) {
        .device = device, 
        .type = type, 
        .exit_code = ex_code};
}

/**
 * Retourne le port dans lequel le message est venu.
 * 
 * Retourne INVALID_PORT si les deux machines ne sont pas connectés.
 */
struct optional_port net_graph_get_received_port_from_macs(struct net_graph* graph, struct mac dest, struct mac src)
{
    for(size_t i = 0; i < graph->nb_devices; ++i)
    {
        struct net_node* current = &graph->nodes[i];
        
        // Cheque si les memes machines
        if(current->device_type == SWITCH)
        {
            if(!mac_equals(current->device.d_switch._mac, dest))
                continue;
        }
        else if(current->device_type == STATION)
        {
            if(!mac_equals(current->device.d_station.mac, dest))
                continue;
        }
        else
        {
            DEBUG_WARNING(ERROR_UNREACHABLE_STATE);
            return (struct optional_port){.port = INVALID_PORT, .exit_code = ERROR_UNREACHABLE_STATE};
        }
        
        // Trouve la destination
        for(size_t j = 0; j < current->nb_connected_nodes; ++j)
        {
            struct net_node* connected = current->d_connections[j].connected_node;
            
            if(connected->device_type == SWITCH)
            {
                if(mac_equals(connected->device.d_switch._mac, src))
                {
                    return (struct optional_port){.port = (port_t)j, .exit_code = SUCESS};
                }
            }
            else if(connected->device_type == STATION)
            {
                if(mac_equals(connected->device.d_station.mac, src))
                {
                    return (struct optional_port){.port = (port_t)j, .exit_code = SUCESS};
                }
            }
        }
        
        // Destination trouvé mais pas la source 
        DEBUG_WARNING(WARNING_NO_PORT_ROUTE);
        return (struct optional_port){.port = INVALID_PORT, .exit_code = WARNING_NO_PORT_ROUTE};
    }
    
    // Destination non trouvé.
    DEBUG_WARNING(ERROR_UNREACHABLE_STATE);
    return (struct optional_port){.port = INVALID_PORT, .exit_code = ERROR_UNREACHABLE_STATE};
}

/**
 * Envoie COST_MAX si les deux index ne sont pas connecté, si ils sont les meme ou une erreur c'est passé.
 */
cost_t net_graph_get_cost_from_two_indexes(struct net_graph* graph, size_t index1, size_t index2)
{
    if(index1 == index2)
    {
        DEBUG_WARNING(ERROR_SAME_ITEM_PARAMETER);
        return COST_MAX;
    }

    if(!graph || !graph->nodes)
    {
        DEBUG_WARNING(ERROR_NULL_POINTER_PARAM);
        return COST_MAX;
    }

    const struct net_node* searching_node = graph->nodes+index1;
    const struct net_node* searched_node  = graph->nodes+index2;

    for(size_t i = 0; i < searching_node->nb_connected_nodes; ++i)
    {
        if( searching_node->d_connections[i].connected_node == searched_node )
        {
            return searching_node->d_connections[i].cost;
        }
    }

    fprintf(stderr, "Couldn't find cost between %zu and %zu\n", index1, index2);

    return COST_MAX;
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
