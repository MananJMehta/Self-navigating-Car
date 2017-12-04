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
#include "_can_dbc/generated_can.h"
#include "can.h"
#include "eint.h"
#include "gpio.hpp"
#include "L4.5_Motor_Control/Steering.hpp"
#include "L4.5_Motor_Control/Speed.hpp"
#include "lcd.hpp"
#include "printf_lib.h"
#include "eint.h"
#include "string.h"

//#define LCD
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

Speed spd;
Steering str;
float val = SLOW;
int flag=false;
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
//enum lcd_led{Sensor, Motor, Comm, Geo};
enum lcd_digits{Miles_covered, Miles_remaining, Dest_lat, Dest_long, Current_lat, Current_long};
enum lcd_health{System, Battery, Degree0, Degree20, Degree40, Degree60, Degree80, Degree_neg20, Degree_neg40, Degree_neg60, Degree_neg80};
enum lcd_status{Off, On};
enum lcd_ultrasound{Ultrasound_left, Ultrasound_front, Ultrasound_right};
enum lcd_form{Main_page, Sensor_page, Distance_page, GPS_page};

/**
 * Global variables to pass data for in 1 Hz scheduler from 10 Hz scheduler
 */
uint8_t speed, us_front, us_left, us_right;
uint16_t distance_covered, distance_remaining;
uint16_t deg_0, deg_20, deg_40, deg_60, deg_80, deg_neg20, deg_neg40, deg_neg60, deg_neg80;
uint16_t dest_lat_val, dest_long_val, current_lat_val, current_long_val;



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
#ifdef LCD
    Uart2& u2 = Uart2::getInstance();
    u2.init(115200);
#endif

    return true; // Must return true upon success
    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}
#ifdef LCD
/**
 * Send readings of LCD's Main Page
 */
void update_LCD_main_page()
{
    /**
     * TODO - These two lines can be removed during implementation
     * Pass the calculated speed instead
     */
    int value = get_random_int(20);
    char random_speed = value;

    display_speedometer(random_speed); //TODO - Set this to speed
    display_LCD_large_led(Battery, On);
}

/**
 * Send readings of LCD's Distance Page
 */
void update_LCD_distance_page()
{
    //Display LCD Numbers
    //TODO - Send Miles
    display_lcd_numbers(Miles_covered, 1234);  //Scaled by 100 (1234 will display as 12.34)
    display_lcd_numbers(Miles_remaining, 5678); //Scaled by 100
}

/**
 * Send readings of LCD's GPS Page
 */
void update_LCD_GPS_page()
{
    //TODO - Set this to dest_lat_val and other coordinate variables
    display_lcd_geo(Dest_lat, 132456789); //Scaled by 1 million (1000000)
    display_lcd_geo(Dest_long, 223654321);
    display_lcd_geo(Current_lat, 356112789);
    display_lcd_geo(Current_long, 432345678);
}

/**
 * Send readings of LCD's Main Page
 */
void update_LCD_sensor_page()
{
    //Display Ultrasound Sensor readings
    display_lcd_bar(Ultrasound_left, us_left * 15); //Scaling up by 15 to display in LED
    display_lcd_bar(Ultrasound_front, us_front * 15);
    display_lcd_bar(Ultrasound_right, us_right * 15);

    display_LCD_large_led(Degree0, deg_0);
    display_LCD_large_led(Degree20, deg_20);
    display_LCD_large_led(Degree40, deg_40);
    display_LCD_large_led(Degree60, deg_60);
    display_LCD_large_led(Degree80, deg_80);
    display_LCD_large_led(Degree_neg20, deg_neg20);
    display_LCD_large_led(Degree_neg40, deg_neg40);
    display_LCD_large_led(Degree_neg60, deg_neg60);
    display_LCD_large_led(Degree_neg80, deg_neg80);
}

/**
 * Below are your periodic functions.
 * The argument 'count' is the number of times each periodic task is called.
 */
#endif
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
#ifdef LCD
    display_bus_reset();


    if (getButtonState()) //Check if the startStop button is pressed in LCD
    {
        display_LCD_large_led(System, On);
    }
    else
    {
        display_LCD_large_led(System, Off);
    }


    /**
     * Check which form (Page) of LCD is active
     */
    char form = check_form();

    /**
     * Display the information of the Active LCD Page
     */
    switch(form)
    {
        case Main_page:
            update_LCD_main_page();
            break;
        case Distance_page:
            update_LCD_distance_page();
            break;
        case Sensor_page:
            update_LCD_sensor_page();
            break;
        case GPS_page:
            update_LCD_GPS_page();
            break;
        default:
            //Do something to throw error
            break;
    }
#endif
}


can_msg_t msg;
CAR_CONTROL_t carControl;
SENSOR_DATA_t sensor_can_msg;
COMPASS_t compass_can_msg;
GPS_DATA_t gps_can_msg;
HEARTBEAT_t heartbeat;
LIDAR_DATA_VALUES_t lidar_data_can_msg;
DISTANCE_VALUES_t lidar_dist_can_msg;
ANDROID_CMD_t androidcmd;

const uint32_t                             CAR_CONTROL__MIA_MS=1000;
const CAR_CONTROL_t                        CAR_CONTROL__MIA_MSG={0,15};
const uint32_t                             SENSOR_DATA__MIA_MS = 3000;
const SENSOR_DATA_t                        SENSOR_DATA__MIA_MSG = {0};
const HEARTBEAT_t                          HEARTBEAT__MIA_MSG = {    HEARTBEAT_cmd_NOOP };
const uint32_t                             HEARTBEAT__MIA_MS = 2000;
const uint32_t                             GPS_DATA__MIA_MS = 3000;
const GPS_DATA_t                           GPS_DATA__MIA_MSG = {5,5};
const uint32_t                             COMPASS__MIA_MS = 3000;
const COMPASS_t                            COMPASS__MIA_MSG = {5,5};
const uint32_t                             DISTANCE_VALUES__MIA_MS = 3000;
const DISTANCE_VALUES_t                    DISTANCE_VALUES__MIA_MSG = {5};
const uint32_t                             LIDAR_DATA_VALUES__MIA_MS = 3000;
const LIDAR_DATA_VALUES_t                  LIDAR_DATA_VALUES__MIA_MSG = {5};

int stopCount=0;
bool master =false;
void period_10Hz(uint32_t count)
{
    static uint32_t counter; //Counter to for checking the period of every second
    counter++;

    while(CAN_rx(can1,&msg,0))
    {

        dbc_msg_hdr_t header;
        header.dlc = msg.frame_fields.data_len;
        header.mid = msg.msg_id;
        switch(header.mid)
        {
            case 120:
                dbc_decode_HEARTBEAT(&heartbeat,msg.data.bytes,&header);
                LE.toggle(1);
                break;
            case 130:
                dbc_decode_ANDROID_CMD(&androidcmd, msg.data.bytes, &header);
                break;
            case 140:
                dbc_decode_CAR_CONTROL(&carControl,msg.data.bytes,&header);
                break;
            case 144:
                dbc_decode_LIDAR_DATA_VALUES(&lidar_data_can_msg, msg.data.bytes, &header);
                break;
            case 150:
                dbc_decode_SENSOR_DATA(&sensor_can_msg, msg.data.bytes, &header);
                break;
            case 155:
                dbc_decode_DISTANCE_VALUES(&lidar_dist_can_msg, msg.data.bytes, &header);
                break;
            case 160:
                dbc_decode_GPS_DATA(&gps_can_msg, msg.data.bytes, &header);
                break;
            case 170:
                dbc_decode_COMPASS(&compass_can_msg, msg.data.bytes, &header);
                break;

        }
    }

    dbc_handle_mia_COMPASS(&compass_can_msg,100);
    dbc_handle_mia_GPS_DATA(&gps_can_msg,100);
    dbc_handle_mia_SENSOR_DATA(&sensor_can_msg,100);
    dbc_handle_mia_HEARTBEAT(&heartbeat,100);
    dbc_handle_mia_LIDAR_DATA_VALUES(&lidar_data_can_msg,100);
    dbc_handle_mia_DISTANCE_VALUES(&lidar_dist_can_msg,100);
    if(dbc_handle_mia_CAR_CONTROL(&carControl,100))
        LE.on(3);
    else LE.off(3);

    str.setDirection(carControl.CAR_CONTROL_steer);

    //flag code - set flag
    flag = carControl.CAR_CONTROL_speed;

    //end set flag

    if(flag==false)
    {
        if(val>STOP)
        {

            val = STOP;
        }
    }
    else if(flag>0 )
    {

        spd.RefCts.ref_count_medium=androidcmd.ANDROID_CMD_speed;
        spd.setSpeed(val);
        stopCount=0;
    }


    if(count%2==0)
        val=spd.speed_check(flag,val);
    //end flag code
    telemetry.MOTOR_TELEMETRY_pwm=val;
    dbc_encode_and_send_MOTOR_TELEMETRY(&telemetry);


#ifdef LCD
    //Update values from CAN every 10 iterations (every 1 Second)
    if (counter % 10 == 0) {

        /**
         * Set Lidar sensor values for LCD display
         */
        deg_0 = sensor_can_msg.LIDAR_0;
        deg_20 = sensor_can_msg.LIDAR_20;
        deg_40 = sensor_can_msg.LIDAR_40;
        deg_60 = sensor_can_msg.LIDAR_60;
        deg_80 = sensor_can_msg.LIDAR_80;
        deg_neg20 = sensor_can_msg.LIDAR_neg20;
        deg_neg40 = sensor_can_msg.LIDAR_neg40;
        deg_neg60 = sensor_can_msg.LIDAR_neg60;
        deg_neg80 = sensor_can_msg.LIDAR_neg80;

        /**
         * Set Ultrasound sensor values for LCD display
         */
        us_left = sensor_can_msg.SONAR_left;
        us_right = sensor_can_msg.SONAR_right;
        us_front = sensor_can_msg.SONAR_back;

        /**
         * Set GPS Coordinates values for LCD Display
         */
        current_lat_val = gps_can_msg.GPS_LATITUDE;
        current_long_val = gps_can_msg.GPS_LONGITUDE;

        /**
         * TODO - Need to update the speed value, distance remaining,
         * distance covered, destination latitude and destination
         * longitude once the DBC file is updated
         */


        /**
         * dest_lat_val = ;
         * dest_long_val = ;
         * speed = ;
         * distance_covered = ;
         * distance_remaining = ;
         */
    }
#endif
}

void period_100Hz(uint32_t count)
{


}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
}
