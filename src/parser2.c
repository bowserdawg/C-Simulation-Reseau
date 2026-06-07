#include "errors.h"
#include "switch.h"
#include "net_graph.h"
#include "global.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#define IP_BYTE_SIZE 4
#define MAC_BYTE_SIZE 6
#define BUF_LEN 256

enum DEVICE_TYPE {
    NO_DEVICE,
    DEV_STATION,
    DEV_SWITCH
};

// Fonctions d'aides
/***************************************************************************************/

/* "HH:MM:SS:PP:QQ:RR" en mac[6]. Retourne SUCCESS si tous ce passe bien. */
static enum EXIT_CODES parse_mac(const char *s, unsigned char mac[6])
{
    unsigned int b[MAC_BYTE_SIZE];
    if (sscanf(s, "%x:%x:%x:%x:%x:%x", &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]) != 6)
        return ERROR_INVALID_STRING_FORMAT;

    for (int i = 0; i < 6; ++i) 
    {
        if (b[i] > 0xFF) return -1;
        mac[i] = (unsigned char)b[i];
    }
    return SUCESS;
}
 
/* "A.B.C.D" en ip[4]. Retourne SUCCESS si tous ce passe bien. */
static enum EXIT_CODES parse_ip(const char *s, unsigned char ip[4])
{
    unsigned int b[IP_BYTE_SIZE];
    if (sscanf(s, "%u.%u.%u.%u", &b[0], &b[1], &b[2], &b[3]) != 4)
        return ERROR_INVALID_STRING_FORMAT;

    for (int i = 0; i < 4; ++i) 
    {
        if (b[i] > 255) return -1;
        ip[i] = (unsigned char)b[i];
    }
    return SUCESS;
}

/* Pour effacer les '\r' and '\n' à la fin d'une ligne. */
static void chomp(char *line)
{
    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
        line[--len] = '\0';
}

/**
 * struct net_graph* graph devrait pas etre alloqué avant.
 */
enum EXIT_CODES parse_config(const char* path, struct net_graph* graph)
{
    /* Entete */
    /*************************************************************************************
     Ouvrir le fichier */
    FILE *file = fopen(path, "r");
    if (!file) {
        DEBUG_WARNING(ERROR_PARSER_NO_FILE);
        return ERROR_PARSER_NO_FILE;
    }

    /* Allocation dynamique */
    /*************************************************************************************/

    /* taille assez grande pour une ligne */
    char line[256];

    /* La fonction sscanf permet de lire une ligne de string et de convertir 
       certaines parties en un type spécifié.
       Elle renvoie le nombre de zone qu'elle a pu convertir.  */
    size_t n_devices, n_nodes;
    if (sscanf(line, "%zu %zu", &n_devices, &n_nodes) != 2) {
        fclose(file);
        fprintf(stderr, "%s: bad header: '%s'\n", G_APP_NAME, line);
        return ERROR_PARSER_HEADER;
    }

    struct optional_net_graph op_net = init_net_graph(n_devices);
    if(op_net.exit_code != SUCESS)
    {
        fclose(file);
        fprintf(stderr, "%s: Failed to initialise graph'\n", G_APP_NAME);
        DEBUG_WARNING(op_net.exit_code);
        return op_net.exit_code;
    }
    *graph = op_net.graph;

    /* Remplir le réseau avce les machines */
    /*************************************************************************************/
    for(size_t i = 0; i < n_devices; ++i)
    {
        if(!fgets(line, sizeof(line), file))
        {
            fprintf(stderr, "%s: unexpected EOF at device line %zu\n", G_APP_NAME, i);
            fclose(file);
            deinit_graph(graph);
            DEBUG_WARNING(ERROR_PARSING_FORMAT_DEVICES);
            return ERROR_PARSING_FORMAT_DEVICES;
        }
        // Efface tous les \n et \r
        chomp(line);

        // Copy la ligne dans un buffer
        char buf[256];
        strncpy(buf, line, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        
        // Tokenize le buffer sur les caractéres ';'
        char *tok = strtok(buf, ";");
        if (!tok) 
        {
            fprintf(stderr, "%s: empty device line at %zu\n %s \n", G_APP_NAME, i, line);
            fclose(file);
            deinit_graph(graph);
            DEBUG_WARNING(ERROR_PARSING_FORMAT_DEVICES);
            return ERROR_PARSING_FORMAT_DEVICES;
        }   

        // Definir le type de la machine
        enum DEVICE_TYPE type = atoi(tok);
        if(type == 0)
        {
            fprintf(stderr, "%s: invalid device type at %zu\n %s \n", G_APP_NAME, i, line);
            fclose(file);
            deinit_graph(graph);
            DEBUG_WARNING(ERROR_PARSING_FORMAT_DEVICES);
            return ERROR_PARSING_FORMAT_DEVICES;
        }

        if(type == DEV_STATION)
        {
            // Les token en version string
            /* <type>;<mac>;<ip> */
            char* mac_str = strtok(NULL, ";");
            char* ip_str = strtok(NULL, ";");
            
            // Version tableau (va etre convertie)
            octet_ip_t  mac[MAC_BYTE_SIZE] = {0};
            octet_mac_t ip [IP_BYTE_SIZE]  = {0};

            // Les addresses mac ou ip n'existes pas
            if(!mac_str || !ip_str)
            {
                fprintf(stderr, "%s: incorrect station device format at line %zu :\n '%s'\n", G_APP_NAME, i, line);
                deinit_graph(graph);
                fclose(file);
                DEBUG_WARNING(ERROR_PARSING_FORMAT_DEVICES);
                return ERROR_PARSING_FORMAT_DEVICES;
            }

            // Les addresses 
            if (parse_mac(mac_str, mac) != SUCESS)
            {
                fprintf(stderr, "%s: incorrect MAC format in station line: '%s'\n", G_APP_NAME, line);
                deinit_graph(graph);
                fclose(file);
                DEBUG_WARNING(ERROR_PARSING_FORMAT_DEVICES);
                return ERROR_PARSING_FORMAT_DEVICES;
            }
            if (parse_ip(ip_str, ip) != SUCESS) 
            {
                fprintf(stderr, "%s: incorrect IP format in station line: '%s'\n", G_APP_NAME, line);
                deinit_graph(graph);
                fclose(file);
                DEBUG_WARNING(ERROR_PARSING_FORMAT_DEVICES);
                return ERROR_PARSING_FORMAT_DEVICES;
            }

            struct device_station station = init_station(
                init_ip(ip[0], ip[1], ip[2], ip[3]),
                init_mac(mac[0], mac[1], mac[2], mac[3], mac[4], mac[5])
            );

            net_graph_add_station(graph, station);
        }
        else if (type == DEV_SWITCH)
        {
            // Les token en version string
            /* <type>;<mac>;<nb port>;<priorité>*/
            char *mac_str = strtok(NULL, ";");
            char *nb_port_str = strtok(NULL, ";"); 
            char *pri_str = strtok(NULL, ";");

            octet_mac_t mac[MAC_NB_OCTET] = {0};

            if (!mac_str || !nb_port_str || !pri_str) {
                fprintf(stderr, "%s: incorrect switch device format at line %zu :\n '%s'\n", G_APP_NAME, i, line);
                deinit_graph(graph);
                fclose(file);
                DEBUG_WARNING(ERROR_PARSING_FORMAT_DEVICES);
                return ERROR_PARSING_FORMAT_DEVICES;
            }

            // L'addresse mac 
            if (parse_mac(mac_str, mac) != SUCESS)
            {
                fprintf(stderr, "%s: incorrect MAC format in station line: '%s'\n", G_APP_NAME, line);
                deinit_graph(graph);
                fclose(file);
                DEBUG_WARNING(ERROR_PARSING_FORMAT_DEVICES);
                return ERROR_PARSING_FORMAT_DEVICES;
            }

            // Les paramétres qu'il faut  pour construire un switch
            struct mac ad_mac = init_mac(mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
            port_t nb_port = (priority_t)atoi(nb_port_str);
            priority_t prio = (priority_t)atoi(pri_str);
            
            struct optional_device_switch swtch = init_switch(ad_mac, nb_port, prio);
            if (swtch.exit_code != SUCESS)
            {
                fprintf(stderr, "%s: init_switch failed.\n", G_APP_NAME);
                deinit_graph(graph);
                fclose(file);
                DEBUG_WARNING(swtch.exit_code);
                return swtch.exit_code;
            }

            net_graph_add_switch(graph, swtch.dev_switch);
        }
        else
        {
            fprintf(stderr, "%s : Unknown device type %d ", G_APP_NAME, type);
            DEBUG_WARNING(ERROR_PARSER_UNKNOWN_CONFIG);
            return ERROR_PARSER_UNKNOWN_CONFIG;
        }
    }

    /* Remplir le réseau avec les liens */
    /*************************************************************************************/

    fclose(file);

    return SUCESS;
}

