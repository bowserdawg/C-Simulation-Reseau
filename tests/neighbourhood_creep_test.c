#include "errors.h"
#include "address_mac.h"
#include "neighbourhood_creep.h"
#include "trame.h"

#include <stdio.h>


const char* DATA = "Hello world";

void test_neighbourhood_creep()
{
    const int len = 5;

    struct neighbourhood_ptr_optional op = init_neighbourhood(len);
    DEBUG_ASSERT_W_MESSAGE(op.exit_code == SUCESS, "FAIL init_neighbourhood failed\n");

    struct neighbourhood* nei = op.n_ptr;

    struct mac mac1 = init_mac(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    struct mac mac2 = init_mac(0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF);


    DEBUG_ASSERT_W_MESSAGE(neighbourhood_get_len(nei) == 0, "FAIL wrong neighbourhood length\n");

    struct optional_tram myTrame_op = init_trame(sizeof(DATA), DATA, mac1, mac2, 0x0800);
    DEBUG_ASSERT_W_MESSAGE(myTrame_op.exit_code == SUCESS,"FAIL init trame failed\n");
    struct trame myTrame = myTrame_op.tr;

    struct optional_tram myTrame2_op = init_trame(sizeof(DATA), DATA, mac2, mac1, 0x0800);
    DEBUG_ASSERT_W_MESSAGE(myTrame2_op.exit_code == SUCESS,"FAIL init trame failed\n");
    struct trame myTrame2 = myTrame2_op.tr;


    neighbourhood_push_trame(nei, &myTrame);  // 0
    neighbourhood_push_trame(nei, &myTrame2);

    DEBUG_ASSERT_W_MESSAGE(neighbourhood_get_len(nei) == 2, "FAIL wrong neighbourhood length\n");


    DEBUG_ASSERT_W_MESSAGE(neighbourhood_get_len(nei) == 2, "FAIL wrong neighbourhood length\n");
    DEBUG_ASSERT_W_MESSAGE(neighbourhood_pop_trame(nei, 0) == SUCESS, "FAIL neighbourhood_pop_trame should have been successful\n");
    DEBUG_ASSERT_W_MESSAGE(neighbourhood_pop_trame(nei, 2) != SUCESS, "FAIL neighbourhood_pop_trame shouldn't have been successful\n");

    struct optional_tram myTrame3_op = init_trame(sizeof(DATA), DATA, mac1, mac2, 0x0800);
    struct trame myTrame3 = myTrame3_op.tr;

    neighbourhood_push_trame(nei, &myTrame3);

    DEBUG_ASSERT_W_MESSAGE(neighbourhood_get_len(nei) == 2, "FAIL len shouldn't have changed\n");
    DEBUG_ASSERT_W_MESSAGE(neighbourhood_is_present(nei, 0) == true, "FAIL should be present because popped but added back\n");


    DEBUG_ASSERT_W_MESSAGE(neighbourhood_pop_trame(nei, 1) == SUCESS, "FAIL neighbourhood_pop_trame should have been successful\n");
    DEBUG_ASSERT_W_MESSAGE(neighbourhood_is_present(nei, 1) == false, "FAIL should not be present because popped\n");

    DEBUG_ASSERT_W_MESSAGE(neighbourhood_is_present(nei, 2) == false, "FAIL shoudln't be present\n");

    struct mac mac_mac2_copie = trame_get_source(neighbourhood_get_trame(nei, 0).tr);

    DEBUG_ASSERT_W_MESSAGE(mac_equals(mac_mac2_copie, mac2), "FAIL Mac should have been the same\n");

    deinit_neighbourhood(nei);
}