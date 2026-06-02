#pragma once

#include <stdint.h>
#include <stdio.h>


#include "network.h"
#include "address_ip.h"

/* Structure pour représenter une station */
struct station {
    struct mac mac_address; 
    struct ip ip_address;};

    /* affiche  les informations d'une station */
    /*  *s pointeur vers la station a afficher  */
    void display_station(const struct station *s);
