#pragma once

#include <stdint.h>

typedef int32_t address_ip_t; 
typedef unsigned char octet_ip_t;

/**
 * "L'adresse IP est attribuée à chaque interface avec le réseau de tout matériel informatique 
 * (routeur, ordinateur, smartphone, objet connecté, 
 * système embarqué, modem (ADSL, wifi, fibre ou câble), imprimante réseau, etc.) 
 * connecté à un réseau utilisant l’Internet Protocol comme protocole de communication 
 * entre ses noeuds.
 * 
 * Pas besoin d'utiliser des pointeurs pour ce struct car 
 * il est vraiment trés petit.
 */
struct ip {
    address_ip_t ip;
};

/**
 * Les octet d'une adresse IP.
 * 
 * OCTET_1 => Bits de poid fort.
 * 
 * OCTET_4 => Bits de poid faible.
 */
enum OCTET_IP
{
    OCTET_1,
    OCTET_2,
    OCTET_3,
    OCTET_4,
    OCTET_MAXIMUM,
};

/**
 * Crée une nouvelle adress ip avec les valeurs donné.
 * 
 * o1 => Bits de poid fort.
 * o4 => Bits de poid faible.
 */
struct ip init_ip(octet_ip_t o1, octet_ip_t o2, octet_ip_t o3, octet_ip_t o4);

/**
 * Transforme une adresse ip en string.
 * 
 * Pas besoin d'allouer de l'espece de string. 
 * Il faut free le pointer string changé par la fonction.
 * 
 * Renvoie le nombre d'octet écrit.
 */
int ip_to_string(char** string, struct ip ip);

/**
 * Donne un octet specifique d'une adresse.
 * 
 * envoie -1 pour un octet invalid.
 * 
 */
int getOctet(struct ip ip, unsigned int octet);