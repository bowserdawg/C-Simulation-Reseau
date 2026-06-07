#include "net_graph.h"
#include "errors.h"

#define TEST_SIZE_GRAPH 20


void test_net_graph()
{
    struct optional_net_graph op_graph = init_net_graph(20);

    DEBUG_ASSERT_W_MESSAGE(op_graph.exit_code == SUCESS, "FAIL init graph failed.\n");
    DEBUG_ASSERT_W_MESSAGE(op_graph.graph.nodes != NULL, "FAIL init graph failed because pointer is null.\n");

    struct net_graph graph = op_graph.graph;

    deinit_graph(&graph);
    DEBUG_ASSERT_W_MESSAGE(graph.nodes == NULL, "FAIL deallocation failed for net_graph.\n");
}