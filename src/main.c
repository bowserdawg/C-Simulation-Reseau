#include "parser.h"
#include "errors.h"
#include "address_ip.h"
#include "address_mac.h"
#include "table_commutation.h"
#include "switch.h"
#include "net_graph.h"
#include "global.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) 
{
    enum EXIT_CODES exit_code = SUCESS;

    if(argc < 2)
    {
        printf("Usage : %s [fichier_config]\n", G_APP_NAME);
        DEBUG_WARNING(ERROR_INVALID_TERMINAL_ARGS);
        exit_code = ERROR_INVALID_TERMINAL_ARGS;
    }
    else
    {
        exit_code = init_net_from_file(argv[1]);
        if(exit_code != SUCESS)
        {
            perror(G_APP_NAME);
        }
    }

    return (exit_code == SUCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}
