#pragma once

#include "address_ip.h"
#include "address_mac.h"

struct device_station {
    struct ip ip;
    struct mac mac;
};  

/**
 * Pas besoin de cette fonction en vrai mais on va l'ajouter pour rester cohérente. 
 */
struct device_station init_station(struct ip ip, struct mac mac);