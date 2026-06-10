#include "global.h"
#include "errors.h"
#include "neighbourhood_creep.h"
#include "trame.h"
#include "address_ip.h"
#include "address_mac.h"
#include "station.h"
#include "switch.h"
#include "net_graph.h"
#include "table_commutation.h"

#include <stdio.h>
#include <string.h>

// A enlever
#define DEFAULT_PROT 0x0806

/**
 * Montre l'état actuel du réseau.
 */
enum EXIT_CODES network_display(struct net_graph* graph)
{
    enum EXIT_CODES ex_code = SUCESS;

    for(size_t i = 0; i < graph->nb_devices; ++i)
    {
        struct optional_device op_dev = net_graph_get_machine(graph, i);
        
        if(op_dev.exit_code != SUCESS)
        {
            DEBUG_WARNING(op_dev.exit_code);
            return op_dev.exit_code;
        }

        if(op_dev.type == STATION)
        {
            char* ip_str = NULL;
            char* mac_str = NULL;
            
            ex_code = ip_to_string(&ip_str, op_dev.device->d_station.ip);
            if(ex_code != SUCESS)
            {
                DEBUG_WARNING(ex_code);
                return ex_code;
            }
            ex_code = mac_to_string(&mac_str, op_dev.device->d_station.mac);
            if(ex_code != SUCESS)
            {
                DEBUG_WARNING(ex_code);
                free(ip_str);
                return ex_code;
            }

            fprintf(stderr, "Device %zu : Station (mac: %s, ip: %s)\n", i, mac_str, ip_str);

            free(ip_str);
            free(mac_str);
        }
        else
        {
            struct device_switch* swtch = &op_dev.device->d_switch;
            char* mac_str = NULL;
            ex_code = mac_to_string(&mac_str, swtch->_mac);
            if(ex_code != SUCESS)
            {
                DEBUG_WARNING(ex_code);
                return ex_code;
            }

            fprintf(stderr, 
                "Device %zu : Switch (mac : %s, number of used ports : %zu, port capacity : %zu, priority : %d, size of mac table %zu)\n", 
                i,
                mac_str, 
                swtch->nb_port_utilise, 
                swtch->_nbPort, 
                swtch->priority,
                swtch->table_commutation.nb_elems
            );

            free(mac_str);
        }

        // Les connections
        fprintf(stderr, "Indexe(s) of the device(s) connected to device %zu [ ", i);
        size_t* ptr = NULL;
        size_t num = net_graph_surrounding_indexes(graph, &ptr,  i);
        if(num == INVALID_INDEX) return ERROR_BAD_ALLOC;
        for(size_t j = 0; j < num; ++j)
        {
            fprintf(stderr, "%zu ", ptr[j]);
        }
        fprintf(stderr, "]\n\n");
        free(ptr);
    }

    return ex_code;
}

/**
 * Le check null doit etre fait avant la fonction.
 */
static struct optional_mac get_mac_from_device(union device* device, enum DEVICE_TYPE type)
{
    if(!device)
    {
        return (struct optional_mac){ .mac = device->d_station.mac, .exit_code = ERROR_NULL_POINTER_PARAM};
    }

    if(type == STATION)
    {
        return (struct optional_mac){ .mac = device->d_station.mac, .exit_code = SUCESS};
    }
    if(type == SWITCH)
    {
        return (struct optional_mac){ .mac = device->d_switch._mac, .exit_code = SUCESS};
    }
    return (struct optional_mac){ .mac = {{0}}, .exit_code = WARNING_ITEM_NOT_FOUND};
}

/**
 * Commence l'envoie d'un paquet.
 */
void start_sending_packet(struct net_graph* graph, struct neighbourhood* container, size_t src_idx, size_t dest_idx, char* data, size_t data_size, cost_t cost)
{
    if(graph->nb_devices < src_idx || graph->nb_devices < dest_idx)
    {
        // Pas vraiment trés grave.
        DEBUG_WARNING(ERROR_INVALID_TERMINAL_ARGS);
        fprintf(stderr, "Error: choosen indexes must be bewteen 0 and %zu", graph->nb_devices -1);
        return;
    }

    struct optional_device src_dev = net_graph_get_machine(graph, src_idx);
    if(src_dev.exit_code != SUCESS)
    {
        DEBUG_WARNING(src_dev.exit_code);
        return;
    }
    struct optional_device dest_dev = net_graph_get_machine(graph, dest_idx);
    if(dest_dev.exit_code != SUCESS)
    {
        DEBUG_WARNING(dest_dev.exit_code);
        return;
    }

    struct optional_mac src_mac = get_mac_from_device(src_dev.device, src_dev.type);
    if(src_mac.exit_code != SUCESS) 
    {
        DEBUG_WARNING(src_mac.exit_code);
        return;
    }
    struct optional_mac dest_mac = get_mac_from_device(dest_dev.device, dest_dev.type);
    if(dest_mac.exit_code != SUCESS)
    {
        DEBUG_WARNING(dest_mac.exit_code);
        return;
    }

    struct optional_tram tram_op = init_trame(data_size, data, dest_mac.mac, src_mac.mac, DEFAULT_PROT);
    if(tram_op.exit_code != SUCESS)
    {
        DEBUG_WARNING(tram_op.exit_code);
        return;
    }

    // Ajouter dans le container.
    enum EXIT_CODES ex_code = neighbourhood_push_trame(container, &tram_op.tr);

    fprintf(stderr, "Success\n");
}