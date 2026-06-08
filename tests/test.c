#include <assert.h>
#include <stdio.h>

void test_ip();
void test_mac();
void test_table_commutation();
void test_station();
void test_switch();
void test_net_graph();
void test_parser();

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
    terminal_message("Starting test_table_commutation...");
    test_table_commutation();
    terminal_message("Starting test_station...");
    test_station();
    terminal_message("Starting test_switch...");
    test_switch();
    terminal_message("Starting test_net_graph...");
    test_net_graph();
    terminal_message("Starting test_parser...");
    test_parser();


    return 0;
}