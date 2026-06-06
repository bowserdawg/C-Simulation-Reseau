#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "errors.h"
#include "address_ip.h"
#include "string.h"

void test_ip()
{
    struct ip ip1 = init_ip(192, 12, 10, 0);


    struct optional_ip ip2 = ip_from_string("192.12.10.0");
    DEBUG_ASSERT_W_MESSAGE(ip2.exit_code == SUCESS, "FAIL ip2 optional\n");

    DEBUG_ASSERT_W_MESSAGE(ip1.ip == ip2.ip.ip, "FAIL ip1.ip == ip2.ip.ip\n");

    char* ip_str1 = NULL;
    ip_to_string(&ip_str1, ip1);

    char* ip_str2 = NULL;
    ip_to_string(&ip_str2, ip2.ip);

    DEBUG_ASSERT_W_MESSAGE(ip_str1 != NULL, "FAIL ip_str1 is null.\n");
    DEBUG_ASSERT_W_MESSAGE(ip_str2 != NULL, "FAIL ip_str2 is null.\n");

    DEBUG_ASSERT_W_MESSAGE(strcmp(ip_str1, ip_str2) == 0, "FAIL both strings aren't similar\n");

    free(ip_str1);
    free(ip_str2);

    
}