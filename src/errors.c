#include "errors.h"

#include <stdbool.h>
#include <stdio.h>

bool error_handler(int error)
{
    if(error == SUCESS) return true;

    // Handle the error
    switch (error)
    {
    case ERR_INVALID_OCTET_PARAM:
        fprintf(stderr, "A function was given an unreachable parameter or invalid parameter for a bitwise operation.\n");
        break;
    
    case ERR_NULL_FILE:
        perror("A file couldn't be open with a system function. : \n");
        break;
    }

    return false;
}