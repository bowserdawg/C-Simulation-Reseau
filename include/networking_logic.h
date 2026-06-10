#pragma once

#include "errors.h"
#include "net_graph.h"
#include "neighbourhood_creep.h"


/**
 * Montre l'état actuel du réseau.
 */
enum EXIT_CODES network_display(struct net_graph* graph);

/**
 * Commence l'envoie d'un paquet.
 */
void start_sending_packet(struct net_graph* graph, struct neighbourhood* container, size_t src_idx, size_t dest_idx, char* data, size_t data_size, cost_t cost);