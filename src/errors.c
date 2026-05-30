#include "errors.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void error_printer(enum EXIT_CODES error)
{
    // Handle the error
    switch (error)
    {
    case ERR_INVALID_OCTET_PARAM:
        fprintf(stderr, "A function was given an unreachable parameter or invalid parameter for a bitwise operation.\n");
        break;
    
    case ERR_NULL_FILE:
        perror("A file couldn't be open with a system function. : ");
        break;
    
    case ERROR_INVALID_STRING_FORMAT:
        fprintf(stderr, "The given string format was invalid for the function.\n");
        break;

    case ERROR_INVALID_strtol:
        perror("Error in strtol : ");
        break;
    case ERROR_NULL_STRING_PARAM:
        fprintf(stderr, "A string given in fonction argument was invalid.\n");
        break;

    default:
        break;
    }
}

void error_handler_crash(enum EXIT_CODES exit_code)
{
    if(exit_code != SUCESS)
    {
        error_printer(exit_code);
        abort();
    }
}

bool error_handler(enum EXIT_CODES exit_code)
{
    if(exit_code == SUCESS)
        return true;
    error_printer(exit_code);
    return false;
}