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
#include <stdio.h>
#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "uart2.hpp"
#include "can.h"
#include "eint.h"
#include "gpio.hpp"
#include "L4.5_Motor_Control/Steering.hpp"
#include "L4.5_Motor_Control/Speed.hpp"
#include "lcd.hpp"
#include "printf_lib.h"
#include "eint.h"
#include "string.h"

#define LCD
/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3); ///check

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}
bool lcdflag = false;
Speed spd;
Steering str;
float val = SLOW;
int flag=false;

can_msg_t msg;
CAR_CONTROL_t carControl;
SENSOR_DATA_t sensor_can_msg;
COMPASS_t compass_can_msg;
GPS_DATA_t gps_can_msg;
HEARTBEAT_t heartbeat;
ANDROID_CMD_t androidcmd;
MOTOR_TELEMETRY_t telemetry;
ANDROID_LOCATION_t androidLocation;

void callBack()
{
    spd.rpm_s.cut_count++;
    LE.toggle(2);
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
    //error code 2 to signal incorrect initialization of steering PWM
    if(!str.init())
        LD.setNumber(2);
    spd.setSpeed(STOP);

#ifdef LCD
    Uart2& u2 = Uart2::getInstance();
    u2.init(115200);
#endif

    return true; // Must return true upon success

}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}

void check_bus()
{
    if(CAN_is_bus_off(can1))
    {
        CAN_reset_bus(can1);
        if(lcdflag)
            display_bus_reset();
    }
}




void period_1Hz(uint32_t count)
{
    telemetry.MOTOR_TELEMETRY_kph= spd.rpm_meter();
    printf("speed:%i\n", telemetry.MOTOR_TELEMETRY_kph);
    check_bus();
#ifdef LCD
    int lcdswitch = SW.getSwitchValues();
    if(lcdswitch == 1)
        lcdflag = 1;
    else if (lcdswitch == 2)
        lcdflag=false;
    /**
     * Check which form (Page) of LCD is active
     * Display the information of the Active LCD Page
     */
    if(lcdflag){
        char form = check_form();

        switch(form)
        {
            case Main_page:
                update_LCD_main_page(telemetry.MOTOR_TELEMETRY_kph,androidcmd.ANDROID_CMD_mode);
                break;
            case Distance_page:
                update_LCD_distance_page(  (spd.rpm_s.cut_count*12.12),compass_can_msg.DISTANCE_CHECKPOINT);
                break;
            case Sensor_page:
                update_LCD_sensor_page(sensor_can_msg);
                break;
            case GPS_page:
                update_LCD_GPS_page(gps_can_msg, androidLocation);
                break;
            case Compass_page:
                update_Compass_page(compass_can_msg.CMP_HEADING, compass_can_msg.CMP_BEARING, compass_can_msg.DEFLECTION_ANGLE);
                break;

            default:
                //Do something to throw error
                break;
        }
    }
#endif
}




const uint32_t                             CAR_CONTROL__MIA_MS=1000;
const CAR_CONTROL_t                        CAR_CONTROL__MIA_MSG={0,15};
const uint32_t                             SENSOR_DATA__MIA_MS = 3000;
const SENSOR_DATA_t                        SENSOR_DATA__MIA_MSG = {0};
const HEARTBEAT_t                          HEARTBEAT__MIA_MSG = {    HEARTBEAT_cmd_NOOP };
const uint32_t                             HEARTBEAT__MIA_MS = 6000;
const uint32_t                             GPS_DATA__MIA_MS = 3000;
const GPS_DATA_t                           GPS_DATA__MIA_MSG = {5,5};
const uint32_t                             COMPASS__MIA_MS = 3000;
const COMPASS_t                            COMPASS__MIA_MSG = {5,5};

int stopCount=0;
bool master =false;
void period_10Hz(uint32_t count)
{
//    static uint32_t counter; //Counter to for checking the period of every second
//    counter++;
    static uint32_t tmp=0;
    while(CAN_rx(can1,&msg,0))
    {

        dbc_msg_hdr_t header;
        header.dlc = msg.frame_fields.data_len;
        header.mid = msg.msg_id;
        switch(header.mid)
        {
            case 130:
                dbc_decode_ANDROID_CMD(&androidcmd, msg.data.bytes, &header);
                break;
            case 135:
                dbc_decode_ANDROID_LOCATION(&androidLocation, msg.data.bytes, &header);
                break;
            case 140:
                dbc_decode_CAR_CONTROL(&carControl,msg.data.bytes,&header);
                break;
            case 150:
                dbc_decode_SENSOR_DATA(&sensor_can_msg, msg.data.bytes, &header);
                break;
            case 160:
                dbc_decode_GPS_DATA(&gps_can_msg, msg.data.bytes, &header);
                break;
            case 170:
                dbc_decode_COMPASS(&compass_can_msg, msg.data.bytes, &header);
                break;
            case 120:
                dbc_decode_HEARTBEAT(&heartbeat,msg.data.bytes,&header);
                LE.toggle(1);
                break;

        }
    }

    dbc_handle_mia_COMPASS(&compass_can_msg,100);
    dbc_handle_mia_GPS_DATA(&gps_can_msg,100);
    dbc_handle_mia_SENSOR_DATA(&sensor_can_msg,100);
    dbc_handle_mia_HEARTBEAT(&heartbeat,10);
    if(heartbeat.HEARTBEAT_cmd == HEARTBEAT_cmd_NOOP)
    {
        str.setDirection(CENTER);
        spd.setSpeed(STOP);
        val = 0;
        LE.on(4);
    }
    else{
        if(val > (STOP- 0.5) || val<( STOP + 0.5))
            str.setDirection(carControl.CAR_CONTROL_steer);
        LE.off(4);

        //flag code - set flag for direction
        flag = carControl.CAR_CONTROL_speed;
        //end set flag

        if(flag==0)
        {
            if(val>STOP || val <STOP)
            {

                val = STOP;

            }
            tmp = count;
        }
        else if(flag == 1 )
        {
            spd.RefCts.ref_count_medium=androidcmd.ANDROID_CMD_speed;
            //LD.setNumber(spd.RefCts.ref_count_medium);
            spd.setSpeed(val);
            tmp = count;
        }
        else if (flag ==2)
        {
            if(tmp>(count +5)) flag =0;
            if(tmp == count)
            {
                if(val > 14)
                {
                    spd.setSpeed(STOP);
                }
            }
            if((count - tmp) == 1)
            {
                spd.setSpeed(SLOWREVERSE);
            }
            if((count - tmp) == 2)
            {

                spd.setSpeed(STOP);

            }
            if((count - tmp) == 3)
            {

                spd.setSpeed(SLOWREVERSE);

                LD.clear();
                LD.setRightDigit('B');
                tmp = 0;
            }
            val=STOP;
        }

        if(count%3==0)
            val=spd.speed_check(flag,val);
        //end flag code

    }
    telemetry.MOTOR_TELEMETRY_pwm=val;
    dbc_encode_and_send_MOTOR_TELEMETRY(&telemetry);
}

}

void period_100Hz(uint32_t count)
{


}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
}
