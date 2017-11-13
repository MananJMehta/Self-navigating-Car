/*
b *     SocialLedge.com - Copyright (C) 2013
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
#include "io.hpp"
#include "printf_lib.h"

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

// Enum for Sonar Status
enum {
    sonar_safe = 0,     //No Obstacle
    sonar_alert = 1,    //Obstacle at medium distance
    sonar_critical = 2  //Obstacle critically near
};


// Enum for Motor Steer
//enum {
//    ExtremeLeft,
//    ExtremeRight,
//    HardLeft,
//    Left,
//    SoftLeft,
//    Center,
//    SoftRight,
//    Right,
//    HardRight
//};

#define HARDLEFT 12
#define LEFT 13
#define SOFTLEFT 14
#define CENTER 15
#define SOFTRIGHT 16
#define RIGHT 17
#define HARDRIGHT 18
#define SONAR_CRITICAL 60
#define SONAR_WARNING 100

// Enum for Motor Speed
enum {
    stop,
    maintain,     //slowest
    Reverse
};

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3);

can_t canTest = can1;

SENSOR_DATA_t sensor_msg = { 0 };
CAR_CONTROL_t master_motor_msg = { 0 };
HEARTBEAT_t heartbeat_msg;

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
    return true; // Must return true upon success
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

void check_can_bus()
{
    if(CAN_is_bus_off(can1))
    {
        CAN_reset_bus(can1);
    }
}

void period_1Hz(uint32_t count)
{
    heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_SYNC;
    dbc_encode_and_send_HEARTBEAT(&heartbeat_msg);
    LE.toggle(1);
    check_can_bus();
}

void period_10Hz(uint32_t count)
{
    can_msg_t can_msg;
    while(CAN_rx(canTest,&can_msg,0))
    {
        dbc_msg_hdr_t can_header;
        can_header.dlc = can_msg.frame_fields.data_len;
        can_header.mid = can_msg.msg_id;
        u0_dbg_printf("in while");
        switch(can_msg.msg_id)
        {
            case 150:
                /*Sonar Priorities are higher than LIDAR as LIDAR's range will be larger*/
                if (dbc_decode_SENSOR_DATA(&sensor_msg, can_msg.data.bytes, &can_header))
                {
#if 1
                    if (sensor_msg.SONAR_left <= SONAR_CRITICAL && sensor_msg.SONAR_right <= SONAR_CRITICAL)
                    {
                        master_motor_msg.CAR_CONTROL_speed = stop;
                        master_motor_msg.CAR_CONTROL_steer = CENTER;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
//                    if(sensor_msg.SONAR_left <= SONAR_WARNING && sensor_msg.SONAR_right <= SONAR_WARNING)
//                    {
//                        sensor_msg.LIDAR_0 = 1;
//                        sensor_msg.LIDAR_20 = 1;
//                        sensor_msg.LIDAR_neg20 = 1;
//                    }
                    else if (sensor_msg.SONAR_left <= SONAR_WARNING)
                    {
                        master_motor_msg.CAR_CONTROL_speed = maintain;
                        master_motor_msg.CAR_CONTROL_steer = RIGHT;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                    else if (sensor_msg.SONAR_right <= SONAR_WARNING)
                    {
                        master_motor_msg.CAR_CONTROL_speed = maintain;
                        master_motor_msg.CAR_CONTROL_steer = LEFT;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
#endif
#if 0
                    if (sensor_msg.SONAR_left <= SONAR_CRITICAL && sensor_msg.SONAR_right <= SONAR_CRITICAL)
                    {
                        master_motor_msg.CAR_CONTROL_speed = stop;
                        master_motor_msg.CAR_CONTROL_steer = CENTER;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                    if(sensor_msg.SONAR_left <= SONAR_WARNING && sensor_msg.SONAR_right <= SONAR_WARNING)
                    {
                        sensor_msg.LIDAR_0 = 1;
                        sensor_msg.LIDAR_20 = 1;
                        sensor_msg.LIDAR_neg20 = 1;
                    }
                    else if (sensor_msg.SONAR_left <= SONAR_CRITICAL)
                    {
                        sensor_msg.LIDAR_neg20 = 1;
                    }
                    else if (sensor_msg.SONAR_right <= SONAR_CRITICAL)
                    {
                        sensor_msg.LIDAR_20 = 1;
                    }
#endif

                    //If any of LIDAR right values set, take HardLeft
                    //else if (sensor_msg.LIDAR_0 || sensor_msg.LIDAR_20 || sensor_msg.LIDAR_40 || sensor_msg.LIDAR_60 || sensor_msg.LIDAR_80)
                    if((sensor_msg.LIDAR_0 && sensor_msg.LIDAR_20 && sensor_msg.LIDAR_neg20 && sensor_msg.LIDAR_40 && sensor_msg.LIDAR_neg40))
                    {
                        master_motor_msg.CAR_CONTROL_steer = CENTER;
                        master_motor_msg.CAR_CONTROL_speed = stop;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                    else if (sensor_msg.LIDAR_40 || sensor_msg.LIDAR_20)
                    {
                        master_motor_msg.CAR_CONTROL_steer = HARDLEFT;
                        master_motor_msg.CAR_CONTROL_speed = maintain;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                    //If any of LIDAR left values set, take HardRight
                    else if (sensor_msg.LIDAR_neg40 || sensor_msg.LIDAR_neg20)
                    {
                        master_motor_msg.CAR_CONTROL_steer = HARDRIGHT;
                        master_motor_msg.CAR_CONTROL_speed = maintain;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                    //Go Straight
                    else if((sensor_msg.LIDAR_0 && sensor_msg.LIDAR_20) && !(sensor_msg.LIDAR_neg20))
                    {
                        master_motor_msg.CAR_CONTROL_steer = HARDLEFT;
                        master_motor_msg.CAR_CONTROL_speed = maintain;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                    else if((sensor_msg.LIDAR_0 && sensor_msg.LIDAR_neg20) && !(sensor_msg.LIDAR_20))
                    {
                        master_motor_msg.CAR_CONTROL_steer = HARDRIGHT;
                        master_motor_msg.CAR_CONTROL_speed = maintain;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                    else if(!sensor_msg.LIDAR_0)
                    {
                        master_motor_msg.CAR_CONTROL_steer = CENTER;
                        master_motor_msg.CAR_CONTROL_speed = maintain;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                    else if(sensor_msg.LIDAR_0)
                    {
                        if((sensor_msg.LIDAR_20 || sensor_msg.LIDAR_40) && !(sensor_msg.LIDAR_neg20))
                        {
                            master_motor_msg.CAR_CONTROL_steer = LEFT;
                            master_motor_msg.CAR_CONTROL_speed = maintain;
                            dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                        }
                        else
                            if((sensor_msg.LIDAR_neg20 || sensor_msg.LIDAR_neg40) && !(sensor_msg.LIDAR_20))
                            {
                                master_motor_msg.CAR_CONTROL_steer = RIGHT;
                                master_motor_msg.CAR_CONTROL_speed = maintain;
                                dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                            }
                            else
                            {
                                master_motor_msg.CAR_CONTROL_steer = HARDLEFT;
                                master_motor_msg.CAR_CONTROL_speed = maintain;
                                dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                            }
                    }
                    else
                    {
                        master_motor_msg.CAR_CONTROL_steer = CENTER;
                        master_motor_msg.CAR_CONTROL_speed = stop;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                }
                break;
        }
    }
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
