#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

// Include for C++
using namespace cgreen; 

// Include other files under test
// #include "test_c_list.c"
#include "test_foo.cpp"
#include "functions.h"
#include "functions.c"
//#include "test_gps_parse.cpp"



#define SHOW_SAMPLE_PASS_AND_FAIL_TESTS_ONLY         0

#if SHOW_SAMPLE_PASS_AND_FAIL_TESTS_ONLY

Ensure(these_should_be_true)
{
    assert_true(1);
    assert_false(0);
}

Ensure(these_should_be_false)
{
    assert_true(1);
    assert_false(0);
}

Ensure(these_should_be_equal)
{
    assert_equal(1, 1);
    assert_not_equal(0, 1);
}

Ensure(these_strings_should_match)
{
    assert_string_equal("Hello", "Hello");
    assert_string_not_equal("Hello", "hello");
}

TestSuite *assertion_tests() 
{
    TestSuite *suite = create_test_suite();
    add_test(suite, these_should_be_true);
    add_test(suite, these_should_be_false);
    add_test(suite, these_should_be_equal);
    add_test(suite, these_strings_should_match);
    return suite;
}

static int an_integer = 0;

void set_up_an_integer() 
{
    an_integer = 1;
}

Ensure(confirm_integer_is_set_up) 
{
    assert_equal_with_message(an_integer, 1, "Could not set up the integer");
    an_integer = 2;
}

Ensure(check_again_during_teardown) 
{
    assert_equal_with_message(an_integer, 2, "Integer was changed from 1 to %d", an_integer);
}

TestSuite *fixture_tests() 
{
    TestSuite *suite = create_test_suite();
    set_setup(suite, set_up_an_integer);
    set_teardown(suite, check_again_during_teardown);
    add_test(suite, confirm_integer_is_set_up);
    return suite;
}

static void print_something_during_setup() 
{
    printf("\tI was called during setup\n");
}

Ensure(print_something_during_a_test) 
{
    printf("\tI am a test\n");
}

static void print_something_during_teardown() 
{
    printf("\tI was called during teardown\n");
}

TestSuite *visible_test()
{
    TestSuite *suite = create_test_suite();
    set_setup(suite, print_something_during_setup);
    add_test(suite, print_something_during_a_test);
    set_teardown(suite, print_something_during_teardown);
    return suite;
}

static void print_something_during_suite_setup() 
{
    printf("I was called during suite setup\n");
}

static void print_something_during_suite_teardown() 
{
    printf("I was called during suite teardown\n");
}

TestSuite *visible_fixtures() 
{
    TestSuite *suite = create_test_suite();
    set_setup(suite, print_something_during_suite_setup);
    add_suite(suite, visible_test());
    add_suite(suite, visible_test());
    set_teardown(suite, print_something_during_suite_teardown);
    return suite;
}

int interference = 0;

Ensure(create_test_interference) 
{
    interference = 1;
}

Ensure(prove_there_is_no_test_interference) 
{
    assert_equal(interference, 0);
}

Ensure(seg_fault) 
{
    int *p = NULL;
    (*p)++;
}

Ensure(time_out_in_only_one_second) 
{
    die_in(1);
    sleep(10);
}

TestSuite *isolation_tests()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, create_test_interference);
    add_test(suite, prove_there_is_no_test_interference);
    add_test(suite, seg_fault);
    add_test(suite, time_out_in_only_one_second);
    return suite;
}

static void takes_integer(int i) 
{
    mock(i);
}

Ensure(expectation_confirmed) 
{
    expect(takes_integer, when(i, is_equal_to(3)));
    takes_integer(3);
}

Ensure(expectation_dashed) 
{
    expect(takes_integer, when(i, is_equal_to(3)));
    takes_integer(4);
}

// static void mixed_parameters(int i, char *s)
// {
//     mock(i, s);
// }

Ensure(confirming_multiple_parameters_multiple_times) 
{
    // expect(mixed_parameters, when(i, is_equal_to(1)), when(s, is_equal_to_string("Hello")));
    // expect(mixed_parameters, when(i, is_equal_to(2)), when(s, is_equal_to_string("Goodbye")));
    // mixed_parameters(1, "Hello");
    // mixed_parameters(2, "Goodbye");
}

Ensure(breaking_multiple_parameters_multiple_times) 
{
    // expect(mixed_parameters, when(i, is_equal_to(1)), when(s, is_equal_to_string("Hello")));
    // expect(mixed_parameters, when(i, is_equal_to(2)), when(s, is_equal_to_string("Goodbye")));
    // mixed_parameters(10, "Helloo");
    // mixed_parameters(20, "Gooodbye");
}

Ensure(uncalled_expectations_should_throw_errors) 
{
    // expect(mixed_parameters, when(i, is_equal_to(1)), when(s, is_equal_to_string("Hello")));
}

Ensure(unexpected_call_should_throw_error) 
{
    // never_expect(mixed_parameters);
    // mixed_parameters(10, "Helloo");
}


Ensure(stuff)
{

}

TestSuite *mock_tests() 
{
    TestSuite *suite = create_test_suite();
    add_test(suite, expectation_confirmed);
    add_test(suite, expectation_dashed);
    add_test(suite, stuff);
    // add_test(suite, confirming_multiple_parameters_multiple_times);
    // add_test(suite, breaking_multiple_parameters_multiple_times);
    // add_test(suite, uncalled_expectations_should_throw_errors);
    // add_test(suite, unexpected_call_should_throw_error);
    return suite;
}

Ensure(take_so_long_that_ctrl_c_is_needed) {
    sleep(10);
}

int main(int argc, char **argv) 
{
    TestSuite *suite = create_test_suite();
    // add_suite(suite, assertion_tests());
    // add_suite(suite, fixture_tests());
    // add_suite(suite, visible_fixtures());
    // add_suite(suite, isolation_tests());
    add_suite(suite, mock_tests());
    // add_suite(suite, foo());

    if (argc > 1) {
        add_test(suite, take_so_long_that_ctrl_c_is_needed);
        return run_single_test(suite, argv[1], create_text_reporter());
    }

    return run_test_suite(suite, create_text_reporter());
}


//write your unit tests here
#else // SHOW_SAMPLE_PASS_AND_FAIL_TESTS_ONLY

// bool angle_is_in_range_of_current_lane(uint8_t current_lane)
// {
//     return mock();
// }

// bool angle_is_in_range_of_next_lane(uint8_t next_lane)
// {
//     return mock();
// }

Ensure(test_lane_lut)
{
    bool bunk_lanes[9] = {0};
    assert_equal(lane_lut[0], bunk_lanes[0]);
}

Ensure(test_angle_1)
{
    expect(angle_is_in_range_of_current_lane, will_return(true));
    //expect(angle_is_in_range_of_next_lane, will_return(false));
    assert_that(lane_algorithm(), is_equal_to(true));
}

Ensure(test_angle_2)
{
    expect(angle_is_in_range_of_current_lane, will_return(false));
    expect(angle_is_in_range_of_next_lane, will_return(true));
    assert_that(lane_algorithm(), is_equal_to(true));
}

Ensure(test_angle_3)
{
    expect(angle_is_in_range_of_current_lane, will_return(false));
    expect(angle_is_in_range_of_next_lane, will_return(false));
    assert_that(lane_algorithm(), is_equal_to(false));
}



Ensure(test_full_range_once)
{
    bool bunk_lanes[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for(int i = 0; i < 359;i++)
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[3];
        lane_algorithm();
    }

    for(int i = 0; i < 359;i++)
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[3];
        lane_algorithm();
    }

    // for(int i = 0; i < 5;i++)
    // {
    //     angle_value_deg = i;
    //     distance_value_cm = test_distance_values[3];
    //     lane_algorithm();
    // }

    // for(int i = 4; i < 360;i++)
    // {
    //     angle_value_deg = i;
    //     distance_value_cm = test_distance_values[1];
    //     lane_algorithm();
    // }    
    
    assert_equal(lane_lut[0], bunk_lanes[0]);
    assert_equal(lane_lut[1], bunk_lanes[1]);
    assert_equal(lane_lut[2], bunk_lanes[2]);
    assert_equal(lane_lut[3], bunk_lanes[3]);
    assert_equal(lane_lut[4], bunk_lanes[4]);
    assert_equal(lane_lut[5], bunk_lanes[5]);
    assert_equal(lane_lut[6], bunk_lanes[6]);
    assert_equal(lane_lut[7], bunk_lanes[7]);
    assert_equal(lane_lut[8], bunk_lanes[8]);
    assert_equal(lane_distances[4], 600);
}


Ensure(testing_each_lane)
{
    bool bunk_lanes[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for(int i = 0; i < 10;i++)//lane 4 0 - 9
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }

    // printf("\n");
    // printf("Current Lane: %d", current_lane);
    // printf("\n");
    // printf("Next Lane: %d", next_lane);
    // printf("\n");

    for(int i = 10; i < 30; i++)//lane 5 10 - 29
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }


    // printf("\n");
    // printf("Current Lane: %d", current_lane);
    // printf("\n");
    // printf("Next Lane: %d", next_lane);
    // printf("\n");

    for(int i = 30; i < 50; i++)//lane 6 30 - 49
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }

    // printf("\n");
    // printf("Current Lane: %d", current_lane);
    // printf("\n");
    // printf("Next Lane: %d", next_lane);
    // printf("\n");

    for(int i = 50; i < 70; i++)//lane 7 50 - 69
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }

    // printf("\n");
    // printf("Current Lane: %d", current_lane);
    // printf("\n");
    // printf("Next Lane: %d", next_lane);
    // printf("\n");

    for(int i = 70; i < 90; i++)//lane 8 70 - 89
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }

    // printf("\n");
    // printf("Current Lane: %d", current_lane);
    // printf("\n");
    // printf("Next Lane: %d", next_lane);
    // printf("\n");

    for(int i = 90; i < 269; i++)//Dont care
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }

    // printf("\n");
    // printf("Current Lane: %d", current_lane);
    // printf("\n");
    // printf("Next Lane: %d", next_lane);
    // printf("\n");

    for(int i = 270; i < 290; i++)//lane 0 270 - 289
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }

    // printf("\n");
    // printf("Current Lane: %d", current_lane);
    // printf("\n");
    // printf("Next Lane: %d", next_lane);
    // printf("\n");

    for(int i = 290; i < 310; i++)//lane 1 290 - 309
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }

    // printf("\n");
    // printf("Current Lane: %d", current_lane);
    // printf("\n");
    // printf("Next Lane: %d", next_lane);
    // printf("\n");

    for(int i = 310; i < 330; i++)//lane 8 310 -330
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }

    // printf("\n");
    // printf("Current Lane: %d", current_lane);
    // printf("\n");
    // printf("Next Lane: %d", next_lane);
    // printf("\n");

    for(int i = 330; i < 350; i++)//lane 8 70 - 89
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }

    // printf("\n");
    // printf("Current Lane: %d", current_lane);
    // printf("\n");
    // printf("Next Lane: %d", next_lane);
    // printf("\n");

    for(int i = 350; i < 360; i++)//lane 8 70 - 89
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }

    // printf("\n");
    // printf("Current Lane: %d", current_lane);
    // printf("\n");
    // printf("Next Lane: %d", next_lane);
    // printf("\n");

    // for(int i = 0; i < 5;i++)
    // {
    //     angle_value_deg = i;
    //     distance_value_cm = test_distance_values[3];
    //     lane_algorithm();
    // }

    // for(int i = 4; i < 360;i++)
    // {
    //     angle_value_deg = i;
    //     distance_value_cm = test_distance_values[1];
    //     lane_algorithm();
    // }    
    
    assert_equal(lane_lut[0], bunk_lanes[0]);
    assert_equal(lane_lut[1], bunk_lanes[1]);
    assert_equal(lane_lut[2], bunk_lanes[2]);
    assert_equal(lane_lut[3], bunk_lanes[3]);
    assert_equal(lane_lut[4], bunk_lanes[4]);
    assert_equal(lane_lut[5], bunk_lanes[5]);
    assert_equal(lane_lut[6], bunk_lanes[6]);
    assert_equal(lane_lut[7], bunk_lanes[7]);
    assert_equal(lane_lut[8], bunk_lanes[8]);
    assert_equal(lane_distances[4], 600);
}

Ensure(test_lane_transitions)
{

    for(int i = 0; i < 37; i++)
    {
        angle_value_deg = test_array[i];
        distance_value_cm = test_distance_values[4];
        lane_algorithm();
    }
}

Ensure(test_360_range_once)
{
    
    for(int i = 0; i < 359;i++)
    {
        angle_value_deg = i;
        distance_value_cm = test_distance_values[6];
        lane_algorithm();
    }

    // for(int i = 0; i < 359;i++)
    // {
    //     angle_value_deg = i;
    //     distance_value_cm = test_distance_values[6];
    //     lane_algorithm();
    // }

    // for(int i = 0; i < 5;i++)
    // {
    //     angle_value_deg = i;
    //     distance_value_cm = test_distance_values[3];
    //     lane_algorithm();
    // }

    // for(int i = 4; i < 360;i++)
    // {
    //     angle_value_deg = i;
    //     distance_value_cm = test_distance_values[1];
    //     lane_algorithm();
    // }    
    
    assert_equal(lane_lut[0], bunk_lanes_high[0]);
    assert_equal(lane_lut[1], bunk_lanes_high[1]);
    assert_equal(lane_lut[2], bunk_lanes_high[2]);
    assert_equal(lane_lut[3], bunk_lanes_high[3]);
    assert_equal(lane_lut[4], bunk_lanes_high[4]);
    assert_equal(lane_lut[5], bunk_lanes_high[5]);
    assert_equal(lane_lut[6], bunk_lanes_high[6]);
    assert_equal(lane_lut[7], bunk_lanes_high[7]);
    assert_equal(lane_lut[8], bunk_lanes_high[8]);
    assert_equal(lane_lut[9], bunk_lanes_high[9]);
    assert_equal(lane_lut[10], bunk_lanes_high[10]);
    assert_equal(lane_lut[11], bunk_lanes_high[11]);
    assert_equal(lane_lut[12], bunk_lanes_high[12]);
    assert_equal(lane_lut[13], bunk_lanes_high[13]);
    assert_equal(lane_lut[14], bunk_lanes_high[14]);
    assert_equal(lane_lut[15], bunk_lanes_high[15]);
    assert_equal(lane_lut[16], bunk_lanes_high[16]);
    assert_equal(lane_lut[17], bunk_lanes_high[17]);
    assert_equal(lane_distances[4], 200);
}


TestSuite *my_tests() 
{
    TestSuite *suite = create_test_suite();
    // add_test(suite, test_lane_lut);
    // add_test(suite, test_angle_1);
    // add_test(suite, test_angle_2);
    // add_test(suite, test_angle_3);
    // add_test(suite, test_full_range_once);
    // add_test(suite, testing_each_lane);
    // add_test(suite, test_lane_transitions);
    add_test(suite, test_360_range_once);
    return suite;
}

int main(int argc, char **argv) 
{
    lidar_init();

    TestSuite *suite = create_test_suite();
    add_suite(suite, my_tests());
    return run_test_suite(suite, create_text_reporter());
}

#endif // !SHOW_SAMPLE_PASS_AND_FAIL_TESTS_ONLY
