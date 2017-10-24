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

//Sonar_Sensor left_sensor,right_sensor, center_sensor;

extern float get_start_time1, dist1, get_stop_time1;
extern float get_start_time2, dist2, get_stop_time2;
extern float get_start_time3, dist3, get_stop_time3;

static GPIO myPinRX1(P2_1);     // left sensor
static GPIO myPinRX2(P2_3);     // center sensor
static GPIO myPinRX3(P2_5);     // right sensor
GPIO PWM_left(P2_0);
GPIO PWM_center(P2_2);
GPIO PWM_right(P2_4);


void Sonar_Sensor::sensor_detect_rise_left()
{
    get_start_time1 = sys_get_uptime_us();
}

void Sonar_Sensor::sensor_detect_fall_left()
{
    get_stop_time1 = sys_get_uptime_us();
    dist1 = ((get_stop_time1 - get_start_time1)/147);
}

// callbacks for center sensor

void Sonar_Sensor::sensor_detect_rise_center()
{
    get_start_time2 = sys_get_uptime_us();
}

void Sonar_Sensor::sensor_detect_fall_center()
{
    get_stop_time2 = sys_get_uptime_us();
    dist2 = ((get_stop_time2 - get_start_time2)/147);
}

// callbacks for right sensor

void Sonar_Sensor::sensor_detect_rise_right()
{
    get_start_time3 = sys_get_uptime_us();
}

void Sonar_Sensor::sensor_detect_fall_right()
{
    get_stop_time3 = sys_get_uptime_us();
    dist3 = ((get_stop_time3 - get_start_time3)/147);
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

//initialising trigger pin for sensor center
    //myPinRX2.setAsOutput();
    myPinRX2.setHigh();
    delay_us(25);
    myPinRX2.setLow();

//initialising trigger pin for sensor right
   // myPinRX3.setAsOutput();
    myPinRX3.setHigh();
    delay_us(25);
    myPinRX3.setLow();

}
