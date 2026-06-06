#include "address_ip.h" // Contient address_ip_t et octet_ip_t et OCTET_IP
#include "errors.h"     // Pour les messages d'erreur
#include "address_mac.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>  // Pour strtok

#define OCTET 0b11111111
#define IP_OCTET_1 0b00000000000000000000000011111111
#define IP_OCTET_2 0b00000000000000001111111100000000
#define IP_OCTET_3 0b00000000111111110000000000000000
#define IP_OCTET_4 0b11111111000000000000000000000000
// Avec le le null terminator.
#define IP_STRING_MAX_SIZE 16

#define MAX_OCTET_VALUE 255

#define OCTET_ASCII_MAX_SIZE 3

/**
 * Si on veut obtenir la valeur d'un seul octet spécifique il faut faire un shift.
 */
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
struct ip init_ip(octet_ip_t o1, octet_ip_t o2, octet_ip_t o3, octet_ip_t o4)
{
    address_ip_t ip_int = 0;

    encodeOctet(ip_int, o1, SHIFT_OCTET_1);
    encodeOctet(ip_int, o2, SHIFT_OCTET_2);
    encodeOctet(ip_int, o3, SHIFT_OCTET_3);
    encodeOctet(ip_int, o4, SHIFT_OCTET_4);

    return (struct ip){ip_int};
}

/**
 * Donne un octet specifique d'une adresse.
 * 
 * Renvoie un optional IP avec la valeur le code d'erreur de sortie.
 */
struct optional_octet ip_get_octet(struct ip ip, enum OCTET_IP octet)
{
    if(octet >= OCTET_MAXIMUM)
    {
        DEBUG_WARNING(ERR_INVALID_OCTET_PARAM);
        return (struct optional_octet) {
            0, 
            ERR_INVALID_OCTET_PARAM
        };
    }
    

    return (struct optional_octet) {
        _getOctet(ip.ip, octet * 8), 
        SUCESS
    };
}

/**
 * Transforme une adresse ip en string.
 * 
 * Pas besoin d'allouer de l'espece de string. 
 * Il faut free le pointer string donné par la fonction.
 * 
 * Renvoie le nombre d'octet écrit. Si le nombre est négatif, il y'a un erreur qu'il faut checker.
 */
int ip_to_string(char** string, struct ip ip)
{
    int val = ip.ip;

    // Les octets
    octet_ip_t o1 = _getOctet(val, SHIFT_OCTET_1);
    octet_ip_t o2 = _getOctet(val, SHIFT_OCTET_2); 
    octet_ip_t o3 = _getOctet(val, SHIFT_OCTET_3);
    octet_ip_t o4 = _getOctet(val, SHIFT_OCTET_4);

    // Calloc renvoie avec la mémoire à 0.
    *string = calloc(IP_STRING_MAX_SIZE, sizeof(char));
    if(!string)
    {
        DEBUG_WARNING(ERROR_BAD_ALLOC);
        return ERROR_BAD_ALLOC;
    }

    // Ecrit sur le buffer l'adress ip.
    // Vu qu'on à utilisé calloc, le terminateur null existe deja! C'est pratique.
    return snprintf(*string, IP_STRING_MAX_SIZE, "%d.%d.%d.%d", o1, o2, o3, o4);
}


/**
 * Donne l'adresse ip avec l'aide d'une chaine terminé
 * par une caractére de fin de chaine.
 * 
 * La chaine de l'ip doit etre delimité par des points comme ceci:
 *      12.78.12.17
 * 
 * Utiliser deux points de suite comme ceci : '1..1' n'est pas valide, 
 * pour representer un zero écriver comme cela : 1.0.1 
 * 
 * Vous pouvez choisir la base.
 */
struct optional_ip ip_from_string(const char* ip_string)
{
    enum EXIT_CODES ex_code = SUCESS;

    if(ip_string == NULL)
    {
        DEBUG_WARNING(ERROR_NULL_STRING_PARAM);
        ex_code = ERROR_NULL_STRING_PARAM;
        goto end;
    }

    // Il faut copier la chaine pour faire des token
    char buf[IP_STRING_MAX_SIZE];
    strncpy(buf, ip_string, sizeof(buf));

    // Transforme la chaine en token pour parcourir els différents octets.
    char *token = strtok(buf, ".");

    // L'adresse ip en 32 bit.
    octet_ip_t results[4] = {0};  

    // Le nombre de token.
    int count = 0;
    while (token) 
    {
        // Si le nombre de token est plus que 3 et on est encore
        // dans la boucle, il y'a une erreur.
        if (count > OCTET_ASCII_MAX_SIZE)
        {
            DEBUG_WARNING(ERROR_INVALID_STRING_FORMAT);
            ex_code = ERROR_INVALID_STRING_FORMAT;
            goto end;
        }

        // Convertie en long non signé.
        unsigned long octet = strtoul(token, NULL, 10);
        // Si c'est beaucoup trop grand, il y'a une erreur.
        if(octet > MAX_OCTET_VALUE)
        {
            DEBUG_WARNING(ERROR_INVALID_STRING_FORMAT);
            ex_code = ERROR_INVALID_STRING_FORMAT;
            goto end;
        }

        results[count] = (octet & 0xFF);

        // Prendre le prochain token.
        token = strtok(NULL, ".");
        count++;
    }

    // A la fin, il doit y'avoir 4 iteration en tout si c'est une adresse ip.
    if(count != 4)
    {
        DEBUG_WARNING(ERROR_INVALID_STRING_FORMAT);
        ex_code = ERROR_INVALID_STRING_FORMAT;
    }

    end:
    return (struct optional_ip) {
        .ip = init_ip(results[0],results[1],results[2],results[3]), 
        .exit_code = ex_code        // On pourait mettre l'ip à 0 mais c'est inutile
    };                              // car 0.0.0.0 est une adresse valide + l'utilisateur             
                                    // devrait check la valeur de l'optional en premier.
}