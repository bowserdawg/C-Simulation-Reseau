#include "parser.h"
#include "net_graph.h"
#include "errors.h"

#include <stddef.h>
#include <stdio.h>

void test_parser()
{
    // Il faut prendre en compte que l'executable "test_runner" est dans 
    // un dossier obscure, mais elle est éxecuté dans le dossier root.
    const char* path_to_file = "./data/configs/my_better_lan";
    struct net_graph graph = {0};

    DEBUG_ASSERT_W_MESSAGE(parse_config(path_to_file, &graph) == SUCESS, "FAIL parsing should have been successful\n");

    // Dans my_better_lan...
    /*
        0;1;4
        0;2;19
        1;2;4
        0;3;0
        2;4;0
    */

   // nbde connection pour switch
    DEBUG_ASSERT_W_MESSAGE(net_graph_get_machine(&graph, 0).device->d_switch.nb_port_utilise == 3, "FAIL incorrect port numbers.\n");

    // Devrait marcher
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 0, 1).exit_code == SUCESS, "FAIL net_graph_devices_are_connected bad exit code.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 0, 2).exit_code == SUCESS, "FAIL net_graph_devices_are_connected bad exit code.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 1, 2).exit_code == SUCESS, "FAIL net_graph_devices_are_connected bad exit code.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 0, 3).exit_code == SUCESS, "FAIL net_graph_devices_are_connected bad exit code.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 2, 4).exit_code == SUCESS, "FAIL net_graph_devices_are_connected bad exit code.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 0, 1).boolean == true, "FAIL net_graph_devices_are_connected bad result.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 0, 2).boolean == true, "FAIL net_graph_devices_are_connected bad result.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 1, 2).boolean == true, "FAIL net_graph_devices_are_connected bad result.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 0, 3).boolean == true, "FAIL net_graph_devices_are_connected bad result.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 2, 4).boolean == true, "FAIL net_graph_devices_are_connected bad result.\n");


    // devrait pas marcher
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 8, 1).exit_code == ERROR_INDEX_OUT_OF_RANGE, "FAIL net_graph_devices_are_connected bad exit code.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 9, 2).exit_code == ERROR_INDEX_OUT_OF_RANGE, "FAIL net_graph_devices_are_connected bad exit code.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 4, 1).exit_code == SUCESS, "FAIL net_graph_devices_are_connected bad exit code.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 2, 3).exit_code == SUCESS, "FAIL net_graph_devices_are_connected bad exit code.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 2, 2).exit_code == SUCESS, "FAIL net_graph_devices_are_connected bad exit code.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 8, 1).boolean == false, "FAIL net_graph_devices_are_connected bad result.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 9, 2).boolean == false, "FAIL net_graph_devices_are_connected bad result.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 4, 1).boolean == false, "FAIL net_graph_devices_are_connected bad result.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 2, 3).boolean == false, "FAIL net_graph_devices_are_connected bad result.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 2, 2).boolean == false, "FAIL net_graph_devices_are_connected bad result.\n");


    deinit_graph(&graph);
}