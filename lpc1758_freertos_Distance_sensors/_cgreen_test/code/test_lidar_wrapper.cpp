#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

// Include for C++
using namespace cgreen;

// Include other files under test
#include "test_c_list.c"
#include "test_foo.cpp"
//#include "test_gps_parse.cpp"
#include "../../L4_IO/lidar_sensor.cpp"




Ensure(char_stream_returns_true)
{
	expect(check_start_scan, will_return(true));
    int x = getMotorPWM();
    assert_that(x, is_equal_to(-1));
}


TestSuite *start_scan_tests() 
{
    TestSuite *suite = create_test_suite();
    add_test(suite, char_stream_returns_true);
    return suite;
}


int main(int argc, char **argv) 
{
    TestSuite *suite = create_test_suite();
    add_suite(suite, start_scan_tests());

    if (argc > 1) {
        add_test(suite, take_so_long_that_ctrl_c_is_needed);
        return run_single_test(suite, argv[1], create_text_reporter());
    }

    return run_test_suite(suite, create_text_reporter());
}