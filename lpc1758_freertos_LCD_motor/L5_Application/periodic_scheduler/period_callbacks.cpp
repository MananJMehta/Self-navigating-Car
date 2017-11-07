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
#include <stdio.h>
#include "eint.h"
#include "gpio.hpp"
#include "L4.5_Motor_Control/Steering.hpp"
#include "L4.5_Motor_Control/Speed.hpp"
#include "_can_dbc/generated_can.h"
#include "can.h"
#include "printf_lib.h"
#include "eint.h"
#include "string.h"

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3);

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}

Speed spd;
Steering str;
float val = SLOW;
bool flag=false;
MOTOR_TELEMETRY_t telemetry;
/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */

void callBack()
{
    spd.rpm_s.cut_count++;
    LE.toggle(1);
}

void initialize_motor_feedback()
{
    eint3_enable_port2(5,eint_rising_edge,callBack);
}


/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    initialize_motor_feedback();
    CAN_init(can1,100,10,10,NULL,NULL);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);
    LD.init();
    //error code 1 to signal incorrect initialization of speed PWM
    if(!spd.init())
        LD.setNumber(1);
    //error code 2 to signal incorrect initialization of steering PWM
    if(!str.init())
        LD.setNumber(2);
    spd.setSpeed(STOP);
    str.setDirection(CENTER); //Aditya : this part might be extra/ already set in init
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

void check_bus()
{
    if(CAN_is_bus_off(can1))
        CAN_reset_bus(can1);
}



void period_1Hz(uint32_t count)
{
    telemetry.MOTOR_TELEMETRY_kph= spd.rpm_meter();
    printf("%f\n",telemetry.MOTOR_TELEMETRY_kph);
    check_bus();
}

const uint32_t        CAR_CONTROL__MIA_MS=3000;
const CAR_CONTROL_t   CAR_CONTROL__MIA_MSG={0};
can_msg_t msg;
CAR_CONTROL_t carControl;

HEARTBEAT_t heartbeat;

void period_10Hz(uint32_t count)
{
    if(count%2==0)
    val=spd.speed_check(flag,val);
    printf("%f\n",spd.getSpeed());
    if(flag==false)
        spd.setSpeed(STOP);

    else if(flag==true)
        spd.setSpeed(val);

    if(SW.getSwitch(1)==true)
        flag=true;
    if(SW.getSwitch(2)==true)
        flag=false;
    if(SW.getSwitch(3)==true)
        val+=0.1;
    if(SW.getSwitch(4)==true)
        val-=0.1;


    while(CAN_rx(can1,&msg,0))
    {
        LE.off(4);
        dbc_msg_hdr_t header;
        header.dlc = msg.frame_fields.data_len;
        header.mid = msg.msg_id;
        switch(header.mid)
        {
            case 120:
                dbc_decode_HEARTBEAT(&heartbeat,msg.data.bytes,&header);
                //LE.toggle(2);
                break;
            case 140:
                dbc_decode_CAR_CONTROL(&carControl,msg.data.bytes,&header);
                // LE.toggle(3);
                break;
        }
    }

    telemetry.MOTOR_TELEMETRY_pwm=val;
    dbc_encode_and_send_MOTOR_TELEMETRY(&telemetry);

    if(dbc_handle_mia_CAR_CONTROL(&carControl,100))
        LE.on(1);
    else LE.off(1);

    //    if(SW.getSwitch(3))
    //        str.setDirection(HARDRIGHT);
    //    else if(SW.getSwitch(4))
    //        str.setDirection(HARDLEFT);
    //else
    if(carControl.CAR_CONTROL_steer==8)
        str.setDirection(HARDRIGHT);
    else if(carControl.CAR_CONTROL_steer==2)
        str.setDirection(HARDLEFT);
    else
        str.setDirection(CENTER);
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
