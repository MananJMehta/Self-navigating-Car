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
 * @brief This is the application entry point.
 *          FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 *          @see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 */
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "stdio.h"
#include "lidar_sensor.hpp"
#include "io.hpp"
#include "printf_lib.h"
/**
 * The main() creates tasks or "threads".  See the documentation of scheduler_task class at scheduler_task.hpp
 * for details.  There is a very simple example towards the beginning of this class's declaration.
 *
 * @warning SPI #1 bus usage notes (interfaced to SD & Flash):
 *      - You can read/write files from multiple tasks because it automatically goes through SPI semaphore.
 *      - If you are going to use the SPI Bus in a FreeRTOS task, you need to use the API at L4_IO/fat/spi_sem.h
 *
 * @warning SPI #0 usage notes (Nordic wireless)
 *      - This bus is more tricky to use because if FreeRTOS is not running, the RIT interrupt may use the bus.
 *      - If FreeRTOS is running, then wireless task may use it.
 *        In either case, you should avoid using this bus or interfacing to external components because
 *        there is no semaphore configured for this bus and it should be used exclusively by nordic wireless.
 */

class lidar_data_acquisition : public scheduler_task
{
    public:
        lidar_data_acquisition(uint8_t priority) : scheduler_task("lidar", 2000, priority){}
        bool init();
        bool run(void* p);
};

bool lidar_data_acquisition::init()
{
    //rplidar.check_start_scan();
    rplidar.start_scan();
    //printf("init\n");
    return true;
}

bool lidar_data_acquisition::run(void* p)
{
    while(1){
    if(!rplidar.check_start)
    {
        LE.on(1);
        LE.on(2);
        LE.on(3);
        LE.on(4);
        rplidar.check_start_scan();
        return true;
    }

    else
    {
        LE.off(1);
        LE.off(2);
        LE.off(3);
        LE.off(4);
    }

    uint16_t temp;
    uint16_t temp1;
    uint16_t angle;
    uint16_t distance;
    float angle_q6;
    float distance_q6;

    //float lookup[9]={ 0.0 , 20.0 , 40.0 , 60.0 , 80.0 , 280.0 , 300.0 , 320.0 , 340.0};
    //uint32_t lookup[9]={280 , 300 , 320 , 340 ,  0 , 20 , 40 , 60 , 80};
    float lookup[27]={275.0 , 280.0 , 285.0 , 295.0 , 300.0 , 305.0 , 315.0 , 320.0 , 325.0 , 335.0 , 340.0 , 345.0 , 355.0 ,  0.0 , 5.0 , 15.0 , 20.0 , 25.0 , 35.0 , 40.0 , 45.0 , 55.0 , 60.0 , 65.0 , 75.0 , 80.0 , 85.0};

    static uint8_t count_for_lut_in_this_function=0;
    static uint8_t count_that_checks_threshold=0;
    static uint8_t count_for_rplidar_lut=0;
    static bool lane_lut_in_this_function[27];
//    for(uint32_t i = 0; i < 360; i++)
//    {

        rplidar.receive_lidar_data();// get da quality

        angle = (uint16_t)(rplidar.receive_lidar_data()); //get da angle_1

        temp = (uint16_t)(rplidar.receive_lidar_data()); //get da angle_2

        distance = (uint16_t)(rplidar.receive_lidar_data()); //get da distance_1
        temp1 = (uint16_t)(rplidar.receive_lidar_data()); //get da distance 2

        angle = angle>>1;
        angle |= temp<<7;
        angle_q6 = (float)(angle)/64.0;
        distance |= temp1<<8;
        distance_q6 = (float)(distance)/4.0;

        if(abs(lookup[count_for_lut_in_this_function]-angle_q6) <= 1)
        {
            if(distance_q6 <= 20.0)
                rplidar.lane_lut[count_for_rplidar_lut] = false;
            else
                rplidar.lane_lut[count_for_rplidar_lut] = true;
            count_that_checks_threshold++;
            count_for_lut_in_this_function++;
            if(count_that_checks_threshold == 3)
                count_for_rplidar_lut++;
        }

        count_for_lut_in_this_function=count_for_lut_in_this_function > 26 ? 0 :count_for_lut_in_this_function;
        count_for_rplidar_lut=count_for_rplidar_lut > 8 ? 0 : count_for_rplidar_lut;

    }
//    }



    //get the angle
    //determine the lane
    //check if there is there is obstacle
    //set lane bit accordingly
    return true;
}

int main(void)
{
    /**
     * A few basic tasks for this bare-bone system :
     *      1.  Terminal task provides gateway to interact with the board through UART terminal.
     *      2.  Remote task allows you to use remote control to interact with the board.
     *      3.  Wireless task responsible to receive, retry, and handle mesh network.
     *
     * Disable remote task if you are not using it.  Also, it needs SYS_CFG_ENABLE_TLM
     * such that it can save remote control codes to non-volatile memory.  IR remote
     * control codes can be learned by typing the "learn" terminal command.
     */
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

    /* Consumes very little CPU, but need highest priority to handle mesh network ACKs */
    scheduler_add_task(new wirelessTask(PRIORITY_CRITICAL));

    scheduler_add_task(new lidar_data_acquisition(PRIORITY_CRITICAL));

    /* Change "#if 0" to "#if 1" to run period tasks; @see period_callbacks.cpp */
    #if 1
    const bool run_1Khz = false;
    scheduler_add_task(new periodicSchedulerTask(run_1Khz));
    #endif

    /* The task for the IR receiver to "learn" IR codes */
    // scheduler_add_task(new remoteTask  (PRIORITY_LOW));

    /* Your tasks should probably used PRIORITY_MEDIUM or PRIORITY_LOW because you want the terminal
     * task to always be responsive so you can poke around in case something goes wrong.
     */

    /**
     * This is a the board demonstration task that can be used to test the board.
     * This also shows you how to send a wireless packets to other boards.
     */
    #if 0
        scheduler_add_task(new example_io_demo());
    #endif

    /**
     * Change "#if 0" to "#if 1" to enable examples.
     * Try these examples one at a time.
     */
    #if 0
        scheduler_add_task(new example_task());
        scheduler_add_task(new example_alarm());
        scheduler_add_task(new example_logger_qset());
        scheduler_add_task(new example_nv_vars());
    #endif

    /**
     * Try the rx / tx tasks together to see how they queue data to each other.
     */
    #if 0
        scheduler_add_task(new queue_tx());
        scheduler_add_task(new queue_rx());
    #endif

    /**
     * Another example of shared handles and producer/consumer using a queue.
     * In this example, producer will produce as fast as the consumer can consume.
     */
    #if 0
        scheduler_add_task(new producer());
        scheduler_add_task(new consumer());
    #endif

    /**
     * If you have RN-XV on your board, you can connect to Wifi using this task.
     * This does two things for us:
     *   1.  The task allows us to perform HTTP web requests (@see wifiTask)
     *   2.  Terminal task can accept commands from TCP/IP through Wifly module.
     *
     * To add terminal command channel, add this at terminal.cpp :: taskEntry() function:
     * @code
     *     // Assuming Wifly is on Uart3
     *     addCommandChannel(Uart3::getInstance(), false);
     * @endcode
     */
    #if 0
        Uart3 &u3 = Uart3::getInstance();
        u3.init(WIFI_BAUD_RATE, WIFI_RXQ_SIZE, WIFI_TXQ_SIZE);
        scheduler_add_task(new wifiTask(Uart3::getInstance(), PRIORITY_LOW));
    #endif

    scheduler_start(); ///< This shouldn't return
    return -1;
}
