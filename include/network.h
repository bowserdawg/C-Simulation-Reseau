#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef uint32_t ip;

struct mac {
  uint8_t mac_array[6];
};

struct network {
  size_t nb_switch;
  size_t nb_station;
};