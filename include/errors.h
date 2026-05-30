#pragma once

#include <stdbool.h>

/**
 * Attention : tous les fonctions ne peuvent pas utliser ce enum.
 * 
 * Si une fonction peut renvoyer un entier negatif
 * utilisable normalement il ne devrait
 * pas utiliser ce systeme, il devrait utiliser un optional à la place
 * ou renvoyer -1;
 */
enum EXIT_CODES
{
    SUCESS,
    ERR_INVALID_OCTET_PARAM = -255,
    ERR_NULL_FILE,
    ERROR_INVALID_STRING_FORMAT,
    ERROR_INVALID_strtol,
    ERROR_NULL_STRING_PARAM,
};

/**
 * Affiche un message et return false s'il ya une erreur.
 */
bool error_handler(enum EXIT_CODES exit_code);

/**
 * Affiche un message et crash le programme si le code de sortie donné donne une erreur.
 */
void error_handler_crash(enum EXIT_CODES exit_code);