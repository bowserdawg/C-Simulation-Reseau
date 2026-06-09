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

            //swtch->nb_port_utilise
            //swtch->priority
            //swtch->_mac
            //swtch->_nbPort
            //swtch->table_commutation

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
        fprintf(stderr, "Indexe(s) of the devices connected to device %zu [ ", i);
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