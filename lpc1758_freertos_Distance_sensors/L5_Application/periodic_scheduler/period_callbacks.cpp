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
#include "lidar_sensor.hpp"
#include "lidar_sensor.h"
#include "tasks.hpp"
#include "stdio.h"


#define rplidar  Lidar_Sensor::getInstance()   ///< Temperature Sensor


/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3);


TaskHandle_t scanHandle = NULL;//use this flow meter handle to turn flow meter on and off
TaskHandle_t receiveHandle = NULL;//use this flow meter handle to turn flow meter on and off



void scan_task(void* p)
{
    bool flag = true;
//    health_data_packet_t health_packet;
//    info_data_packet_t info_packet;
//    sample_rate_packet_t sample_packet;

//     while(flag)
//    {
         if(flag)
         {
//             rplidar.get_health(&health_packet);
//             vTaskDelay(2);
//             rplidar.get_info(&info_packet);
//             vTaskDelay(4);
//             rplidar.get_info(&info_packet);
//             vTaskDelay(4);
//             rplidar.get_sample_rate(&sample_packet);
//             vTaskDelay(1);
//             rplidar.get_info(&info_packet);
//             vTaskDelay(4);
//             rplidar.get_info(&info_packet);
//             vTaskDelay(4);
//             rplidar.get_sample_rate(&sample_packet);
//             vTaskDelay(2);
             rplidar.stop_scan();
             vTaskDelay(1);
             rplidar.start_scan();
//             vTaskDelay(5000);
//             flag = false;
             //start scan
         }

         else
         {
//             rplidar.stop_scan();
//             vTaskDelay(1000);
//             flag = true;
             //stop scan
         }
//    }

}


//this task can be turnned on and off every time there is a
//start and stop command...so in the rplidar.start_scan function enable this task
//in rplidar.stop_scan(); turn off this task
void receive_task(void* p)
{

//    bool starting_scan = true;
//    char arr[7]={ 0xa5 , 0x5a , 0x05 , 0x00, 0x00 , 0x40 , 0x81};
//    uint16_t temp;
//    uint16_t temp1;
//    uint16_t angle;
//    uint16_t distance;
//    float angle_q6;
//    float distance_q6;
//
//    starting_scan = rplidar.check_start_scan();
//
//    while(starting_scan)
//    {
//        uint32_t lookup[9]={ 280 , 300 , 320 , 340 , 0 , 20 , 40 , 60 , 80 };
//
//        for(uint16_t i = 0; i < 360; i++)
//        {
//            uint8_t count=0;
//
//        rplidar.receive_lidar_data();// get da quality
//        angle = (uint16_t)(rplidar.receive_lidar_data()); //get da angle_1
//
//        temp = (uint16_t)(rplidar.receive_lidar_data()); //get da angle_2
//
//        distance = (uint16_t)(rplidar.receive_lidar_data()); //get da distance_1
//        temp1 = (uint16_t)(rplidar.receive_lidar_data()); //get da distance 2
//
//        if(lookup[count]==i){
//            angle = angle>>1;
//            angle |= temp<<8;
//            angle_q6 = angle/64.0;
//            distance |= temp1<<8;
//            distance_q6 = distance/4.0;
//            count++;
//            if(distance_q6 < 0.5)
//                rplidar.lane_lut[count]=true;
//            else
//                rplidar.lane_lut[count]=false;
//        }
//        }
//
//
//        //get the angle
//        //determine the lane
//        //check if there is there is obstacle
//        //set lane bit accordingly
//    }

    bool scanning = false;

    while (1)
    {
        if(!scanning) rplidar.check_start_scan();

    }

}




/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    #if 0
    xTaskCreate(scan_task, (const char*)"scan", STACK_BYTES(2048), 0, PRIORITY_HIGH, &scanHandle);
    #endif

    #if 0
    xTaskCreate(receive_task, (const char*)"recv", STACK_BYTES(2048), 0, PRIORITY_HIGH, &receiveHandle);
    #endif
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
//    printf("\n%d",rplidar.det_smol_angle());

    LE.toggle(1);
}

void period_10Hz(uint32_t count)
{
//    LE.toggle(2);
}

void period_100Hz(uint32_t count)
{
    static bool lidar_ready_to_send = false;
    static bool sent_scan_start = false;


    //wait 2 seconds then start scan
    if (!sent_scan_start && count==200)
    {
        rplidar.start_scan();
        sent_scan_start = true;
    }
    //if scan start is sent look for the start scan sequence from lidar
    else if (sent_scan_start)
    {
        if(rplidar.check_start_scan())
        {
            lidar_ready_to_send = true;
        }
    }
    //if lidar is ready to send then toggle LED
    else if (lidar_ready_to_send)
    {
        LE.toggle(3);
    }

}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
//    LE.toggle(4);
}
