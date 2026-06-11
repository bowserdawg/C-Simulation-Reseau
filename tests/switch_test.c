#include "switch.h"

#include <string.h>

void test_switch()
{
    struct mac mac = init_mac(0xF1, 0xA2, 0x18, 0x89, 0x5B, 0x09);

    const port_t nb_port = 8;
    const priority_t priority = 1200;


    struct optional_device_switch swtch = init_switch(mac, nb_port, priority);

    // Test pour voire si le switch est bien initalisé.
    DEBUG_ASSERT_W_MESSAGE(swtch.exit_code == SUCCESS, "FAIL Switch initalisation failed\n");

    // test pour voire si le nombre de port est correcte.
    DEBUG_ASSERT_W_MESSAGE(swtch.dev_switch._nbPort == nb_port, "FAIL bad number of ports\n");

    char* mac_str = NULL;
    // Check si l'adresse mac est correcte.
    mac_to_string(&mac_str, swtch.dev_switch._mac);

    DEBUG_ASSERT_W_MESSAGE(strcmp(mac_str, "F1:A2:18:89:5B:09") == 0, "FAIL mac string comparison failed\n");

    free(mac_str);

    struct mac mac2 = init_mac(0x91, 0xAA, 0x18, 0x89, 0x5B, 0x09);
    port_t thePort = 1;

    // Test ajout
    DEBUG_ASSERT_W_MESSAGE(switch_add_table(&swtch.dev_switch, thePort, mac2) == SUCCESS, "FAIL entry couldn't be added\n");
    // Test lookup
    DEBUG_ASSERT_W_MESSAGE(switch_look_table(&swtch.dev_switch, mac2).exit_code == SUCCESS, "FAIL entry should be found in table.\n");
    DEBUG_ASSERT_W_MESSAGE(switch_look_table(&swtch.dev_switch, mac2).port != INVALID_PORT, "FAIL port number should have been valid.\n");
    // Test lookup invalid
    DEBUG_ASSERT_W_MESSAGE(switch_look_table(&swtch.dev_switch, mac).port == INVALID_PORT , "FAIL port number should have been invalid.\n");

    deinit_switch(&swtch.dev_switch);
    // Test deallocation
    DEBUG_ASSERT_W_MESSAGE(swtch.dev_switch.table_commutation.data == NULL, "FAIL deallocation fail\n");
}