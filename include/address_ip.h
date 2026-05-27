#include <stdint.h>

struct ip;

typedef int32_t address_ip_t; 
typedef unsigned char octet_ip_t;

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
 * o1 => plus grand octet
 * o4 => plus petit octet
 */
struct ip* init_ip(octet_ip_t o1, octet_ip_t o2, octet_ip_t o3, octet_ip_t o4);

void deinit_ip(struct ip** ip);

/**
 * Transforme une adresse ip en string.
 * 
 * Pas besoin d'allouer de l'espece de string. 
 * Il faut free le pointer string changé par la fonction.
 * 
 * Renvoie le nombre d'octet écrit.
 */
int ip_to_string(char** string, struct ip* ip);