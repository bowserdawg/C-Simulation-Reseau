#include "address_ip.h" // Contient address_ip_t et octet_ip_t et OCTET_IP

#include <stdio.h>
#include <stdlib.h>

#define OCTET 0b11111111
#define IP_OCTET_1 0b00000000000000000000000011111111
#define IP_OCTET_2 0b00000000000000001111111100000000
#define IP_OCTET_3 0b00000000111111110000000000000000
#define IP_OCTET_4 0b11111111000000000000000000000000
// Avec le le null terminator.
#define IP_STRING_MAX_SIZE 16

struct ip {
    address_ip_t _ip;
};

enum OCTET_SHIFT
{
    SHIFT_OCTET_1 = 0,
    SHIFT_OCTET_2 = 8,
    SHIFT_OCTET_3 = 16,
    SHIFT_OCTET_4 = 24,
};

// Donne la valeur en octet d'indice SHIFT (utiliser enum OCTET_SHIFT) dans ADRESS
#define _getOctet(ADRESS, SHIFT) (ADRESS >> SHIFT) & 0b11111111

// Encode la valeur VAL dans ADRESS dans le position SHIFT (utiliser enum OCTET_SHIFT)
#define encodeOctet(ADRESS, VAL, SHIFT) ADRESS += (VAL << SHIFT)

/**
 * Crée une nouvelle adress ip avec les valeurs donné.
 * o1 => plus grand octet
 * o4 => plus petit octet
 */
struct ip* init_ip(octet_ip_t o1, octet_ip_t o2, octet_ip_t o3, octet_ip_t o4)
{
    address_ip_t ip_int = 0;
    encodeOctet(ip_int, o1, SHIFT_OCTET_1);
    encodeOctet(ip_int, o2, SHIFT_OCTET_2);
    encodeOctet(ip_int, o3, SHIFT_OCTET_3);
    encodeOctet(ip_int, o4, SHIFT_OCTET_4);

    struct ip* ip = calloc(1, sizeof(struct ip));
    ip->_ip = ip_int;
    return ip;
}

void deinit_ip(struct ip** ip)
{
    free(*ip);
}

/**
 * Transforme une adresse ip en string.
 * 
 * Pas besoin d'allouer de l'espece de string. 
 * Il faut free le pointer string changé par la fonction.
 * 
 * Renvoie le nombre d'octet écrit.
 */
int ip_to_string(char** string, struct ip* ip)
{
    int val = ip->_ip;

    // Les octets
    octet_ip_t o1 = _getOctet(val, SHIFT_OCTET_1);
    octet_ip_t o2 = _getOctet(val, SHIFT_OCTET_2); 
    octet_ip_t o3 = _getOctet(val, SHIFT_OCTET_3);
    octet_ip_t o4 = _getOctet(val, SHIFT_OCTET_4);

    // Calloc renvoie avec la mémoire à 0.
    *string = calloc(IP_STRING_MAX_SIZE, sizeof(char));

    // Ecrit sur le buffer l'adress ip.
    return snprintf(*string, IP_STRING_MAX_SIZE, "%d.%d.%d.%d", o1, o2, o3, o4);
    // Vu qu'on à utilisé calloc, le terminateur null existe deja! C'est pratique.
}


/**
 * Donne un octet specifique d'une adresse.
 * 
 * Utilise la notation big endian.
 */
int getOctet(struct ip* ip, int nbOctet)
{
    return 1;
}