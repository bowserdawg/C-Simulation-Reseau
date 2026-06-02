#pragma once

#include <stdint.h>
#include <stdio.h>

/* Type pour l adresse MAC sur 6 octets */
typedef struct {
    uint8_t bytes[6];
    } mac_address_t;
    
/* Type  pour l adresse IPv4  sur 4 octets */
typedef struct {
    uint8_t ssid[4];
    }ip_address_t;

    /* Structure pour représenter une station */
typedef struct {    
    mac_address_t mac;
    ip_address_t ip;
    } station_t;

/* Fonction pour afficher une adresse MAC */
void print_mac_address(mac_address_t mac) ;

/* Fonction pour afficher une adresse IP */
void print_ip_address(ip_address_t ip) ;


    
    