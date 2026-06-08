#pragma once

#include "errors.h"
#include "address_mac.h"
#include "address_ip.h"
#include "net_graph.h"

enum EXIT_CODES init_net_from_file(const char *filename);
void display_ip(struct ip *);

/**
* Permet de créer un graphe réseau avec un struct net_graph* et un fichier de config.
* Le graphe est rempli avec les informations données dans le fichier.
*
*      - path : Lien vers le fichier de config.
*
*      - graph : Le graph qui va être rempli.
*                REMARQUE IMPORTANTE : Le graphe devrait pas être alloué avant.
*
*
* Retourne : Un code de sortie qui indique si l'operation à échoué ou non.
* Codes de sortie possibles:
*          - SUCESS (en cas de succès)
*          - ERROR_PARSER_NO_FILE
*          - ERROR_PARSER_HEADER
*          - ERROR_PARSING_FORMAT_DEVICES
*          - ERROR_PARSER_UNKNOWN_CONFIG
*          - ERROR_PARSING_FORMAT_NODE
*          - Autres codes d'erreur venu d'autres fonctions utilisé par parse_config.
 */
enum EXIT_CODES parse_config(const char* path, struct net_graph* graph);
