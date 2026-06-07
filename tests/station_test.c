#include "station.h"
#include "address_ip.h"
#include "address_mac.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void test_station()
{
    struct ip ip = init_ip(192, 20, 12, 92);
    struct mac mac = init_mac(0xF1, 0xA2, 0x18, 0x89, 0x5B, 0x09);

    struct device_station station = init_station(ip, mac); 

    char* ip_str = NULL;
    ip_to_string(&ip_str, station.ip);
    char* mac_str = NULL;
    mac_to_string(&mac_str, station.mac);

    DEBUG_ASSERT_W_MESSAGE(strcmp(ip_str, "192.20.12.92") == 0, "FAIL ip string comparison failed\n");
    DEBUG_ASSERT_W_MESSAGE(strcmp(mac_str, "F1:A2:18:89:5B:09") == 0, "FAIL mac string comparison failed\n");

    free(ip_str);
    free(mac_str);
}