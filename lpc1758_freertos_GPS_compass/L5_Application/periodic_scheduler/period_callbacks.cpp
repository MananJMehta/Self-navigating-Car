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
#include "printf_lib.h"
#include "gps_v1.hpp"
#include "can.h"
#include "_can_dbc/generated_can.h"
#include "string.h"
#include "utilities.h"
#include "compass.hpp"

#define ARD_LAT_TEST 37.335472
#define ARD_LON_TEST -121.892294

Uart2_GPS gps;
Compass compass;
HEARTBEAT_t hrt_buffer = {HEARTBEAT_cmd_NOOP};
ANDROID_LOCATION_t ard_buffer = {0};

//Compass
COMPASS_t compass_msg = { 0 };

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);
    return CAN_tx(can1, &can_msg, 0);
}


static bool heartbeat_flag = false;

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3);

void CAN_GPS_Trasmit();
void CAN_COMPASS_Transmit();
//void calibrate_compass();
//void stop_calibrate();
//void original_firmware();

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    //Compass calibration

    gps.init();

    CAN_init(can1, 100, 100, 100, NULL, NULL);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);

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

    if (SW.getSwitch(1))
    {
        //calibrate_compass();
        compass.Calibrate_Compass();
    }

    if (SW.getSwitch(2))
    {
        //stop_calibrate();
        compass.Stop_Calibrate();
    }

    if (SW.getSwitch(3))
    {
        compass.Original_Firmware_Calibration();
        //original_firmware();
    }

    gps.parse_data();

    printf("Lat: %f\n", gps.getLatitude());
    printf("Lon: %f\n", gps.getLongitude());

    float bearing_test = gps.bearingAngle(ARD_LAT_TEST, ARD_LON_TEST);
    float distance_test = gps.distanceCheckpoint(ARD_LAT_TEST, ARD_LON_TEST);

    printf("Dist: %f\n", distance_test);
    printf("Bear: %f\n", bearing_test);

    if(heartbeat_flag)
    {
        CAN_GPS_Trasmit();
        CAN_COMPASS_Transmit();
    }

    //    LE.toggle(1);
}

void period_10Hz(uint32_t count)
{
    gps.receive();

    can_msg_t can_msg_receive;

    while(CAN_rx(can1, &can_msg_receive, 0))
    {
        dbc_msg_hdr_t msg_hdr_receive;
        msg_hdr_receive.mid = can_msg_receive.msg_id;
        msg_hdr_receive.dlc = can_msg_receive.frame_fields.data_len;

        switch(can_msg_receive.msg_id)
        {
            case 120:
                dbc_decode_HEARTBEAT(&hrt_buffer, can_msg_receive.data.bytes, &msg_hdr_receive);
                if(hrt_buffer.HEARTBEAT_cmd == HEARTBEAT_cmd_SYNC)
                {
                    heartbeat_flag = true;
                    LE.toggle(1);
                }
                else if(hrt_buffer.HEARTBEAT_cmd == HEARTBEAT_cmd_NOOP)
                {
                    heartbeat_flag = false;
                    LE.on(1);
                }
                break;

            case 135:
                dbc_decode_ANDROID_LOCATION(&ard_buffer, can_msg_receive.data.bytes, &msg_hdr_receive);
                //u0_dbg_printf("Lat: %f\n", ard_buffer.ANDROID_CMD_lat);
                //u0_dbg_printf("Lon: %f\n", ard_buffer.ANDROID_CMD_long);
                break;
        }
    }
}


void period_100Hz(uint32_t count)
{
//    LE.toggle(3);
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
//    LE.toggle(4);
}

void CAN_GPS_Trasmit()
{
    GPS_DATA_t gps_buffer_transmit = {0};
    gps_buffer_transmit.GPS_LATITUDE = gps.getLatitude();
    gps_buffer_transmit.GPS_LONGITUDE = gps.getLongitude();
//    printf("Lat: %f\n",  gps_buffer_transmit.GPS_LATITUDE);
//    printf("Lon: %f\n", gps_buffer_transmit.GPS_LONGITUDE);

    can_msg_t can_gps_transmit = {0};
    dbc_msg_hdr_t msg_hdr_transmit = dbc_encode_GPS_DATA(can_gps_transmit.data.bytes, &gps_buffer_transmit);
    can_gps_transmit.msg_id = msg_hdr_transmit.mid;
    can_gps_transmit.frame_fields.data_len = msg_hdr_transmit.dlc;

    if(CAN_tx(can1, &can_gps_transmit, 0))
    {
        LE.toggle(3);
    }
    else
    {
        LE.off(3);
    }
}

void CAN_COMPASS_Transmit()
{
    float compass_heading_value = compass.Get_Compass_Heading();
//    printf("Compass heading = %f\n", compass_heading_value);
    compass_msg.CMP_HEADING = compass_heading_value;
    dbc_encode_and_send_COMPASS(&compass_msg);
}

#if 0
void CAN_COMPASS_Transmit()
{
    int addr = 0xc0;
    int reg = 0x02;
    int bearing_int = 0;
    float bearing_float = 0;
    uint8_t buffer[2] = { 0 };

    if (I2C2::getInstance().readRegisters(addr, reg, &buffer[0], 2))
    {
        bearing_int = buffer[0];
        bearing_int <<= 8;
        bearing_int += buffer[1];
        bearing_float = (float)(bearing_int/10.00);
        //u0_dbg_printf("Int = %d\n", bearing_int);
        printf("Compass heading = %f\n", bearing_float);
        compass_msg.CMP_BEARING = bearing_float;
        dbc_encode_and_send_COMPASS(&compass_msg);
    }
}



void calibrate_compass()
{
    int command_reg = 0;
    int addr = 0xc0;
    uint8_t buffer[1] = { 0 };
    buffer[0] = 0xF0;
    I2C2::getInstance().writeRegisters(addr, command_reg, &buffer[0], 1);
    delay_ms(30);
    buffer[0] = 0xF5;
    I2C2::getInstance().writeRegisters(addr, command_reg, &buffer[0], 1);
    delay_ms(30);
    buffer[0] = 0xF6;
    I2C2::getInstance().writeRegisters(addr, command_reg, &buffer[0], 1);
    //printf("Calibration started\n");
}

void stop_calibrate()
{
    int command_reg = 0;
    int addr = 0xc0;
    uint8_t buffer[1] = { 0 };
    buffer[0] = 0xF8;
    I2C2::getInstance().writeRegisters(addr, command_reg, &buffer[0], 1);
}

void original_firmware()
{
    int command_reg = 0;
    int addr = 0xc0;
    uint8_t buffer[1] = { 0 };
    buffer[0] = 0x20;
    I2C2::getInstance().writeRegisters(addr, command_reg, &buffer[0], 1);
    delay_ms(30);
    buffer[0] = 0x2A;
    I2C2::getInstance().writeRegisters(addr, command_reg, &buffer[0], 1);
    delay_ms(30);
    buffer[0] = 0x60;
    I2C2::getInstance().writeRegisters(addr, command_reg, &buffer[0], 1);
    //printf("Calibration started\n");
}
#endif
