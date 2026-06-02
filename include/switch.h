#pragma once

#include "network.h"
#include "station.h"
#include <stdint.h>
#include <stdlib.h>

struct switch_d {
  struct mac mac_adress;
  size_t nb_ports;
  size_t priority;
  // table de commutation à ajouter
};

void set_switch_mac(struct switch_d *, struct mac);
void set_switch_priority(struct switch_d *, size_t);
void set_switch_ports(struct switch_d *, size_t);