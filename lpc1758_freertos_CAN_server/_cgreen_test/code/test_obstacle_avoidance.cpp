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
#include "../../L5_Application/periodic_scheduler/periodic_callbacks.h"

//#include "test_gps_parse.cpp"



Ensure(expectation_confirmed) 
{
    //this function will receive sensor message, process, and store
    //into variables that are easily accessible by periodic functions
    expect(receive_sensor_can_message, when(i, is_equal_to(3)));
    //this function will take the variables above and accesss lidar
    //data and determine which way is the best way to proceeed
    //based on the data and the current orientation with respect to destination
    expect(det_smollest_angle, when(i, is_equal_to(3)));
    //this function will simply take the variables from above and send
    //them to the motor controller so that the motor controller can execute these
    //commands
    expect(send_dir_and_rpm_to_motor, when(i, is_equal_to(3)));

}

TestSuite *foo() 
{
    TestSuite *suite = create_test_suite();
    add_test(suite, expectation_confirmed);
    return suite;
}


int main(int argc, char **argv) 
{
    TestSuite *suite = create_test_suite();

    add_suite(suite, foo());

    if (argc > 1) {
        add_test(suite, take_so_long_that_ctrl_c_is_needed);
        return run_single_test(suite, argv[1], create_text_reporter());
    }

    return run_test_suite(suite, create_text_reporter());
}