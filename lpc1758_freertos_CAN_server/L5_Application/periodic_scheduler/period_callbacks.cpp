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
#include "string"
#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "can.h"
#include "_can_dbc/generated_can.h"
#include "printf_lib.h"
#include "map"
#include <cmath>
#include <utility>

using namespace std;

#define SONAR_CODE  0        //Uncomment to include SONAR code
//#define SONAR_ALERT         //Uncomment to include SONAR_ALERT code

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);


// Enum for Sonar Status
enum {
    sonar_safe = 0,     //No Obstacle
    sonar_alert = 1,    //Obstacle at medium distance
    sonar_critical = 2  //Obstacle critically near
};


// Enum for Motor Steer
enum {
    ExtremeLeft,
    ExtremeRight,
    HardLeft,
    Left,
    SoftLeft,
    Center,
    SoftRight,
    Right,
    HardRight
};


// Enum for Motor Speed
enum {
    Forward_L1,     //medium
    Forward_L2,     //fastest
    Forward_L3,     //slowest
    Stop,
    Reverse
};

map<char, int8_t> m;
map<char, uint8_t> m_dir;
map<int8_t, string> m_update_lanes;
char arr[9];

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

    //array initialized to ensure the car goes straight
    arr[0]='c'; //center
    arr[1]='r'; //soft right
    arr[2]='l'; //soft left
    arr[3]='R'; //right
    arr[4]='L'; //left
    arr[5]='H'; //hard right
    arr[6]='I'; //hard left
    arr[7]='E'; //extreme right
    arr[8]='F'; //extreme left

    //std::map for steering control
    m_dir['c']=Center;//center
    m_dir['r']=HardRight;//soft right
    m_dir['R']=HardRight;//right
    m_dir['H']=HardRight;//hard right
    m_dir['E']=HardRight;//extreme right
    m_dir['l']=HardLeft;//soft left
    m_dir['L']=HardLeft;//left
    m_dir['I']=HardLeft;//hard left
    m_dir['F']=HardLeft;//extreme left
    m_dir['s']=Center;



    return true; // Must return true upon success
}

//reads data from the CAN message based on x
//@params label x, CAN message y
//@returns x->y.__
int8_t map_get_value(char x, SENSOR_DATA_t y)
{
    if(x=='c')
        return y.LIDAR_0;
    else if (x=='r')
        return y.LIDAR_20;
    else if (x=='R')
        return y.LIDAR_40;
    else if (x=='H')
        return y.LIDAR_60;
    else if (x=='E')
        return y.LIDAR_80;
    else if (x=='l')
        return y.LIDAR_neg20;
    else if (x=='L')
        return y.LIDAR_neg40;
    else if (x=='I')
        return y.LIDAR_neg60;
    return y.LIDAR_neg80;
}

//updates the lanes to look out for after turning
void update_map_lane(int8_t correction, bool sign)
{
    string local_array = m_update_lanes[correction];
    for (uint8_t i=0; i<9 ; i++)
        arr[i] = local_array[i];

}


//turns the steering if an obstacle is deleted
//@params received can message from the sonar sensor
//@returns steering rotation and car speed
pair<uint8_t, uint8_t> update_lanes(SENSOR_DATA_t x)
{
    u0_dbg_printf("\n");
    uint8_t i;
    for (i=0; map_get_value(arr[i], x)== 1 && i<9; i++){}
    /*u0_dbg_printf("%d ", x.LIDAR_0);
    u0_dbg_printf("%d ", x.LIDAR_20);
    u0_dbg_printf("%d ", x.LIDAR_40);
    u0_dbg_printf("%d ", x.LIDAR_60);
    u0_dbg_printf("%d ", x.LIDAR_80);
    u0_dbg_printf("%d ", x.LIDAR_neg20);
    u0_dbg_printf("%d ", x.LIDAR_neg40);
    u0_dbg_printf("%d ", x.LIDAR_neg60);
    u0_dbg_printf("%d ", x.LIDAR_neg80);
    u0_dbg_printf("\n");
    for(uint8_t j=0 ; j<9; j++)
        u0_dbg_printf("%c",arr[j]);

    u0_dbg_printf("i %d\n",i);*/

    if(i==9)
        return make_pair(Center, Stop);

    /*u0_dbg_printf("\nUpdated\n");
    for(uint8_t j=0 ; j<9; j++)
        u0_dbg_printf("%c",arr[j]);*/

    return make_pair(m_dir[arr[i]], Forward_L1);
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
    heartbeat_msg.HEARTBEAT_cmd = HEARTBEAT_cmd_SYNC;
    dbc_encode_and_send_HEARTBEAT(&heartbeat_msg);
    LE.toggle(1);
    if(CAN_is_bus_off(can1))
    {
        CAN_reset_bus(can1);
    }
}

void period_10Hz(uint32_t count)
{
    //LE.toggle(2);
    can_msg_t can_msg;
    while(CAN_rx(canTest,&can_msg,0))
    {
        dbc_msg_hdr_t can_header;
        can_header.dlc = can_msg.frame_fields.data_len;
        can_header.mid = can_msg.msg_id;
        //u0_dbg_printf("in while");
        switch(can_msg.msg_id)
        {
            //u0_dbg_printf("in switch");
            case 150:

                /*Sonar Priorities are higher than LIDAR as LIDAR's range will be larger*/
                if (dbc_decode_SENSOR_DATA(&sensor_msg, can_msg.data.bytes, &can_header))
                {
#ifdef SONAR_CODE
                    if (sensor_msg.SONAR_left == sonar_critical)
                    {
                        LE.off(2);
                        LE.off(3);
                        LE.on(4);
                        master_motor_msg.CAR_CONTROL_steer = HardRight;
                        master_motor_msg.CAR_CONTROL_speed = Forward_L1;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                    else if (sensor_msg.SONAR_right == sonar_critical)
                    {
                        LE.off(2);
                        LE.off(4);
                        LE.on(3);
                        master_motor_msg.CAR_CONTROL_steer = HardLeft;
                        master_motor_msg.CAR_CONTROL_speed = Forward_L1;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                    //Back Sonar currently not being used for LAB2
//                    else if (sensor_msg.SONAR_back == sonar_critical)
//                    {
//                        LE.on(4);
//                        master_motor_msg.CAR_CONTROL_steer = Center;      //put stop here
//                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
//                    }
#ifdef SONAR_ALERT
                    else if (sensor_msg.SONAR_right == sonar_alert)
                    {
                        LE.off(2);
                        LE.off(4);
                        LE.on(3);
                        master_motor_msg.CAR_CONTROL_steer = SoftLeft;
                        master_motor_msg.CAR_CONTROL_speed = Forward_L1;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
                    else if (sensor_msg.SONAR_left == sonar_alert)
                    {
                        LE.off(2);
                        LE.off(3);
                        LE.on(4);
                        master_motor_msg.CAR_CONTROL_steer = SoftRight;
                        master_motor_msg.CAR_CONTROL_speed = Forward_L1;
                        dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
                    }
#endif
#endif
                    //If any of LIDAR right values set, take HardLeft
                    pair<uint8_t , uint8_t> son;
                    son = update_lanes(sensor_msg);
                    master_motor_msg.CAR_CONTROL_steer = son.first;
                    master_motor_msg.CAR_CONTROL_speed = son.second;
                    dbc_encode_and_send_CAR_CONTROL(&master_motor_msg);
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
