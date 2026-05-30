#pragma once

#include <stdbool.h>

enum ERRORS
{
    SUCESS,
    ERR_INVALID_OCTET_PARAM = -255,
    ERR_NULL_FILE,
};

/**
 * Utilise l'enum ERRORS trouvé dans errors.h.
 * 
 * Si il n'ya pas d'erreur la fonction continue normalement.
 * 
 * Return true si tous c'est bien passé.
 */ 
bool error_handler(int error);