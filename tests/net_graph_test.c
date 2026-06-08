#include "net_graph.h"
#include "errors.h"

#include <stdio.h>

#define TEST_SIZE_GRAPH 20
#define NB_PORT_TEST 8
#define PRIORITY_TEST 1903

void test_net_graph()
{
    struct optional_net_graph op_graph = init_net_graph(TEST_SIZE_GRAPH);

    DEBUG_ASSERT_W_MESSAGE(op_graph.exit_code == SUCESS, "FAIL init graph failed.\n");
    DEBUG_ASSERT_W_MESSAGE(op_graph.graph.nodes != NULL, "FAIL init graph failed because pointer is null.\n");

    struct net_graph graph = op_graph.graph;


    struct optional_device_switch op_sw1 = init_switch(
        init_mac(0xAF, 0x76, 0xFA, 0xEA, 0x09, 0x12),
        NB_PORT_TEST,
        PRIORITY_TEST
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
    DEBUG_ASSERT_W_MESSAGE(graph.cap_devices == TEST_SIZE_GRAPH, "FAIL incorrect capacity of devices.\n");

    DEBUG_ASSERT_W_MESSAGE(net_graph_add_node(&graph, 0, 1, 10) == SUCESS, "FAIL valid node addition failed.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_add_node(&graph, 10, 3, 10) != SUCESS, "FAIL invalid node addition didn't fail.\n");

    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 0, 1).exit_code == SUCESS, "FAIL net_graph_devices_are_connected bad exit code.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 10, 3).exit_code == ERROR_INDEX_OUT_OF_RANGE, "FAIL net_graph_devices_are_connected bad exit code.\n");

    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 0, 1).boolean == true, "FAIL devices should be connected.\n");
    DEBUG_ASSERT_W_MESSAGE(net_graph_devices_are_connected(&graph, 10, 3).boolean == false, "FAIL devices should not be connected.\n");

    deinit_graph(&graph);
    DEBUG_ASSERT_W_MESSAGE(graph.nodes == NULL, "FAIL deallocation failed for net_graph.\n");
}