#pragma once

#include "address_ip.h"
#include "address_mac.h"
#include "stdlib.h"

typedef size_t port_t;
typedef uint16_t priority_t;

struct device_switch {
    struct mac _mac;
    port_t _nbPort;
};  

struct optional_port {
    port_t port;
    enum EXIT_CODES exit_code;
};