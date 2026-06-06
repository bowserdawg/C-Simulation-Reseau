#include "address_ip.h"
#include "address_mac.h"
#include "switch.h"

#define SWITCH_COMMUTATION_TABLE_SIZE 16384

/**
 * Initialise un nouveau switch. 
 */
struct optional_device_switch init_switch(struct mac mac, port_t nbPort)
{
    enum EXIT_CODES ex_code = SUCESS;

    struct optional_table_commutation table = init_table_commutation(SWITCH_COMMUTATION_TABLE_SIZE);
    if(table.exit_code != SUCESS)
    {
        DEBUG_WARNING(table.exit_code);
        ex_code = table.exit_code;
    }

    return (struct optional_device_switch)
                {
                    .dev_switch = (struct device_switch)
                    {
                        .table_commutation = table.table,
                        ._mac = mac, 
                        ._nbPort = nbPort
                    }, 

                    .exit_code = ex_code
                };
}