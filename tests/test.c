#include <assert.h>
#include <stdio.h>

void test_ip();

static void terminal_message(const char* message)
{
    fprintf(stderr, "  %s\n", message);
}

int main()
{
    terminal_message("Passing test_ip...");
    test_ip();

    return 0;
}