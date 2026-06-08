#include "trame.h"
#include "address_mac.h"

int test_trame()
{
    uint8_t payload[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    struct mac dest = { .mac_array = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF} };
    struct mac src  = { .mac_array = {0xAA,0xBB,0xCC,0xDD,0xEE,0xFF} };

    struct trame tr = init_trame(sizeof(payload), payload, dest, src, 0x0800);

    /* Lecture des champs */
    struct mac d = trame_get_destination(tr);
    struct mac s = trame_get_source(tr);
    protocol_t p = trame_get_protocol(tr);

    /* Accès en lecture seule au bloc DATA */
    const void *data_ptr = NULL;
    size_t data_len = trame_get_data(tr, &data_ptr);

    /* Vérification de l'intégrité */
    bool ok = trame_is_fcs_valid(tr);

    deinit_trame(tr);
    return 0;
}