#include "switch.h"

#include <string.h>

void test_switch()
{
    struct ip ip = init_ip(192, 20, 12, 92);
    struct mac mac = init_mac(0xF1, 0xA2, 0x18, 0x89, 0x5B, 0x09);

    const int nb_port = 8;

    struct optional_device_switch swtch = init_switch(mac, nb_port);

    // Test pour voire si le switch est bien initalisé.
    DEBUG_ASSERT_W_MESSAGE(swtch.exit_code == SUCESS, "FAIL Switch initalisation failed\n");

    // test pour voire si le nombre de port est correcte.
    DEBUG_ASSERT_W_MESSAGE(swtch.dev_switch._nbPort == nb_port, "FAIL bad number of ports\n");

    deinit_switch(&swtch.dev_switch);

    char* mac_str = NULL;
    // Check si l'adresse mac est correcte.
    mac_to_string(&mac_str, swtch.dev_switch._mac);

    DEBUG_ASSERT_W_MESSAGE(strcmp(mac_str, "F1:A2:18:89:5B:09") == 0, "FAIL mac string comparison failed\n");

    free(mac_str);

    DEBUG_ASSERT_W_MESSAGE(swtch.dev_switch.table_commutation.data == NULL, "FAIL deallocation fail\n");
}