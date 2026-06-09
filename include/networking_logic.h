#pragma once

#include "errors.h"
#include "net_graph.h"

/**
 * Montre l'état actuel du réseau.
 */
enum EXIT_CODES network_display(struct net_graph* graph);
