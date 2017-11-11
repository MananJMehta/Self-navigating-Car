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




Ensure(test_update_lanes)
{
    bool lane_check[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    int angle[362];
    float integer = -1.0;

    for(int i = 0; i < 362; i++)
    {
        expect(get_quality_value, will_return(9.0);
        expect(get_angle_value, will_return(integer));//this will go around -1 to 361
        expect(get_distance_value, will_return(10.0));//this is within th range
        integer++;
    }

    assert_that(x, is_equal_to(lane_check));
}


TestSuite *start_scan_tests() 
{
    TestSuite *suite = create_test_suite();
    add_test(suite, test_update_lanes);
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