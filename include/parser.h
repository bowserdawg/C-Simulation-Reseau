#pragma once
#include "address_mac.h"
#include "address_ip.h"


enum EXIT_CODES init_net_from_file(const char *filename);
void display_ip(struct ip *);