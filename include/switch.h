#pragma once

#include "address_ip.h"
#include "address_mac.h"

#include <stdlib.h>

#include "table_commutation.h"

struct device_switch {
    struct table_commutation table_commutation;
    struct mac _mac;
    port_t _nbPort;
};  

struct optional_device_switch {
    struct device_switch dev_switch;
    enum EXIT_CODES exit_code;
};

struct optional_port {
    port_t port;
    enum EXIT_CODES exit_code;
};

/**
 * Initialise un nouveau switch. 
 */
struct optional_device_switch init_switch(struct mac mac, port_t nbPort);