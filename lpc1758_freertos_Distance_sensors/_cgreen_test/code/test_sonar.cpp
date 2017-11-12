#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "../../L4_IO/src/sonar_sensor.cpp"// The file under test
#include "lpc_sys.h"

// Include for C++
using namespace cgreen;

float sys_get_uptime_us(void)
{

}
Ensure(sonar_testing)
{

}

TestSuite *sonar_testSuite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, sonar_testing);
    return suite;
}
