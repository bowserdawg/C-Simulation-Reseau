#include <address_ip.h>
#include "station.h"

void display_station(const struct station *s){
    if (s  == NULL){
        return;
    }


    printf("Station:\n");
    
    printf(" -IP Adress: ");
    display_ip(&s->ip_address);

    printf(" -MAC Adress: ");
    display_mac(&s->mac_address);

   

}

void set_station_ip(struct station *s, address_ip_t ip) {
        if (s == NULL) {
            return;
        }
        s->ip_address.ip = ip;
    }

void set_station_mac(struct station *s, struct mac mac) {
        if (s == NULL) {
            return;
        }
        s->mac_address = mac;
    }