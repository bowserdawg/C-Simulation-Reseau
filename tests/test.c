#include <assert.h>
#include <stdio.h>

void test_ip();
void test_mac();
void table_commutation_test();

static void terminal_message(const char* message)
{
    fprintf(stderr, "  \033[92m%s\033[0m\n", message);
}

int main()
{
    terminal_message("Starting test_ip...");
    test_ip();
    terminal_message("Starting test_mac...");
    test_mac();
    terminal_message("Starting table_commutation_test...");
    table_commutation_test();

    return 0;
}