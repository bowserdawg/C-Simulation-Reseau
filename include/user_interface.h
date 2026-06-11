#pragma once

#include "net_graph.h"
#include "neighbourhood_creep.h"

/**
 * Montres les trames dans le réseau avec du couleur et une légende à la fin.
 */
void display_trames(struct neighbourhood* container);

/**
 * L'utilisateur et le programme intéragissent grace à cette ffonction.
 */
enum EXIT_CODES user_loop(struct net_graph graph, struct neighbourhood * neighbourhood);

/**
 * Donne le nombre de tick.
 */
size_t get_global_tick();
