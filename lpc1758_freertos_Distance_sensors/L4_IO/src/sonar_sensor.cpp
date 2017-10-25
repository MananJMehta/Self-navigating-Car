/*
 * sonar_sensor.cpp
 *
 *  Created on: 21-Oct-2017
 *      Author: Shashank Iyer
 */
#include "sonar_sensor.hpp"
#include "eint.h"
#include "lpc_sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "stdio.h"
#include "utilities.h"

extern SemaphoreHandle_t sonar_mutex;
extern float left_start, dist_in_left, left_stop;
extern float center_start, dist_in_center, center_stop;
extern float right_start, dist_in_right, right_stop;

static GPIO myPinRX1(P2_1);     // left sensor
static GPIO myPinRX2(P2_3);     // center sensor
static GPIO myPinRX3(P2_5);     // right sensor
GPIO PWM_left(P2_0);
GPIO PWM_center(P2_2);
GPIO PWM_right(P2_4);


void Sonar_Sensor::sensor_detect_rise_left()
{
    left_start = sys_get_uptime_us();
}

void Sonar_Sensor::sensor_detect_fall_left()
{
    left_stop = sys_get_uptime_us();
    dist_in_left = ((left_stop - left_start)/147);
    //xSemaphoreGiveFromISR(sonar_mutex,0);
}

// callbacks for center sensor

void Sonar_Sensor::sensor_detect_rise_center()
{
    center_start = sys_get_uptime_us();
}

void Sonar_Sensor::sensor_detect_fall_center()
{
    center_stop = sys_get_uptime_us();
    dist_in_center = ((center_stop - center_start)/147);
}

// callbacks for right sensor

void Sonar_Sensor::sensor_detect_rise_right()
{
    right_start = sys_get_uptime_us();
}

void Sonar_Sensor::sensor_detect_fall_right()
{
    right_stop = sys_get_uptime_us();
    dist_in_right = ((right_stop - right_start)/147);
    xSemaphoreGiveFromISR(sonar_mutex,0);
}



#ifdef __cplusplus
extern "C" {
#endif

bool Sonar_Sensor::init()
{
    myPinRX1.setAsOutput();
    myPinRX2.setAsOutput();
    myPinRX3.setAsOutput();
    PWM_left.setAsInput();
    PWM_center.setAsInput();
    PWM_right.setAsInput();

//initialising interrupt pins for sensor 1

    eint3_enable_port2(0,eint_falling_edge,sensor_detect_fall_left);
    eint3_enable_port2(0,eint_rising_edge,sensor_detect_rise_left);

    eint3_enable_port2(2,eint_falling_edge,sensor_detect_fall_center);
    eint3_enable_port2(2,eint_rising_edge,sensor_detect_rise_center);

    eint3_enable_port2(4,eint_falling_edge,sensor_detect_fall_right);
    eint3_enable_port2(4,eint_rising_edge,sensor_detect_rise_right);

    return true;
}

#ifdef __cplusplus
}
#endif

void Sonar_Sensor::start_operation()
{
//initialising trigger pin for sensor left
    //myPinRX1.setAsOutput();
    myPinRX1.setHigh();
    delay_us(25);
    myPinRX1.setLow();

}
