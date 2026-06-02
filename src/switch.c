#include "../include/switch.h"

void set_switch_mac(struct switch_d *switch_d, struct mac mac) {
  switch_d->mac_adress = mac;
}

void set_switch_ports(struct switch_d *switch_d, size_t ports) {
  switch_d->nb_ports = ports;
}
void set_switch_priority(struct switch_d *switch_d, size_t priority) {
  switch_d->priority = priority;
}