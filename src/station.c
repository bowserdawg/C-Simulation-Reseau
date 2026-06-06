#include "address_ip.h"
#include "address_mac.h"
#include "station.h"

/**
 * Pas besoin de cette fonction en vrai mais on va l'ajouter pour rester cohérente. 
 */
struct station init_station(struct ip ip, struct mac mac)
{
    return (struct station){ip, mac};
}