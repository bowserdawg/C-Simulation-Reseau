#pragma once

#include <stdbool.h>

/**
 * Les codes de sorties d'erreur ne peuvent que etre negative.
 * 
 * Attention : tous les fonctions ne peuvent pas utliser ce enum.
 * 
 * Si une fonction DOIT renvoyer un entier utilisable normalement il ne devrait
 * pas utiliser ce systeme, il devrait utiliser un optional à la place ou renvoyer -1;
 */
enum EXIT_CODES
{
    SUCESS,
    ERR_INVALID_OCTET_PARAM = -255,
    ERR_NULL_FILE,
    ERROR_BAD_ALLOC,
    ERROR_INVALID_STRING_FORMAT,
    ERROR_INVALID_strtoul,
    ERROR_NULL_STRING_PARAM,
    ERROR_INVALID_TERMINAL_ARGS,
    ERROR_NULL_POINTER_PARAM,
    ERROR_INVALID_PORT_PARAM,
    ERROR_CAPACITY_REACHED,
    ERROR_PARSER_NO_FILE,
    ERROR_PARSER_HEADER,
    ERROR_PARSING_FORMAT_DEVICES,
    ERROR_PARSING_FORMAT_NODE,
    ERROR_PARSER_UNKNOWN_CONFIG,
    ERROR_INDEX_OUT_OF_RANGE,
    ERROR_NODE_SEMANTIC_ERROR_SAME_INDEX,
    WARNING_ITEM_NOT_FOUND,
    WARNING_ITEM_NULL_FREEING,

};

/**
 * Crash s'il y'a une erreur et 
 * print l'endroit ou l'erreur est arriver
 */
void error_diagnostic_abort(enum EXIT_CODES exit_code, 
    const char* file, const char* func, int line);

/**
 * Montre un message de warning si le code de sortie est négative
 */
void error_warning_diagnostic(enum EXIT_CODES exit_code, 
    const char* file, const char* func, int line);

/**
 * Si l'expression boolean donné est fausse, affiche un message donné et fait un assert faux.
 */
void error_assert_with_message(bool expression, const char* message,
    const char* file, const char* func, int line);

#ifdef DEBUG
    #define DEBUG_WARNING(ERR_CODE) error_warning_diagnostic(ERR_CODE, __FILE__, __func__, __LINE__)
    #define DEBUG_ABORT_IF_ERREUR(ERR_CODE) error_diagnostic_abort(ERR_CODE, __FILE__, __func__, __LINE__)
    #define DEBUG_ASSERT_W_MESSAGE(EXPR, MESSAGE) error_assert_with_message(EXPR, MESSAGE, __FILE__, __func__, __LINE__)
#else
    #define DEBUG_WARNING(ERR_CODE) ((void)0)
    #define DEBUG_ABORT_IF_ERREUR(ERR_CODE) ((void)0)
    #define DEBUG_ASSERT_W_MESSAGE(EXPR, MESSAGE) ((void)0)
#endif
