#pragma once

#include "errors.h"
#include "net_graph.h"
#include "neighbourhood_creep.h"


/**
 * Montre l'état actuel du réseau.
 */
enum EXIT_CODES network_display(struct net_graph* graph);

/**
 * L'utilisateur envoie un paquet avec cette fonction.
 */
void user_queues_packet(struct net_graph* graph, struct neighbourhood* container, size_t src_idx, size_t dest_idx, const char* data, size_t data_size, cost_t cost);

/**
 * gere les ticks.
 */
void network_handle_tick(struct net_graph* graph, struct neighbourhood* container);