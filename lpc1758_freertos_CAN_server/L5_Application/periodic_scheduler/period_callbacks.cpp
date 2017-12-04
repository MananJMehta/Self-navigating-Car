/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * This contains the period callback functions for the periodic scheduler
 *
 * @warning
 * These callbacks should be used for hard real-time system, and the priority of these
 * tasks are above everything else in the system (above the PRIORITY_CRITICAL).
 * The period functions SHOULD NEVER block and SHOULD NEVER run over their time slot.
 * For example, the 1000Hz take slot runs periodically every 1ms, and whatever you
 * do must be completed within 1ms.  Running over the time slot will reset the system.
 */

#include "string.h"
#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "can.h"
#include "_can_dbc/generated_can.h"
#include "printf_lib.h"
#include <cmath>
#include <utility>
#include <io.hpp>
using namespace std;

#define SONAR_CODE  0        //Uncomment to include SONAR code
//#define SONAR_ALERT         //Uncomment to include SONAR_ALERT code

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);


// Enum for Sonar Status
//enum {
//    sonar_safe = 0,     //No Obstacle
//    sonar_alert = 1,    //Obstacle at medium distance
//    sonar_critical = 2  //Obstacle critically near
//};


// Define for Motor Steer
#define HARDLEFT 12
#define LEFT 13
#define SOFTLEFT 14
#define CENTER 15
#define SOFTRIGHT 16
#define RIGHT 17
#define HARDRIGHT 18

// Enum for Motor Speed
//enum {
//    Forward_L1,     //medium
//    Forward_L2,     //fastest
//    Forward_L3,     //slowest
//    Stop,
//    Reverse
//};

//Define a critical distance for sonar
#define sonar_critical 60
#define sonar_warning 90
#define cep 4

uint8_t arr[9];
uint8_t flag_speed = 0;
bool boot_t = true;
bool flag_navigation = false;
bool flag_fix = false;
bool flag_next = false;
bool flag_free_run = false;
uint8_t direction = CENTER;

const SENSOR_DATA_t SENSOR_DATA__MIA_MSG = {1,1,1,1,1,1,1,1,1,15,15,15};
const uint32_t SENSOR_DATA__MIA_MS = 500;
/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3);

can_t canTest = can1;
SENSOR_DATA_t sensor_msg = { 0 };
ANDROID_CMD_t and_msg ={0};
CAR_CONTROL_t master_motor_msg = { 0 };
MASTER_REQUEST_t request_msg = {0};
HEARTBEAT_t heartbeat_msg;
COMPASS_t compass_msg = {0};
GPS_DATA_t gps_msg = {0};
ANDROID_LOCATION_t locate_msg = {0};

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);
    return CAN_tx(canTest, &can_msg, 0);
}

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    CAN_init(canTest, 100, 10, 10, NULL, NULL);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(canTest);

    //array for steering control
    arr[0]=15;//center

    arr[1]=SOFTRIGHT;//soft right
    arr[3]=RIGHT;//right
    arr[5]=HARDRIGHT;//hard right
    arr[7]=HARDRIGHT;//extreme right

    arr[2]=SOFTLEFT;//soft left
    arr[4]=LEFT;//left
    arr[6]=HARDLEFT;//hard left
    arr[8]=HARDLEFT;//extreme left

    return true; // Must return true upon success
}

//checks if the lane specified by the bearing angle is free
//@params the direction specified by void correct_guidance(COMPASS_t x)
//@params lidar lanes
//@return whether the lane is free

bool check_direction_free(SENSOR_DATA_t y)
{
    if(direction == 0  && (y.LIDAR_0 == 0 && y.LIDAR_20 == 0 && y.LIDAR_neg20 == 0 ))
        return true;

    if((direction == 1 || direction == 3) && (y.LIDAR_0 == 0 && y.LIDAR_20 == 0 && y.LIDAR_40 == 0 ))
        return true;

    if(direction == 5 && (y.LIDAR_60 == 0 && y.LIDAR_20 == 0 && y.LIDAR_40 == 0 ))
            return true;

    if((direction == 2 || direction == 4) && (y.LIDAR_0 == 0 && y.LIDAR_neg20 == 0 && y.LIDAR_neg40 == 0 ))
        return true;

    if(direction == 6 && (y.LIDAR_neg60 == 0 && y.LIDAR_neg20 == 0 && y.LIDAR_neg40 == 0 ))
            return true;

    return false;
}

//reads data from the CAN message
//@returns the bit with highest priority
uint8_t map_get_value(SENSOR_DATA_t y)
{
    if(check_direction_free(y))
        return direction;
    if (y.LIDAR_0 == 1)
    {
        if(y.LIDAR_20 == 1 && y.LIDAR_neg20 == 1 && y.LIDAR_40 == 1 && y.LIDAR_neg40 == 1)
            return 9;
        if(y.LIDAR_20 == 1 && y.LIDAR_neg20 == 1 && y.LIDAR_40 == 0)
            return 5;
        if (y.LIDAR_20 == 1 && y.LIDAR_neg20 == 1 && y.LIDAR_neg40 == 0)
            return 6;
        if (y.LIDAR_20 == 1 && y.LIDAR_neg20 == 0)
            return 6;
        if (y.LIDAR_20 == 0 && y.LIDAR_neg20 == 1)
            return 5;
        if (y.LIDAR_neg40 == 0)
            return 6;
        if(y.LIDAR_40 == 0)
            return 5;
    }

    if(y.LIDAR_0 == 0 && y.LIDAR_20 == 0 && y.LIDAR_neg20 == 0 && y.LIDAR_40 ==1)
    {
        return 2;
    }
    if(y.LIDAR_0 == 0 && y.LIDAR_20 == 0 && y.LIDAR_neg20 == 0 && y.LIDAR_neg40 ==1)
    {
        return 1;
    }
    if(y.LIDAR_0 == 0 && y.LIDAR_20 == 0 && y.LIDAR_neg20 == 0 )
    {
        if(flag_free_run)
        {
            return 0;
        }else
        {
            return direction;
        }
    }
    if (y.LIDAR_0 == 0 && y.LIDAR_20 == 0 && y.LIDAR_neg20 == 1 )
        return 3;

    if (y.LIDAR_0 == 0 && y.LIDAR_20 == 1 && y.LIDAR_neg20 == 0)
        return 4;

    return 9;
}

//turns the steering if an obstacle is detected
//@params received can message from the lidar sensor
//@returns steering rotation and car speed
pair<uint8_t, uint8_t> update_lanes(SENSOR_DATA_t x)
{
    static pair<uint8_t , uint8_t> return_value;
    /*Aditya Change
return_value.first=HARDLEFT;//steering
     */
    return_value.first=CENTER;
    return_value.second=0;//speed

    uint8_t i=map_get_value(x);

    if(i==9)
    {
        LE.on(2);
        LE.on(3);
        LE.on(4);
        return return_value;
    }

    LE.off(2);
    LE.off(3);
    LE.off(4);

    return_value.first = arr[i];
    return_value.second = flag_speed;
    return return_value;
}

//stops the car if the obstacle is too close based on sonar
void stop_lidar (SENSOR_DATA_t& x)
{
    x.LIDAR_0 = 1;
    x.LIDAR_20 = 1;
    x.LIDAR_40 = 1;
    x.LIDAR_neg20 = 1;
    x.LIDAR_neg40 = 1;
}

bool reached_destination(COMPASS_t x)
{
    if (x.DISTANCE_CHECKPOINT > cep)
    {
        return false;
    }
    else
    {
        if(flag_next)
        {
            request_msg.MASTER_REQUEST_cmd = 0xf;
            dbc_encode_and_send_MASTER_REQUEST(&request_msg);
        }
        return true;
    }
}

void correct_guidance(COMPASS_t x)
{
    static pair<uint8_t, uint8_t> return_value;
    return_value.second = 1;
    return_value.first = CENTER;
    if (x.DEFLECTION_ANGLE <= 15 && x.DEFLECTION_ANGLE >= -15)
    {
        direction = 0; //CENTER
        //return_value.first = CENTER;

    }else
    if (x.DEFLECTION_ANGLE > 15 && x.DEFLECTION_ANGLE <= 35 )
    {
     //soft right
        direction = 1;  //SOFTRIGHT;
        //return_value.first = SOFTRIGHT;

    }else
    if (x.DEFLECTION_ANGLE > 35 && x.DEFLECTION_ANGLE <= 60)
    {
     //right
        direction = 3;//RIGHT;
        //return_value.first = RIGHT;

    }else
    if(x.DEFLECTION_ANGLE > 60 && x.DEFLECTION_ANGLE <= 180)
    {
       //hard right
        direction = 5;//HARDRIGHT;
        //return_value.first = HARDRIGHT;

    }else
    if (x.DEFLECTION_ANGLE < -15 && x.DEFLECTION_ANGLE >= -35 )
    {
     //soft left
        direction = 2;//SOFTLEFT;
        //return_value.first = SOFTLEFT;

    }else
    if (x.DEFLECTION_ANGLE < -35 && x.DEFLECTION_ANGLE >= -60)
    {
     //left
        direction = 4;//LEFT;
        //return_value.first = LEFT;

    }else
    if(x.DEFLECTION_ANGLE < -60 && x.DEFLECTION_ANGLE >= -180)
    {
       //hard left
        direction = 6;//HARDLEFT;
        //return_value.first = HARDLEFT;

    }

}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}

/**
 * Below are your periodic functions.
 * The argument 'count' is the number of times each periodic task is called.
 */
void period_1Hz(uint32_t count)
{
    //heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_SYNC;
    if(!boot_t)
    {
        dbc_encode_and_send_HEARTBEAT(&heartbeat_msg);

    }else
    {
        heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_REBOOT;
        dbc_encode_and_send_HEARTBEAT(&heartbeat_msg);
        heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_NOOP;
        boot_t = false;
    }
    LE.toggle(1);
    if(CAN_is_bus_off(can1))
    {
        CAN_reset_bus(can1);
    }
}

void period_10Hz(uint32_t count)
{
    if(SW.getSwitch(2))
    {
        flag_speed = 0;
        heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_NOOP;
    }
    if(SW.getSwitch(1))
    {
        flag_speed = 1;
        heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_SYNC;
    }
    if(SW.getSwitch(3))
    {
        compass_msg.DISTANCE_CHECKPOINT = 5; //CEP is 10
    }



    can_msg_t can_msg;
    while(CAN_rx(canTest,&can_msg,0))
    {
        dbc_msg_hdr_t can_header;
        can_header.dlc = can_msg.frame_fields.data_len;
        can_header.mid = can_msg.msg_id;
        switch(can_header.mid)
        {
            case 130:
                if (dbc_decode_ANDROID_CMD(&and_msg, can_msg.data.bytes, &can_header))
                {
                    if(and_msg.ANDROID_CMD_start == 0)
                    {
                        flag_speed = 0;
                        flag_navigation = false;
                        flag_free_run = false;
                        heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_NOOP;
                    }
                    else
                    {
                        if(and_msg.ANDROID_CMD_mode)
                        {
                            flag_navigation = true;
                            request_msg.MASTER_REQUEST_cmd = 0xf;
                            dbc_encode_and_send_MASTER_REQUEST(&request_msg);
                        }else
                        {
                            flag_navigation = false;
                            flag_free_run = true;
                            flag_speed = 1;
                        }

                        heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_SYNC;
                    }
                }
                break;
            case 135:
                if (dbc_decode_ANDROID_LOCATION(&locate_msg,can_msg.data.bytes, &can_header))
                {
                    if(locate_msg.ANDROID_CMD_isLast)
                    {
                        flag_next = false;
                    }else
                    {
                        flag_next = true;
                    }
                }
                break;
            case 150:
                    /*Sonar Priorities are higher than LIDAR as LIDAR's range will be larger*/
                    if (dbc_decode_SENSOR_DATA(&sensor_msg, can_msg.data.bytes, &can_header))
                    {
                        if(sensor_msg.SONAR_left < 100)
                        {
                            LD.clear();
                            LD.setNumber(sensor_msg.SONAR_left);
                        }else
                        {
                            LD.clear();
                            LD.setNumber(99);
                        }
                        if(sensor_msg.SONAR_left > 14 && sensor_msg.SONAR_left <= sonar_critical)
                        {
                            stop_lidar(sensor_msg);
                        }
                        if(sensor_msg.SONAR_left > 14 && sensor_msg.SONAR_left <= sonar_warning)
                        {
                            sensor_msg.LIDAR_0 = 1;
                        }
                        pair<uint8_t , uint8_t> son;
                        son.first = CENTER;
                        if (flag_fix || flag_free_run)
                        {
                            son = update_lanes(sensor_msg);
                        }
                        master_motor_msg.CAR_CONTROL_steer = son.first;
                        master_motor_msg.CAR_CONTROL_speed = son.second;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                break;
            case 170:
                if(flag_navigation)
                {
                    if (dbc_decode_COMPASS(&compass_msg, can_msg.data.bytes,&can_header))
                    {
                        if(!reached_destination(compass_msg))
                        {
                            //pair<uint8_t, uint8_t> son;
                            //son = correct_guidance(compass_msg);
                            //master_motor_msg.CAR_CONTROL_steer = son.first;
                            //master_motor_msg.CAR_CONTROL_speed = son.second;
                            //dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                            correct_guidance(compass_msg);
                        }else
                        {
                            flag_speed = 0;
                        }
                    }
                }
                break;
            case 160:
                if(flag_navigation)
                {
                    if(dbc_decode_GPS_DATA(&gps_msg, can_msg.data.bytes, &can_header))
                    {
                        if(gps_msg.GPS_FIX)
                        {
                            flag_fix = true;
                            flag_speed = 1;
                        }else
                        {
                            flag_fix = false;
                            flag_speed = 0;
                        }
                    }
                }
                break;

        }
    }

    dbc_handle_mia_SENSOR_DATA(&sensor_msg, 100);
}

void period_100Hz(uint32_t count)
{
    //LE.toggle(3);
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
    //LE.toggle(4);
}
