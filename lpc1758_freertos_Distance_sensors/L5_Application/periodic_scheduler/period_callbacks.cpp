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

#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "sensor_functions.hpp"
#include "tasks.hpp"
#include "stdio.h"


Sonar_Sensor sonar;
SemaphoreHandle_t sonar_mutex;




/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3);

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    sonar_mutex = xSemaphoreCreateMutex();
    sonar.init();

    initialize_can();

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

void period_1Hz(uint32_t count)
{
    //


    if(CAN_is_bus_off(can1))
        CAN_reset_bus(can1);
//    send_distance_values();

        static bool start_flag = false;
        //if uart is not active send stop scan folowed by start scan
        if(start_flag)
        {
            rplidar.start_scan();
            start_flag = false;
        }
        if(!rplidar.uart_active())
        {
            rplidar.reset_core();
            start_flag = true;
        }
}

void period_10Hz(uint32_t count)
{
    static uint32_t prev_count = count;
    rplidar.flag = true;

    check_heartbeat();

//    Turn this on if you wanna see lane distance values
    #if 0
    send_lane_distance_values();
    #endif



    if(xSemaphoreTake(sonar_mutex,1))
    {
        sonar.start_operation();
    }


    if ((count-prev_count) == 1)
    {
        send_lidar_sonar_data();
        prev_count = count;
    }

}

void period_100Hz(uint32_t count)
{

}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
//    static uint32_t prev_count = count;
////    static LIDAR_DATA_VALUES_t Data;
//
//    if ((count-prev_count)%3 == 0)
//    {
////        add_some_data_to_msg(&Data);
//    }
//    if ((count-prev_count) == 9)
//    {
////        send_three_values(&Data);
//        prev_count = count;
//    }

}


