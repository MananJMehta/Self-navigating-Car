#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "../../L4_IO/L4.5_Motor_Control/Steering.hpp"      // The file under test

// Include for C++
using namespace cgreen;

int Steering::init()(void) { return mock(); }

Ensure(testing_steering)
{
    int value_val;
    Steering str;

    expect(str.init, will_return(true));

}


TestSuite *steering_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, testing_steering);
    return suite;
}
