#pragma once

#include "address_ip.h"
#include "address_mac.h"

#include <stdint.h>
#include <stdio.h>

struct station {
    struct ip ip;
    struct mac mac;
};  

/**
 * Pas besoin de cette fonction en vrai mais on va l'ajouter pour rester cohérente. 
 */
struct station init_station(struct ip ip, struct mac mac);