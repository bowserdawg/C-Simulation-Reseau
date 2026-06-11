#include "parser.h"
#include "net_graph.h"
#include "neighbourhood_creep.h"
#include "networking_logic.h"
#include "errors.h"

#include <stdio.h>

static size_t count_active(struct neighbourhood* n)
{
    size_t count = 0;
    size_t len = neighbourhood_get_len(n);
    for (size_t i = 0; i < len; ++i)
        if (neighbourhood_is_present(n, i))
            count++;
    return count;
}

void test_simulation(void)
{
    // --- my_lan_no_circle: full-tree flood must terminate (no broadcast storm) ---
    //
    // Send from station 7 to station 14.  Their MACs are unknown to all switches,
    // so every switch will flood in all directions.  In a tree, each copy moves
    // strictly away from its source and can never loop back, so all frames must
    // be delivered and the neighbourhood must eventually empty.
    //
    // Longest path in this tree: ~6 hops × cost 4 = 24 ticks.
    // 100 ticks gives ample headroom.

    struct net_graph graph = {0};
    DEBUG_ASSERT_W_MESSAGE(
        parse_config("./data/configs/my_lan_no_circle", &graph) == SUCCESS,
        "FAIL: could not parse my_lan_no_circle\n");

    struct neighbourhood_ptr_optional nei_op = init_neighbourhood(graph.nb_devices * 4);
    DEBUG_ASSERT_W_MESSAGE(nei_op.exit_code == SUCCESS,
        "FAIL: init_neighbourhood failed\n");
    struct neighbourhood* nei = nei_op.n_ptr;

    user_queues_packet(&graph, nei, 7, 14, "storm_test", 10);

    size_t max_active = 0;
    for (int tick = 0; tick < 100; ++tick)
    {
        size_t active = count_active(nei);
        if (active > max_active) max_active = active;
        network_handle_tick(&graph, nei);
    }

    size_t remaining = count_active(nei);
    fprintf(stderr, "    max concurrent frames in flight : %zu\n", max_active);
    fprintf(stderr, "    frames still active after 100 ticks : %zu\n", remaining);

    DEBUG_ASSERT_W_MESSAGE(remaining == 0,
        "FAIL: broadcast storm detected — frames still circulating after 100 ticks\n");

    deinit_neighbourhood(nei);
    deinit_graph(&graph);
}
