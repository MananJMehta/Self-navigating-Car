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
#include "gpio.hpp"
#include "stdio.h"
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
#define cep 7

uint8_t arr[9];
uint8_t flag_speed = 0;
bool boot_t = true;
bool flag_navigation = false;
bool flag_fix = false;
bool flag_next = false;
bool flag_free_run = false;
bool flag_toggle = false;
uint8_t direction = CENTER;
bool flag_cp = false;

const SENSOR_DATA_t SENSOR_DATA__MIA_MSG = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,15};
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
GPIO redLED(P2_0);
GPIO whiteLED(P2_2);
GPIO rightLED(P2_1);
GPIO leftLED(P2_3);

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
    CAN_init(canTest, 100, 50, 50, NULL, NULL);
//    const can_std_id_t slist[] = {CAN_gen_sid(canTest, 0x078), CAN_gen_sid(canTest, 0x07D), CAN_gen_sid(canTest, 0x082), CAN_gen_sid(canTest, 0x087), CAN_gen_sid(canTest, 0x096)};
//    CAN_setup_filter(slist,5,NULL,0,NULL,0,NULL,0);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(canTest);

    redLED.setAsOutput();
    whiteLED.setAsOutput();
    leftLED.setAsOutput();
    rightLED.setAsOutput();
    redLED.setHigh();
    whiteLED.setHigh();
    leftLED.setHigh();
    rightLED.setHigh();

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

//bool check_direction_free(SENSOR_DATA_t y)
//{
//    if(direction == 0  && (y.LIDAR_0 == 0 && y.LIDAR_20 == 0 && y.LIDAR_neg20 == 0 ))
//        return true;
//
//    if((direction == 1 || direction == 3) && (y.LIDAR_0 == 0 && y.LIDAR_20 == 0 && y.LIDAR_40 == 0 ))
//        return true;
//
//    if(direction == 5 && (y.LIDAR_60 == 0 && y.LIDAR_20 == 0 && y.LIDAR_40 == 0 ))
//        return true;
//
//    if((direction == 2 || direction == 4) && (y.LIDAR_0 == 0 && y.LIDAR_neg20 == 0 && y.LIDAR_neg40 == 0 ))
//        return true;
//
//    if(direction == 6 && (y.LIDAR_neg60 == 0 && y.LIDAR_neg20 == 0 && y.LIDAR_neg40 == 0 ))
//        return true;
//
//    return false;
//}

//reads data from the CAN message
//@returns the bit with highest priority
uint8_t map_get_value(SENSOR_DATA_t y)
{
//    if(check_direction_free(y))
//        return direction;
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

uint8_t takeReverse()
{
    if(!sensor_msg.LIDAR_160 && !sensor_msg.LIDAR_140)
    {
        //turn HARDright
        return HARDRIGHT;
    }else
    if(!sensor_msg.LIDAR_neg160 && !sensor_msg.LIDAR_neg140)
    {
        //turn hardleft
        return HARDLEFT;
    }else
    if(!sensor_msg.LIDAR_160)
    {
        return SOFTRIGHT;
    }else
    if(!sensor_msg.LIDAR_neg160)
    {
        return SOFTLEFT;
    }else
    if(!sensor_msg.LIDAR_180)
    {
        return CENTER;
    }

    return 99;
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
        //aditya reverse last start
        //return_value.first = CENTER;
        return_value.second = 2;
        return_value.first = takeReverse();
        if(return_value.first == 99)
        {
            return_value.second = 0;
            return_value.first = CENTER;
        }
        //aditya reverse last end
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
//        static uint32_t wait_count = 11;
//        if(flag_next && wait_count > 10)
//        {
//            request_msg.MASTER_REQUEST_cmd = 0xf;
//            dbc_encode_and_send_MASTER_REQUEST(&request_msg);
//            wait_count = 0;
//        }
//        if(!(flag_next) && (wait_count > 15))
//        {
//            //stop
//            flag_navigation = false;
//            flag_free_run = false;
//            heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_NOOP;
//            LD.clear();
//            LD.setRightDigit('H');
//            LD.setLeftDigit('H');
//        }
//        wait_count++;
        if(flag_cp)
        {
            if(flag_next)
            {
                request_msg.MASTER_REQUEST_cmd = 0xf;
                dbc_encode_and_send_MASTER_REQUEST(&request_msg);
            }
            if(!flag_next)
            {
                flag_navigation = false;
                flag_free_run = false;
                heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_NOOP;
                LD.clear();
                LD.setRightDigit('H');
                LD.setLeftDigit('H');
                redLED.setLow();
                whiteLED.setLow();
                rightLED.setLow();
                leftLED.setLow();
                flag_toggle = true;
            }
            flag_cp = false;
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

void turnIndicator(uint8_t x)
{
    if(!(sensor_msg.SONAR_front <= sonar_critical))
    {
        if(x == RIGHT || x == HARDRIGHT || x == SOFTRIGHT)
        {
            rightLED.setLow();
            leftLED.setHigh();
        }
        if(x == LEFT || x == HARDLEFT || x == SOFTLEFT)
        {
            leftLED.setLow();
            rightLED.setHigh();
        }
        if(x == CENTER)
        {
            rightLED.setHigh();
            leftLED.setHigh();
        }
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

void toggleLED()
{
    if(flag_toggle)
    {
        redLED.toggle();
        whiteLED.toggle();
        leftLED.toggle();
        rightLED.toggle();
    }
}

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

//    LD.clear();
//    LD.setNumber(LS.getPercentValue());
    if(LS.getPercentValue() < 1)
    {
        whiteLED.setLow();
    }
    if(LS.getPercentValue() > 2)
    {
        whiteLED.setHigh();
    }
    toggleLED();
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
//    if(SW.getSwitch(3))
//    {
//        master_motor_msg.CAR_CONTROL_steer = CENTER;
//        master_motor_msg.CAR_CONTROL_speed = 2;
//        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
//        LD.setLeftDigit('L');
//        LD.setRightDigit('L');
//    }
//    if(SW.getSwitch(4))
//    {
//        master_motor_msg.CAR_CONTROL_steer = CENTER;
//       master_motor_msg.CAR_CONTROL_speed = 1;
//       dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
//       LD.setLeftDigit('F');
//       LD.setRightDigit('F');
//    }





    //dbc_handle_mia_SENSOR_DATA(&sensor_msg, 100);
}

void period_100Hz(uint32_t count)
{

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
           //                        LD.clear();
           //                        LD.setNumber(99);
                                   flag_speed = 0;
                                   flag_navigation = false;
                                   flag_free_run = false;
                                   redLED.setLow();
                                   whiteLED.setHigh();
                                   leftLED.setLow();
                                   rightLED.setLow();
                                   heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_NOOP;
                               }
                               else
                               {
                                   if(and_msg.ANDROID_CMD_mode)
                                   {
           //                            LD.clear();
           //                            LD.setNumber(22);
                                       redLED.setHigh();
                                       leftLED.setHigh();
                                       rightLED.setHigh();
                                       flag_navigation = true;
                                       request_msg.MASTER_REQUEST_cmd = 0xf;
                                       dbc_encode_and_send_MASTER_REQUEST(&request_msg);
                                   }else
                                   {
           //                            LD.clear();
           //                            LD.setNumber(11);
                                       redLED.setHigh();
                                       leftLED.setHigh();
                                       rightLED.setHigh();
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
                                   //printf("got isLast\n");
                                   LD.clear();
                                   LD.setRightDigit('C');
                                   LD.setLeftDigit('L');
                                   flag_next = false;
                               }else
                               {
                                   //printf("next data\n");
                                   LD.clear();
           //                        static uint8_t cnt = 1;
                                   LD.setRightDigit('E');
                                   LD.setLeftDigit('E');
           //                        cnt++;
                                   flag_next = true;
                               }
                           }
                           break;
                       case 150:
                               /*Sonar Priorities are higher than LIDAR as LIDAR's range will be larger*/
                               if (dbc_decode_SENSOR_DATA(&sensor_msg, can_msg.data.bytes, &can_header))
                               {
           //                        if(sensor_msg.SONAR_front < 100)
           //                        {
           //                            LD.clear();
           //                            LD.setNumber(sensor_msg.SONAR_front);
           //                        }else
           //                        {
           //                            LD.clear();
           //                            LD.setNumber(99);
           //                        }
                                   if(sensor_msg.SONAR_front > 14 && sensor_msg.SONAR_front <= sonar_critical)
                                   {
                                       redLED.setLow();
                                       leftLED.setHigh();
                                       rightLED.setHigh();
                                       stop_lidar(sensor_msg);
                                   }
                                   if(sensor_msg.SONAR_front > 14 && sensor_msg.SONAR_front <= sonar_warning)
                                   {
                                       sensor_msg.LIDAR_0 = 1;
                                   }else
                                   {
                                       redLED.setHigh();
                                   }
                                   pair<uint8_t , uint8_t> son;
                                   son.first = CENTER;
                                   if (flag_fix || flag_free_run)
                                   {
                                       son = update_lanes(sensor_msg);
                                   }
                                   master_motor_msg.CAR_CONTROL_steer = son.first;
                                   master_motor_msg.CAR_CONTROL_speed = son.second;
                                   turnIndicator(master_motor_msg.CAR_CONTROL_steer);
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
                                       flag_cp = true;
                                       correct_guidance(compass_msg);
                                   }else
                                   {
           //                            if(!flag_next)
           //                            {
           ////                                flag_navigation = false;
           ////                                flag_free_run = false;
           ////                                heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_NOOP;
           ////                                LD.clear();
           ////                                LD.setRightDigit('H');
           ////                                LD.setLeftDigit('H');
           //                                //printf("Final destination reached");
           //                            }else
           //                            {
           //                                //printf("Checkpoint reached");
           //                            }

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
    //LE.toggle(3);
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
    //LE.toggle(4);
}
