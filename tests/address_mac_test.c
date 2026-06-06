#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "errors.h"
#include "address_mac.h"
#include "string.h"

void test_mac()
{
    struct mac mac1 = init_mac(192, 168, 187, 123, 16, 89);
    struct optional_mac op_mac = mac_from_str("C0:A8:BB:7B:10:59");
    // Test si mac_from_str pour savoir si aucune erreurs se produisent.
    DEBUG_WARNING(op_mac.exit_code);
    DEBUG_ASSERT_W_MESSAGE(op_mac.exit_code == SUCESS, "FAIL mac_from_str\n");

    // Test si les deux adresses mac obtenu avec deux fonctions différents sont les memes
    for(size_t i = 0; i < MAC_NB_OCTET; ++i)
    {
        DEBUG_ASSERT_W_MESSAGE(mac1.mac_array[i] == op_mac.mac.mac_array[i], "FAIL Both mac addresses given should have been similar.\n");
    }

    char* string1 = NULL;
    enum EXIT_CODES ex = mac_to_string(&string1, mac1);
    // Test si une adresse mac peut etre correctment convertie en chaine de caractere.
    DEBUG_WARNING(ex);
    DEBUG_ASSERT_W_MESSAGE(ex == SUCESS, "FAIL mac_to_string error code\n");

    char* string2 = NULL;
    ex = mac_to_string(&string2, op_mac.mac);
    // Test si les deux string obtenu avec mac_to_string sont exactement le meme.
    DEBUG_ASSERT_W_MESSAGE(strcmp(string1, string2) == 0, "FAIL both mac strings aren't similar\n");

    struct optional_mac op1 = mac_from_str(string1);
    struct optional_mac op2 = mac_from_str(string2);
    // Test si la fonction mac_from_str fonctionne correctement sans erreur
    DEBUG_ASSERT_W_MESSAGE(op1.exit_code == SUCESS && op2.exit_code == SUCESS, "FAIL Unsuccessful mac_from_str.\n");
    
    // Test si les deux adresses mac obtenu avec la meme fonction (mac_from_str) sont les memes
    for(size_t i = 0; i < MAC_NB_OCTET; ++i)
    {
        DEBUG_ASSERT_W_MESSAGE(op1.mac.mac_array[i] == op2.mac.mac_array[i], "FAIL Both mac addresses given by mac_from_str should have been similar.\n");
    }

    // Test erreur de formattage
    struct optional_mac op_mac_fin = mac_from_str("C00:A8:BB:7B:10:59");
    DEBUG_ASSERT_W_MESSAGE(op_mac_fin.exit_code != SUCESS, "FAIL mac_from_str should have given an error\n");
    op_mac_fin = mac_from_str("C0A8:BB:7B:10:59");
    DEBUG_ASSERT_W_MESSAGE(op_mac_fin.exit_code != SUCESS, "FAIL mac_from_str should have given an error\n");
    op_mac_fin = mac_from_str("C0:BB:7BI:10:59");
    DEBUG_ASSERT_W_MESSAGE(op_mac_fin.exit_code != SUCESS, "FAIL mac_from_str should have given an error\n");
    op_mac_fin = mac_from_str("C0:BB:7B:1001:59");
    DEBUG_ASSERT_W_MESSAGE(op_mac_fin.exit_code != SUCESS, "FAIL mac_from_str should have given an error\n");
    op_mac_fin = mac_from_str("C0:BB:7I:10:5JU9");
    DEBUG_ASSERT_W_MESSAGE(op_mac_fin.exit_code != SUCESS, "FAIL mac_from_str should have given an error\n");

    op_mac_fin = mac_from_str(NULL);
    // Tester si les arguments null peuvent etre géré 
    DEBUG_ASSERT_W_MESSAGE(op_mac_fin.exit_code == ERROR_NULL_STRING_PARAM, "FAIL a null string parameter should give an error\n");

    free(string1);
    free(string2);
}