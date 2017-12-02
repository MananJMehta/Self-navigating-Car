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
#include "lpc_pwm.hpp"


PWM pwm(PWM::pwm2,8);
float val = 14;
uint32_t tmp = 0;
bool flag = false;

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
    pwm.set(14);
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
    //LE.toggle(1);

}

void period_10Hz(uint32_t count)
{
    //LE.toggle(2);
    if(SW.getSwitch(3))
    {
        flag = false;
        val = 14;
        pwm.set(val);
        LD.clear();
        LD.setRightDigit('S');
    }else
    if(SW.getSwitch(1))
    {
//        flag = false;
//        val = val + 0.5;
//        if(val > 18)
//        {
//            val = 14;
//        }
//        pwm.set(val);
//        LD.clear();
//        LD.setNumber(val);
        flag = false;
        val = 15.5;
        pwm.set(val);
        LD.clear();
        LD.setRightDigit('F');
    }else
    if(SW.getSwitch(2))
    {
        flag = false;
        val = val + 0.5;
        if(val > 18)
        {
            val = 14;
        }
        pwm.set(val);
        LD.clear();
        LD.setNumber(val);
//        flag = false;
//        val = val - 1;
//        if(val < 3)
//        {
//            val = 14;
//        }
//        pwm.set(val);
//        LD.clear();
//        LD.setNumber(val);
    }else
    if(SW.getSwitch(4))
    {
        flag = true;
        tmp = count;
    }

    if(flag)
    {
        if(count - tmp > 5)
        {
            flag = false;
        }
        if(tmp == count)
        {
            if(val > 14)
            {
                val = 14;
                pwm.set(val);
                //LD.setNumber(val);
            }
        }
        if((count - tmp) == 1)
        {
            val = 12.6;
            pwm.set(val);
            //LD.setNumber(val);
        }
        if((count - tmp) == 2)
        {
            val = 14;
            pwm.set(val);
            //LD.setNumber(val);
        }
        if((count - tmp) == 3)
        {
            val = 12.6;
            pwm.set(val);
            //LD.setNumber(val);
            LD.clear();
            LD.setRightDigit('B');
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
