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
// #include "../../projects/lpc1758_freertos_Distance_sensors_unit_tests/L4_IO/lidar_sensor.hpp"
// #include "../../projects/TataNano/Autonomous-car/lpc1758_freertos_Distance_sensors/L2_Drivers/uart2.hpp"
#include "lidar_sensor.cpp"
#include "uart2.cpp"
#include "can.c"


Ensure(test_lane_algorithm)
{

    bool lane_check[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    float integer = -1.0;

    for(int i = 270; i < 290; i++)
    {
        expect(angle_value_deg, will_return(integer));//this will go around -1 to 361
        expect(distance_value_cm, will_return(300));//this is within th range
        integer++;
    }

    assert_that(rplidar.lane_lut[0], is_equal_to(true));

    for(int i = 0; i < 362; i++)
    {
        expect(angle_value_deg, will_return(integer));//this will go around -1 to 361
        expect(distance_value_cm, will_return(300));//this is within th range
        integer++;
    }


    assert_that(rplidar.lane_lut, is_equal_to(lane_check));
}

Ensure (test_get_quality_value)
{
    uint16_t x = 9;
    expect(receive_lidar_data, will_return(9.0));//this will go around -1 to 361
    assert_that(rplidar.get_quality_value(), is_equal_to(x));
    expect(receive_lidar_data, will_return(-5.0));//this will go around -1 to 361
    assert_that(rplidar.get_quality_value(), is_equal_to(-5));
    expect(receive_lidar_data, will_return(9.0));//this will go around -1 to 361
    assert_that(rplidar.get_quality_value(), is_equal_to(x));
}

Ensure(take_so_long_that_ctrl_c_is_needed) {
    sleep(10);
}



TestSuite *start_scan_tests() 
{
    TestSuite *suite = create_test_suite();
    add_test(suite, test_lane_algorithm);
    add_test(suite, test_get_quality_value);
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