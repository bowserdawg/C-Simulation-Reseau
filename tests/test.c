#include <assert.h>
#include <stdio.h>

void test_ip();
void test_mac();

static void terminal_message(const char* message)
{
    fprintf(stderr, "  %s\n", message);
}

int main()
{
    terminal_message("Passing test_ip...");
    test_ip();
    terminal_message("Passing test_mac...");
    test_mac();

    return 0;
}