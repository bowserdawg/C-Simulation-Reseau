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
#include "user_interface.h" // Pour get_global_tick()

#include <stdio.h>
#include <string.h>

// A enlever
#define DEFAULT_PROT 0x0806

/**
 * Montres les trames dans le réseau avec du couleur et une légende à la fin.
 */
void display_trames(struct neighbourhood* container)
{
    const size_t len = neighbourhood_get_len(container);

    size_t num_user = 0;
    for(size_t i = 0; i < len; ++i)
    {
        if(neighbourhood_is_present(container, i))
        {
            struct optional_tram tram_op = neighbourhood_get_trame(container, i);
            if(tram_op.exit_code != SUCCESS)
            {
                DEBUG_WARNING(tram_op.exit_code);
                fprintf(stderr, "Error : couldn't display trames.\n");
                return;
            }

            char* colored;
            enum EXIT_CODES ex = trame_to_hex_ansi(tram_op.tr, &colored);
            if(ex != SUCCESS)
            {
                DEBUG_WARNING(ex);
                fprintf(stderr, "Error : couldn't display trames.\n");
                return;
            }
            fprintf(stderr, "Trame%zu %zu \n%s\n\n", num_user,i,colored);
            free(colored);
            num_user++;
        }
    }

    // Envoie une chaine statique montrant la légende
    fprintf(stderr, "%s", trame_get_colored_legend());
}

/**
 * Montre l'état actuel du réseau.
 */
enum EXIT_CODES network_display(struct net_graph* graph)
{
    enum EXIT_CODES ex_code = SUCCESS;

    for(size_t i = 0; i < graph->nb_devices; ++i)
    {
        struct optional_device op_dev = net_graph_get_machine(graph, i);
        
        if(op_dev.exit_code != SUCCESS)
        {
            DEBUG_WARNING(op_dev.exit_code);
            return op_dev.exit_code;
        }

        if(op_dev.type == STATION)
        {
            char* ip_str = NULL;
            char* mac_str = NULL;
            
            ex_code = ip_to_string(&ip_str, op_dev.device->d_station.ip);
            if(ex_code != SUCCESS)
            {
                DEBUG_WARNING(ex_code);
                return ex_code;
            }
            ex_code = mac_to_string(&mac_str, op_dev.device->d_station.mac);
            if(ex_code != SUCCESS)
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
            if(ex_code != SUCCESS)
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
        if(num == INVALID_INDEX)
        {
            DEBUG_WARNING(ERROR_BAD_ALLOC);
            return ERROR_BAD_ALLOC;
        } 
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
        return (struct optional_mac){ .mac = {{0}}, .exit_code = ERROR_NULL_POINTER_PARAM};
    }

    if(type == STATION)
    {
        return (struct optional_mac){ .mac = device->d_station.mac, .exit_code = SUCCESS};
    }
    if(type == SWITCH)
    {
        return (struct optional_mac){ .mac = device->d_switch._mac, .exit_code = SUCCESS};
    }
    return (struct optional_mac){ .mac = {{0}}, .exit_code = WARNING_ITEM_NOT_FOUND};
}

// p_idx == physical, c'est le lien entre les machines
static void queue_packet(struct net_graph* graph, struct neighbourhood* container,
    struct mac dest_mac, struct mac src_mac, size_t p_dest_idx,
    size_t p_src_idx, cost_t cost, const char* data, size_t data_size)
{
    // init_trame fait une copie du data
    struct optional_tram tram_op = init_trame(data_size, data, dest_mac, src_mac, DEFAULT_PROT);
    if(tram_op.exit_code != SUCCESS)
    {
        DEBUG_WARNING(tram_op.exit_code);
        return;
    }

    // Ajouter dans le container.
    enum EXIT_CODES ex_code = neighbourhood_push_trame(container, &tram_op.tr, cost, p_dest_idx, p_src_idx);
    
    if(ex_code == SUCCESS)
    {
        char* mac_dst = NULL;
        if(mac_to_string(&mac_dst, dest_mac) != SUCCESS)
        {
            return;
        }
        char* mac_src = NULL;
        if(mac_to_string(&mac_src, src_mac) != SUCCESS)
        {
            free(mac_dst);
            return;
        }
        fprintf(stderr, 
            "Queued packet successfully sent at tick %zu: \n Ethernet(dest:%s, src:%s) \n Physical(from dev%zu to dev%zu) \n\n",
                get_global_tick(),
                mac_dst,
                mac_src,
                p_src_idx,
                p_dest_idx
            );

        free(mac_dst);
        mac_dst = NULL;
        free(mac_src);
        mac_src = NULL;
    }
    else
    {
        deinit_trame(&tram_op.tr);
        DEBUG_WARNING(ex_code);
        fprintf(stderr, "Queue packet ERROR\n");
    }
}

/**
 * L'utilisateur envoie un paquet avec cette fonction.
 */
void user_queues_packet(struct net_graph* graph, struct neighbourhood* container, size_t src_idx, size_t dest_idx, const char* data, size_t data_size, cost_t cost)
{
    // Check si l'index de machine est correcte
    if(graph->nb_devices <= src_idx || graph->nb_devices <= dest_idx)
    {
        // Pas vraiment trés grave.
        DEBUG_WARNING(ERROR_INDEX_OUT_OF_RANGE);
        fprintf(stderr, "Error: choosen indexes must be bewteen 0 and %zu", graph->nb_devices - 1);
        return;
    }

    // get les device
    struct optional_device src_dev = net_graph_get_machine(graph, src_idx);
    if(src_dev.exit_code != SUCCESS)
    {
        DEBUG_WARNING(src_dev.exit_code);
        return;
    }
    struct optional_device dest_dev = net_graph_get_machine(graph, dest_idx);
    if(dest_dev.exit_code != SUCCESS)
    {
        DEBUG_WARNING(dest_dev.exit_code);
        return;
    }

    // get les mac
    struct optional_mac src_mac = get_mac_from_device(src_dev.device, src_dev.type);
    if(src_mac.exit_code != SUCCESS) 
    {
        DEBUG_WARNING(src_mac.exit_code);
        return;
    }
    struct optional_mac dest_mac = get_mac_from_device(dest_dev.device, dest_dev.type);
    if(dest_mac.exit_code != SUCCESS)
    {
        DEBUG_WARNING(dest_mac.exit_code);
        return;
    }
    

    size_t* indexes;
    size_t nb_indexes = net_graph_surrounding_indexes(graph, &indexes, src_idx);
    if(nb_indexes == INVALID_INDEX)
    {
        DEBUG_WARNING(ERROR_UNREACHABLE_STATE);
        fprintf(stderr, "Fetching indexes failed\n");
        free(indexes);
        return;
    }

    for(size_t i = 0; i < nb_indexes; ++i)
    {
        queue_packet(graph, container, dest_mac.mac, src_mac.mac, indexes[i] , src_idx, cost, data, data_size);
    }

    free(indexes);
}


/**
 * gere les ticks.
 */
void network_handle_tick(struct net_graph* graph, struct neighbourhood* container)
{
    const size_t len = neighbourhood_get_len(container);

    for(size_t i = 0; i < len; ++i)
    {
        if(neighbourhood_is_present(container, i))
        {
            struct optional_tick time = neighbourhood_decrease_time(container, i);
            if(time.exit_code != SUCCESS)
            {
                DEBUG_WARNING(time.exit_code);
                continue;
            }
            // Le temps restant est 0, il faut changer le paquet !
            if(!time.time)
            {
                struct optional_tram tram_op = neighbourhood_get_trame(container, i);
                if(tram_op.exit_code != SUCCESS)
                {
                    DEBUG_WARNING(tram_op.exit_code);
                    continue;
                }

                struct mac mac_dest = trame_get_destination(tram_op.tr);
                struct mac mac_source = trame_get_source(tram_op.tr);

                struct optional_device dev = net_graph_get_device_from_mac(graph, mac_source);
                if(dev.exit_code != SUCCESS)
                {
                    DEBUG_WARNING(dev.exit_code);
                    continue;
                }

                // Si la machine source destination est une station, c'est fini, le payload est arrivé à la maison.
                // On met un body quand meme si on veut rajouter des features dans le future.
                if(dev.type == STATION)
                {
                    size_t p_src_idx = neighbourhood_get_src(container, i);
                    size_t p_dest_idx = neighbourhood_get_dest(container, i);

                    char* mac_dst = NULL;
                    if(mac_to_string(&mac_dst, mac_dest) != SUCCESS)
                    {
                        continue;
                    }
                    char* mac_src = NULL;
                    if(mac_to_string(&mac_src, mac_source) != SUCCESS)
                    {
                        free(mac_dst);
                        continue;
                    }

                    fprintf(stderr, 
                    "Queued packet successfully arrived at station : \n Ethernet(dest:%s, src:%s) \n Physical(from dev%zu to dev%zu) \n\n",
                        mac_dst,
                        mac_src,
                        p_src_idx,
                        p_dest_idx
                    );
                    free(mac_dst);
                    free(mac_src);
                }
                // Sinon, le switch se gère de router le paquet.
                else if(dev.type == SWITCH)
                {
                    // Destinations physiques et non mac
                    size_t dest_new_src = neighbourhood_get_dest(container, i);
                    size_t sender       = neighbourhood_get_src(container, i);
                    size_t* indexes;
                    size_t nb_indexes = net_graph_surrounding_indexes(graph, &indexes, dest_new_src);
                    port_t sent_port_index = INVALID_PORT;
                    for(port_t j = 0; j < nb_indexes; ++j)
                    {
                        if(indexes[j] == sender)
                        {
                            sent_port_index = j;
                        }
                    }
                    
                    // Maintenant, ajouter dans la table du switch le MAC source et le port associé.
                    table_commutation_add(&dev.device->d_switch.table_commutation, sent_port_index, mac_source);

                    // Voir si la destination est dans la table.
                    struct optional_port port_op = table_commutation_lookup(dev.device->d_switch.table_commutation, mac_dest);
                    if(port_op.exit_code != SUCCESS)
                    {
                        DEBUG_WARNING(port_op.exit_code);
                        free(indexes);
                        continue;
                    }

                    // Si oui, envoyer le paquet vers la machine.
                    const void* data;
                    // Pas de gestion d'erreur, peut faire un gros segfault car le struct trame est trés dangeureux.
                    size_t copy_data_size = trame_get_data(tram_op.tr, &data);

                    // Si l'adresse mac est dans la table de lookup
                    if(port_op.port != INVALID_INDEX)
                    {
                        // Unicast
                        cost_t new_cost  =     net_graph_get_cost_from_two_indexes(graph, dest_new_src, port_op.port);
                        if(new_cost == COST_MAX)
                        {
                            fprintf(stderr, "Couldn't find the copy cost\n");
                            free(indexes);
                            continue;
                        }

                        // "la destination est la nouvelle source"
                        // Crée une nouvelle trame, le data va etre copier
                        queue_packet(graph, container, mac_dest, mac_source, port_op.port, dest_new_src, new_cost, data, copy_data_size);
                    }
                    // Sinon, on flood
                    else
                    {
                        // Multicast
                        for(size_t j = 0; j < nb_indexes; ++j)
                        {
                            enum SWITCH_PORT_STATUS port_status = switch_get_port_status(&dev.device->d_switch, j);
                            // Envoie un paquet à chaque port non bloqué et pas a l'envoyeur
                            if( ((port_status == ROOT) || (port_status == DESIGNATED)) && (indexes[j] != sender))
                            {
                                cost_t copy_cost  =     net_graph_get_cost_from_two_indexes(graph, dest_new_src, indexes[j]);
                                queue_packet(graph, container, mac_dest, mac_source, indexes[j], dest_new_src, copy_cost, data, copy_data_size);
                            }
                        }
                    }

                    free(indexes);
                }
                // Deinit l'acien tram pour de bon, AVEC UN POP ET NON UN DEINIT TRAME
                neighbourhood_pop_trame(container, i);
            }  
        }

    }
}