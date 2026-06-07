#include "parser.h"
#include "errors.h"
#include "address_ip.h"
#include "address_mac.h"
#include "table_commutation.h"
#include "switch.h"
#include "net_graph.h"

#include <stdio.h>
#include <stdlib.h>

#define NET_APP_NAME "main"

int main(int argc, char *argv[]) 
{
    enum EXIT_CODES exit_code = SUCESS;

    if(argc < 2)
    {
        printf("Usage : %s [fichier_config]\n", NET_APP_NAME);
        DEBUG_WARNING(ERROR_INVALID_TERMINAL_ARGS);
        exit_code = ERROR_INVALID_TERMINAL_ARGS;
    }
    else
    {
        exit_code = init_net_from_file(argv[1]);
        if(exit_code != SUCESS)
        {
            perror(NET_APP_NAME);
        }
    }

    struct optional_net_graph op_graph = init_net_graph(20);

    DEBUG_ASSERT_W_MESSAGE(op_graph.exit_code == SUCESS, "FAIL init graph failed.\n");
    DEBUG_ASSERT_W_MESSAGE(op_graph.graph.nodes != NULL, "FAIL init graph failed because pointer is null.\n");

    struct net_graph graph = op_graph.graph;


    struct optional_device_switch op_sw1 = init_switch(
        init_mac(0xAF, 0x76, 0xFA, 0xEA, 0x09, 0x12), 8
    );
    DEBUG_ASSERT_W_MESSAGE(op_sw1.exit_code == SUCESS, "FAIL init_switch failed.\n");
    struct device_switch sw1 = op_sw1.dev_switch;

    struct device_station st1 = init_station(
                    init_ip(12, 138, 192, 12), 
                    init_mac(0xF2, 0x66, 0xFA, 0xAD, 0x67, 0x92)
                );

    struct optional_size s1 = net_graph_add_station(&graph, st1);
    struct optional_size s2 = net_graph_add_switch(&graph, sw1);

    DEBUG_ASSERT_W_MESSAGE(s1.exit_code == SUCESS, "FAIL adding station operation failed\n");
    DEBUG_ASSERT_W_MESSAGE(s2.exit_code == SUCESS, "FAIL adding switch operation failed\n");

    DEBUG_ASSERT_W_MESSAGE(graph.nb_devices == 2, "FAIL incorrect number of devices.\n");

    deinit_graph(&graph);

    DEBUG_ASSERT_W_MESSAGE(graph.nodes == NULL, "FAIL deallocation failed for net_graph.\n");

    return (exit_code == SUCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}
