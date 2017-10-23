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

float get_start_time1 , dist1, get_stop_time1;
float get_start_time2 , dist2, get_stop_time2;
float get_start_time3 , dist3, get_stop_time3;
SemaphoreHandle_t xSensorHandle_rise1=xSemaphoreCreateBinary();
SemaphoreHandle_t xSensorHandle_fall1=xSemaphoreCreateBinary();
SemaphoreHandle_t xSensorHandle_rise2=xSemaphoreCreateBinary();
SemaphoreHandle_t xSensorHandle_fall2=xSemaphoreCreateBinary();
SemaphoreHandle_t xSensorHandle_rise3=xSemaphoreCreateBinary();
SemaphoreHandle_t xSensorHandle_fall3=xSemaphoreCreateBinary();

//////Call back functions and distance calculators for sensor1//////////
void rise_detector1()
{
    get_start_time1 = sys_get_uptime_us();
}

void fall_detector1()
{
    get_stop_time1 = sys_get_uptime_us();
}

bool Sonar_Sensor::obstacle_1()
{

        dist1 = ((get_stop_time1 - get_start_time1)/147);
        printf("Y\n"); printf("%f\n",dist1);
        if(dist1<=20.0)
            return true;

    return false;
}

//////Call back functions and distance calculators for sensor 2//////////

void rise_detector2()
{
    xSemaphoreGive(xSensorHandle_rise2);
}

void sensor_detect_rise2()
{
    if( xSemaphoreTake( xSensorHandle_rise2, ( TickType_t ) 1 ) == pdTRUE )
        get_start_time2 = sys_get_uptime_us();
}

void fall_detector2()
{
    xSemaphoreGive(xSensorHandle_fall2);
}

bool Sonar_Sensor::obstacle_2()
{
    if( xSemaphoreTake( xSensorHandle_fall2, ( TickType_t ) 1 ) == pdTRUE ){
        get_stop_time2 = sys_get_uptime_us();
        dist2 = ((get_stop_time2 - get_start_time2)/147);
        if(dist2<=80)
            return true;
    }
    return false;
}

//////Call back functions and distance calculators for sensor 3//////////

void rise_detector3()
{
    xSemaphoreGive(xSensorHandle_rise3);
}

void sensor_detect_rise3()
{
    if( xSemaphoreTake( xSensorHandle_rise3, ( TickType_t ) 1 ) == pdTRUE )
        get_start_time3 = sys_get_uptime_us();
}

void fall_detector3()
{
    xSemaphoreGive(xSensorHandle_fall3);
}

bool Sonar_Sensor::obstacle_3()
{
    if( xSemaphoreTake( xSensorHandle_fall3, ( TickType_t ) 1 ) == pdTRUE ){
        get_stop_time3 = sys_get_uptime_us();
        dist3 = ((get_stop_time3 - get_start_time3)/147);
        if(dist3<=80)
            return true;
    }
    return false;
}

#ifdef __cplusplus
extern "C" {
#endif

void Sonar_Sensor::init()
{
    //initialising interrupt pins for sensor 1
    eint3_enable_port2(0,eint_rising_edge, rise_detector1);
    eint3_enable_port2(0,eint_falling_edge, fall_detector1);

    //initialising interrupt pins for sensor 2
    eint3_enable_port2(2,eint_rising_edge, rise_detector2);
    eint3_enable_port2(2,eint_falling_edge, fall_detector2);

    //initialising interrupt pins for sensor 3
    eint3_enable_port2(4,eint_rising_edge, rise_detector3);
    eint3_enable_port2(4,eint_falling_edge, fall_detector3);
}

#ifdef __cplusplus
}
#endif

void Sonar_Sensor::start_operation()
{
    //initialising trigger pin for sensor 1
    static GPIO myPinRX1(P2_1);
    myPinRX1.setAsOutput();
    myPinRX1.setHigh();

    //initialising trigger pin for sensor 2
    static GPIO myPinRX2(P2_3);
    myPinRX2.setAsOutput();
    myPinRX2.setHigh();

    //initialising trigger pin for sensor 3
    static GPIO myPinRX3(P2_5);
    myPinRX3.setAsOutput();
    myPinRX3.setHigh();

    //essential for HW state machine
    delay_us(25);

    myPinRX1.setLow();
    myPinRX2.setLow();
    myPinRX3.setLow();
}
