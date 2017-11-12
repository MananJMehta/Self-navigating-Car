#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "../../L5_Application/periodic_scheduler/lcd_utilities.cpp"      // The file under test

// Include for C++
using namespace cgreen;

Ensure(testing_lcd)
{
    uint8_t lsb_val, msb_val;
    uint16_t geo_left, geo_right;

    /**
     * Testing random number generation
     */
    assert_that(get_random_int(20), is_less_than(21));
    assert_that(get_random_int(20), is_greater_than(-1));
    assert_that(get_random_int(-1), is_equal_to(0));
    assert_that(get_random_int(99999999), is_greater_than(-1));
    assert_that(get_random_int(99999999), is_less_than(100000000));


    /**
     * Testing conversion from 16 bit to 8 bit values
     */
    convert16_to_hex(0x0000, &lsb_val, &msb_val);
    assert_that(lsb_val, is_equal_to(0x00));
    assert_that(msb_val, is_equal_to(0x00));

    convert16_to_hex(0x0001, &lsb_val, &msb_val);
    assert_that(lsb_val, is_equal_to(0x01));
    assert_that(msb_val, is_equal_to(0x00));

    convert16_to_hex(0xFFFF, &lsb_val, &msb_val);
    assert_that(lsb_val, is_equal_to(0xFF));
    assert_that(msb_val, is_equal_to(0xFF));

    convert16_to_hex(0x1234, &lsb_val, &msb_val);
    assert_that(lsb_val, is_equal_to(0x34));
    assert_that(msb_val, is_equal_to(0x12));

    convert16_to_hex(0xFF00, &lsb_val, &msb_val);
    assert_that(lsb_val, is_equal_to(0x00));
    assert_that(msb_val, is_equal_to(0xFF));

    convert16_to_hex(0x00FF, &lsb_val, &msb_val);
    assert_that(lsb_val, is_equal_to(0xFF));
    assert_that(msb_val, is_equal_to(0x00));

    /**
     * Tests for displaying Geo values
     */
    convert_geo(100000000, &geo_left, &geo_right);
    assert_that(geo_left, is_equal_to(10000));
    assert_that(geo_right, is_equal_to(0000));

    convert_geo(360123456, &geo_left, &geo_right);
    assert_that(geo_left, is_equal_to(36012));
    assert_that(geo_right, is_equal_to(3456));

    //TODO - This should return false
    convert_geo(370123456, &geo_left, &geo_right);
    assert_that(geo_left, is_equal_to(37012));
    assert_that(geo_right, is_equal_to(3456));

    convert_geo(299999999, &geo_left, &geo_right);
    assert_that(geo_left, is_equal_to(29999));
    assert_that(geo_right, is_equal_to(9999));

    convert_geo(000000000, &geo_left, &geo_right);
    assert_that(geo_left, is_equal_to(00000));
    assert_that(geo_right, is_equal_to(0000));

}

TestSuite *lcd_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, testing_lcd);
    return suite;
}
