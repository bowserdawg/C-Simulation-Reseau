#include "parser.h"
#include "errors.h"
#include "address_ip.h"
#include "address_mac.h"
#include "table_commutation.h"
#include "switch.h"
#include "net_graph.h"
#include "global.h"
#include "trame.h"
#include "neighbourhood_creep.h"
#include "user_interface.h"

#include <stdio.h>
#include <stdlib.h>

#define CONFIG_FILES_PATH "./data/configs/"
#define MAX_FILENAME_LENGTH 256

const char* DATA = "Hello World";

int main(int argc, char *argv[]) 
{
    enum EXIT_CODES exit_code = SUCESS;
    struct net_graph graph = {NULL, 0, 0};
    
    struct neighbourhood* neighbourhood;


    if(argc < 2)
    {
        printf("Usage : %s [fichier_config]\n", G_APP_NAME);
        DEBUG_WARNING(ERROR_INVALID_TERMINAL_ARGS);
        exit_code = ERROR_INVALID_TERMINAL_ARGS;
    }
    else
    {
        char filepath[MAX_FILENAME_LENGTH];
        snprintf(filepath, MAX_FILENAME_LENGTH, "%s%s", CONFIG_FILES_PATH, argv[1]);

        exit_code = parse_config(filepath, &graph);

        if(exit_code == SUCESS)
        {
            struct neighbourhood_ptr_optional nei_op = init_neighbourhood(graph.nb_devices);
            exit_code = nei_op.exit_code;
            if(exit_code == SUCESS)
            {
                neighbourhood = nei_op.n_ptr;

                // start la simulation ici
                user_loop(graph, neighbourhood);

                deinit_neighbourhood(neighbourhood);
            }
            else
            {
                DEBUG_WARNING(SUCESS);
                fprintf(stderr, "%s: Error initialising network\n", G_APP_NAME);
            }
        }
        else
        {
            fprintf(stderr, "%s: Parsing failed, exiting program\n", G_APP_NAME);
        }
    }
    
    // On peut free meme s'il est NULL car c'est possible.
    deinit_graph(&graph);
    

    return (exit_code == SUCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}
