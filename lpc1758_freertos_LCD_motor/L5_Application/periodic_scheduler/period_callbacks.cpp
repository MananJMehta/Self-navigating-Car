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
#include "L4.5_Motor_Control/Steering.hpp"
#include "L4.5_Motor_Control/Speed.hpp"
#include "_can_dbc/generated_can.h"
#include "can.h"
#include "printf_lib.h"

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);
Speed spd;
Steering str;

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

    CAN_init(can1,100,10,10,NULL,NULL);
    //error code 1 to signal incorrect initialization of speed PWM
    if(!spd.init())
        LD.setNumber(1);
    //error code 2 to signal incorrect initialization of steering PWM
    if(!str.init())
        LD.setNumber(2);
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
        LE.init();
        LE.on(4);
    }
}
const uint32_t        CAR_CONTROL__MIA_MS=3000;
const CAR_CONTROL_t   CAR_CONTROL__MIA_MSG={0};
can_msg_t msg;
CAR_CONTROL_t carControl;
HEARTBEAT_t heartbeat;
void period_10Hz(uint32_t count)
{
    spd.setSpeed(Speed::speedOfCar::MEDIUM);



    while(CAN_rx(can1,&msg,0))
    {
        dbc_msg_hdr_t header;
        header.dlc = msg.frame_fields.data_len;
        header.mid = msg.msg_id;
        switch(header.mid)
        {
            case 120:
                dbc_decode_HEARTBEAT(&heartbeat,msg.data.bytes,&header);
                LE.toggle(2);
                break;
            case 140:
                dbc_decode_CAR_CONTROL(&carControl,msg.data.bytes,&header);
            LE.toggle(3);
                break;
        }
    }

    if(dbc_handle_mia_CAR_CONTROL(&carControl,100))
        LE.on(1);
    else LE.off(1);

    str.setDirection((Steering::directionOfCar)carControl.CAR_CONTROL_steer);
    u0_dbg_printf("%i\n",carControl.CAR_CONTROL_steer);
}

void period_100Hz(uint32_t count)
{

}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{

}
