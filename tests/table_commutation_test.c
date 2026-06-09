#include "table_commutation.h"
#include "switch.h"

#include <stdio.h>

void test_table_commutation()
{
    struct optional_table_commutation op_tc = init_table_commutation(100);
    DEBUG_WARNING(op_tc.exit_code);
    DEBUG_ASSERT_W_MESSAGE(op_tc.exit_code == SUCESS, "FAIL init_table_commutation had an error\n");

    struct table_commutation table = op_tc.table;

    struct mac mac1 = init_mac(12, 18, 17, 172, 172, 75);
    struct mac mac2 = init_mac(189, 12, 18, 67, 172, 75);
    struct mac mac3 = init_mac(75, 19, 172, 17, 182, 12);

    // Test add
    table_commutation_add(&table, 0, mac1);
    table_commutation_add(&table, 1, mac2);
    table_commutation_add(&table, 2, mac3);

    // Test lookup pour lookup valide
    struct optional_port op1 = table_commutation_lookup(table, mac1);
    struct optional_port op2 = table_commutation_lookup(table, mac2);
    struct optional_port op3 = table_commutation_lookup(table, mac3);

    DEBUG_ASSERT_W_MESSAGE(op1.port != INVALID_PORT, "FAIL port number must be avaible.\n");
    DEBUG_ASSERT_W_MESSAGE(op2.port != INVALID_PORT, "FAIL port number must be avaible.\n");
    DEBUG_ASSERT_W_MESSAGE(op3.port != INVALID_PORT, "FAIL port number must be avaible.\n");

    // Test lookup pour lookup invalide
    op1 = table_commutation_lookup(table, init_mac(75, 19, 170, 17, 182, 12));
    op2 = table_commutation_lookup(table, init_mac(75, 19, 172, 17, 182, 11));
    op3 = table_commutation_lookup(table, init_mac(75, 19, 172, 17, 182, 1));

    DEBUG_ASSERT_W_MESSAGE(op1.port == INVALID_PORT, "FAIL port number must not be avaible.\n");
    DEBUG_ASSERT_W_MESSAGE(op2.port == INVALID_PORT, "FAIL port number must not be avaible.\n");
    DEBUG_ASSERT_W_MESSAGE(op3.port == INVALID_PORT, "FAIL port number must not be avaible.\n");

    deinit_table_commutation(&op_tc.table);
}