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
#include "_can_dbc/generated_can.h"
#include "can.h"
#include "string.h"

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);
bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}
/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3);

/// Called once before the RTOS is , this is a good place to initialize things once
bool period_init(void)
{
    CAN_init(can1,10,10,10,NULL,NULL);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);
    LD.init();
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
    if(CAN_is_bus_off(can1))
    {
        CAN_reset_bus(can1);
    }
}

SENSOR_MESSAGE_t sensorMsg={0};
ANDROID_MESSAGE_t androidMsg={0};
GPS_MESSAGE_t gpsMsg={0};
MOTOR_MESSAGE_t motorMsg={0};

const uint32_t                             SENSOR_MESSAGE__MIA_MS=3000;
const SENSOR_MESSAGE_t                     SENSOR_MESSAGE__MIA_MSG={0};
const uint32_t                             MOTOR_MESSAGE__MIA_MS=3000;
const MOTOR_MESSAGE_t                      MOTOR_MESSAGE__MIA_MSG={0};
const uint32_t                             ANDROID_MESSAGE__MIA_MS=3000;
const ANDROID_MESSAGE_t                    ANDROID_MESSAGE__MIA_MSG={0};
const uint32_t                             GPS_MESSAGE__MIA_MS=3000;
const GPS_MESSAGE_t                        GPS_MESSAGE__MIA_MSG={0};

void period_10Hz(uint32_t count)
{
    //CANtx handling
    CAN_TEST_t masterMessage={0};
    KILL_SWITCH_t killSwitch={0};
    MOTOR_MOVEMENT_t motorMovement ={0};
    masterMessage.CAN_TEST_master=0x01;
    dbc_encode_and_send_CAN_TEST(&masterMessage);


    motorMovement.MOTOR_MOVEMENT_sig = SW.getSwitchValues();
    LD.setNumber(SW.getSwitchValues());
    dbc_encode_and_send_MOTOR_MOVEMENT(&motorMovement);


    if(LS.getPercentValue() >=75)
    {
        killSwitch.KILL_SWITCH_Sig=1;
        dbc_encode_and_send_KILL_SWITCH(&killSwitch);
    }
    dbc_msg_hdr_t message_header;
    can_msg_t can_msg;

    //CANrx Handling
    while(CAN_rx(can1,&can_msg,0))
    {
        message_header.dlc=can_msg.frame_fields.data_len;
        message_header.mid=can_msg.msg_id;


        switch(message_header.mid)
        {
            case 300:
                LE.toggle(1);
                break;
            case 301:
                LE.toggle(2);
                break;
            case 302:
                LE.toggle(3);
                break;
            case 303:
                LE.toggle(4);
                break;
        }
    }

    //MIA handling
    dbc_handle_mia_ANDROID_MESSAGE(&androidMsg,100);
    dbc_handle_mia_GPS_MESSAGE(&gpsMsg,100);
    dbc_handle_mia_MOTOR_MESSAGE(&motorMsg,100);
    dbc_handle_mia_SENSOR_MESSAGE(&sensorMsg,100);
    dbc_decode_SENSOR_MESSAGE(&sensorMsg,can_msg.data.bytes,&message_header);

}

void period_100Hz(uint32_t count)
{

}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{

}
