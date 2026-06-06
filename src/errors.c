#include "errors.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Affiche un message d'erreur sur le terminal pour une erreur donné en paramètre.
 * Si il n'y a pas d'erreur, "Unspecified error code : [code]" est affiché.
 * Si une valeur de (enum EXIT_CODES)SUCESS est donné, la procédure affiche "Success".
 * 
 * Paramètres:
 *      - error : Le type d'erreur, trouvé dans l'enum "enum EXIT_CODES" dans "errors.h".
 * 
 * Valeur de retour: Ne renvoie rien.
 */
static void error_printer(enum EXIT_CODES error)
{
    switch (error)
    {
    case SUCESS:
        fprintf(stderr, "Success\n");
        break;
    case ERR_INVALID_OCTET_PARAM:
        fprintf(stderr, "A function was given an unreachable or invalid parameter for a bitwise operation\n");
        break;
    
    case ERR_NULL_FILE:
        perror("A file couldn't be open with a system function");
        break;
    
    case ERROR_BAD_ALLOC:
        perror("Memory allocation failed");
        break;
    
    case ERROR_INVALID_STRING_FORMAT:
        fprintf(stderr, "The given string format was invalid for the function\n");
        break;

    case ERROR_INVALID_strtoul:
        perror("Error in strtoul");
        break;

    case ERROR_NULL_STRING_PARAM:
        fprintf(stderr, "A string given in fonction argument was invalid\n");
        break;

    default:
        fprintf(stderr, "Unspecified error code : %d\n", error);
        break;
    }
}

/**
 * Fonction d'aide qui printe la location d'une erreur.
 * 
 * Paramètres:
 *      - file : Le nom du fichier source ou l'erreur est arrivée.
 *      - func : Le nom de la fonction ou le l'erreur est arrivée.
 *      - line : La ligne pu l'erreur est arrivé.
 * 
 * Valeur de retour: Ne renvoie rien.
 */
static void error_show_error_location(const char* file, const char* func, int line)
{
    fprintf(stderr, "\033[31m[ERROR]\033[0m [%s]::[%s]::[line %d] : ", file, func, line);
}

/**
 * Fonction d'aide qui printe tous la location d'un "warning".
 * 
 * Paramètres:
 *      - file : Le nom du fichier source ou l'erreur est arrivée.
 *      - func : Le nom de la fonction ou le l'erreur est arrivée.
 *      - line : La ligne pu l'erreur est arrivé.
 * 
 * Valeur de retour: Ne renvoie rien.
 */
static void error_show_warning_location(const char* file, const char* func, int line)
{
    fprintf(stderr, "\033[35m[WARNING]\033[0m [%s]::[%s]::[line %d] : ", file, func, line);
}

/**
 * Renvoie un booleen false s'il y'a une erreur et 
 * print l'endroit ou l'erreur est arriver.
 */
bool error_diagnostic(enum EXIT_CODES exit_code, 
    const char* file, const char* func, int line)
{
    if(exit_code != SUCESS)
    {
        error_show_error_location(file, func, line);
        error_printer(exit_code);
        return false;
    }
    return true;
}


/**
 * Crash s'il y'a une erreur et print l'endroit ou l'erreur est arriver
 */
void error_diagnostic_abort(enum EXIT_CODES exit_code, 
    const char* file, const char* func, int line)
{
    if(exit_code != SUCESS)
    {
        error_show_error_location(file, func, line);
        error_printer(exit_code);
        abort();
    }
}

/**
 * Renvoie un booleen false s'il y'a un warning et 
 * print l'endroit ou le warning est arriver.
 */
bool error_warning_diagnostic(enum EXIT_CODES exit_code, 
    const char* file, const char* func, int line)
{
    if(exit_code == SUCESS)
    {
        return true;
    }

    error_show_warning_location(file, func, line);
    error_printer(exit_code);

    return false;
}

/**
 * Si l'expression boolean donné est fausse, affiche un message donné et fait un assert faux.
 */
void error_assert_with_message(bool expression, const char* message,
    const char* file, const char* func, int line)
{
    if(!expression)
    {
        error_show_error_location(file, func, line);
        fputs(message, stderr);
        assert(false);
    }
}