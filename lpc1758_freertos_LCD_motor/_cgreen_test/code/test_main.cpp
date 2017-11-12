#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

// Include for C++
using namespace cgreen;

// Include other files under test
#include "test_lcd.cpp"


#define SHOW_SAMPLE_PASS_AND_FAIL_TESTS_ONLY         1

#if SHOW_SAMPLE_PASS_AND_FAIL_TESTS_ONLY

int main(int argc, char **argv) 
{
    TestSuite *suite = create_test_suite();
    add_suite(suite, lcd_suite());

    return run_test_suite(suite, create_text_reporter());
}

#endif // !SHOW_SAMPLE_PASS_AND_FAIL_TESTS_ONLY
