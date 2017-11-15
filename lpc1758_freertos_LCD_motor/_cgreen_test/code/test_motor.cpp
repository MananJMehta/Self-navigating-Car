#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "../../L4_IO/L4.5_Motor_Control/speed_util.cpp"      // The file under test
// Include for C++
using namespace cgreen;

BeforeEach(testing_motor){}

/**
 * Tests to check check if the motor would maintain speed
 */

Ensure(testing_motor_maintain_speed)
{
    /**
     * @Params
     * check_speed_diff(uint16_t rpm, uint16_t ref, float curr_speed, float *val)
     */
    float val;
    check_speed_diff(10, 8, 5, &val);
    val = val*10;                       //Scale it by 10 to convert to an integer for testing
    assert_equal(val, 49);

    check_speed_diff(8, 9, 5, &val);
    val = val*10;                       //Scale it by 10 to convert to an integer for testing
    assert_equal(val, 50);

    check_speed_diff(8, 10, 5, &val);
    val = val*10;                       //Scale it by 10 to convert to an integer for testing
    assert_equal(val, 51);

    check_speed_diff(8, 8, 5, &val);
    val = val*10;                       //Scale it by 10 to convert to an integer for testing
    assert_equal(val, 50);

    check_speed_diff(10, 8, 15.5, &val);
    val = val*10;                       //Scale it by 10 to convert to an integer for testing
    assert_equal(val, 154);

    check_speed_diff(8, 10, 15.5, &val);
    val = val*10;                       //Scale it by 10 to convert to an integer for testing
    assert_equal(val, 155);

    check_speed_diff(8, 10, 100, &val);
    val = val*10;                       //Scale it by 10 to convert to an integer for testing
    assert_equal(val, 155);

    check_speed_diff(10, 8, 100, &val);
    val = val*10;                       //Scale it by 10 to convert to an integer for testing
    assert_equal(val, 155);
}


/**
 * Tests to check check if RPM, Speed and timings are maintained
 */

BeforeEach(testing_motor_rpm_meter){}


Ensure(testing_motor_rpm_meter)
{
    /**
     * @Params
     * rpm_mtr_val(uint8_t *second_count, uint16_t *rps, uint16_t *cut_count, uint16_t *old_count, float *speed, uint16_t *old_countms)
     */
    uint8_t second = 59; uint16_t rps = 2; uint16_t cut_count = 9; uint16_t old_count = 8; float speed = 2; uint16_t old_countms = 8;

    rpm_mtr_val(&second, &rps, &cut_count, &old_count, &speed, &old_countms);
    assert_equal(second, 0);
    assert_equal(rps, 1);
    assert_equal(cut_count, 0);
    assert_equal(old_count, 0);
    speed=speed*1000;                  //Scale it by 1000 to convert to an integer for testing
    assert_equal(speed, 328);
    assert_equal(old_countms, 0);


    second = 0; rps = 2; cut_count = 9; old_count = 8; speed = 2; old_countms = 8;

    rpm_mtr_val(&second, &rps, &cut_count, &old_count, &speed, &old_countms);
    assert_equal(second, 1);
    assert_equal(rps, 1);
    assert_equal(cut_count, 9);
    assert_equal(old_count, 9);
    speed=speed*1000;               //Scale it by 1000 to convert to an integer for testing
    assert_equal(speed, 328);
    assert_equal(old_countms, 8);



    second = 1; rps = 2; cut_count = 9; old_count = 0; speed = 9; old_countms = 8;

    rpm_mtr_val(&second, &rps, &cut_count, &old_count, &speed, &old_countms);
    assert_equal(second, 2);
    assert_equal(rps, 9);
    assert_equal(cut_count, 9);
    assert_equal(old_count, 9);
    speed=speed*1000;               //Scale it by 1000 to convert to an integer for testing
    assert_equal(speed, 2956);
    assert_equal(old_countms, 8);


    second = 5; rps = 2; cut_count = 8; old_count = 6; speed = 0; old_countms = 5;

    rpm_mtr_val(&second, &rps, &cut_count, &old_count, &speed, &old_countms);
    assert_equal(second, 6);
    assert_equal(rps, 2);
    assert_equal(cut_count, 8);
    assert_equal(old_count, 8);
    speed=speed*1000;               //Scale it by 1000 to convert to an integer for testing
    assert_equal(speed, 657);
    assert_equal(old_countms, 5);


    second = 5; rps = 2; cut_count = 11; old_count = 3; speed = 5; old_countms = 5;

    rpm_mtr_val(&second, &rps, &cut_count, &old_count, &speed, &old_countms);
    assert_equal(second, 6);
    assert_equal(rps, 8);
    assert_equal(cut_count, 11);
    assert_equal(old_count, 11);
    speed=speed*1000;               //Scale it by 1000 to convert to an integer for testing
    assert_equal(speed, 2628);
    assert_equal(old_countms, 5);



    second = 6; rps = 5; cut_count = 12; old_count = 3; speed = 6; old_countms = 5;

    rpm_mtr_val(&second, &rps, &cut_count, &old_count, &speed, &old_countms);
    assert_equal(second, 7);
    assert_equal(rps, 9);
    assert_equal(cut_count, 12);
    assert_equal(old_count, 12);
    speed=speed*1000;
    assert_equal(speed, 2956);
    assert_equal(old_countms, 5);

}


TestSuite *motor_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, testing_motor_maintain_speed);
    add_test(suite, testing_motor_rpm_meter);
    return suite;
}
