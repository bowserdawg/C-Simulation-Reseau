#include "trame.h"
#include "address_mac.h"

#include <stdio.h>
#include <string.h>

#define ANSI_RESET      "\x1b[0m"
const char *TRAME_LEGENDE_COPIE =
    "\n=== Legende de la trame Ethernet ===\n"
    "\x1b[38;5;240m" "██ Préambule " ANSI_RESET "(7 octets)  — Synchronisation (0xAA x 7)\n"
    "\x1b[38;5;214m" "██ SFD       " ANSI_RESET "(1 octet)   — Start Frame Delimiter (0xAB)\n"
    "\x1b[38;5;196m" "██ MAC dest  " ANSI_RESET "(6 octets)  — Adresse MAC de destination\n"
    "\x1b[38;5;208m" "██ MAC src   " ANSI_RESET "(6 octets)  — Adresse MAC source\n"
    "\x1b[38;5;226m" "██ Type      " ANSI_RESET "(2 octets)  — EtherType / protocole\n"
    "\x1b[38;5;82m"  "██ DATA      " ANSI_RESET "(46+ octets)— Charge utile (bourrage inclus)\n"
    "\x1b[38;5;21m"  "██ FCS       " ANSI_RESET "(4 octets)  — CRC-32 de contrôle d'intégrité\n"
    "=====================================\n";

int test_trame()
{
    uint8_t payload[] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd' };
    struct mac dest = { .mac_array = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF} };
    struct mac src  = { .mac_array = {0xAA,0xBB,0xCC,0xDD,0xEE,0xFF} };

    struct optional_tram tr = init_trame(sizeof(payload), payload, dest, src, 0x0800);

    /* Lecture des champs */
    struct mac d = trame_get_destination(tr.tr);
    struct mac s = trame_get_source(tr.tr);
    protocol_t p = trame_get_protocol(tr.tr);

    // Convertir en string
    char* d_mac_str = NULL;
    DEBUG_ASSERT_W_MESSAGE(mac_to_string(&d_mac_str, d) == SUCESS, "FAIL mac to string operation failed\n");
    char* s_mac_str = NULL;
    DEBUG_ASSERT_W_MESSAGE(mac_to_string(&s_mac_str, s) == SUCESS, "FAIL mac to string operation failed\n");

    /// TODO : Ajouter une fonction "get_protocol_name";
    DEBUG_ASSERT_W_MESSAGE(strcmp(d_mac_str, "FF:FF:FF:FF:FF:FF") == 0, "FAIL dest mac address strings should have been equal\n");
    DEBUG_ASSERT_W_MESSAGE(strcmp(s_mac_str, "AA:BB:CC:DD:EE:FF") == 0, "FAIL source mac address strings should have been equal\n");
    DEBUG_ASSERT_W_MESSAGE(p == 0x0800, "FAIL incorrect protocol\n");
    // Bon nom de protocol
    DEBUG_ASSERT_W_MESSAGE(strcmp(trame_get_protocol_str_name(p), "IPv4") == 0, "FAIL incorrect protocol name\n");
    
    free(d_mac_str);
    free(s_mac_str);
    
    /* Accès en lecture seule au bloc DATA */
    const void *data_ptr = NULL;
    size_t data_len = trame_get_data(tr.tr, &data_ptr);

    // Bourrage corrrecte
    DEBUG_ASSERT_W_MESSAGE(data_len == 46, "ERROR incorrect padding\n");

    // Payload correcte
    const uint8_t* data = (uint8_t*)data_ptr;
    for(size_t i = 0; i < sizeof(payload); ++i)
    {
        //fprintf(stderr, "%c", (char)data[i]); // Affiche "hello world"
        DEBUG_ASSERT_W_MESSAGE(payload[i] == data[i], "ERROR different payload\n");
    }
    //puts("\n");

    /* Vérification de l'intégrité */
    bool ok = trame_is_fcs_valid(tr.tr);
    DEBUG_ASSERT_W_MESSAGE(ok == true, "FAIL incorrect fcs\n");

    char* rep_hex = NULL;
    DEBUG_ASSERT_W_MESSAGE(trame_to_hex(tr.tr, &rep_hex) == SUCESS, "FAIL trame to hex failed\n");

    
    fprintf(stderr, "%s\n",rep_hex);
    free(rep_hex);

    DEBUG_ASSERT_W_MESSAGE(strcmp(trame_get_colored_legend(), TRAME_LEGENDE_COPIE) == 0, "FAIL dest mac address strings should have been equal\n");

    /* La trame colorié */
    char* colored = NULL;
    DEBUG_ASSERT_W_MESSAGE( trame_to_hex_ansi(tr.tr, &colored) == SUCESS, "FAIL trame to hex ANSI failed\n");
    printf("%s\n", colored);
    free(colored); 

    deinit_trame(&tr.tr);

    return 0;
}