#include <stdint.h>

#include "lidar_sensor.hpp"
#include "printf_lib.h"
#include "can.h"
#include "string.h"

//extern "C"
//{
    #include "_can_dbc/generated_can.h"
//}

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}


void Lidar_Sensor::send_lidar_command(lidar_cmd_t lidar_cmd)
{

    u2.printf("%c", lidar_header);
    u2.printf("%c",lidar_cmd);
}


/*
 *
 * This command will not return anything but it will stop the
Lidar Sensor, you should wait 1ms before sending another command
also we should send command to see status of lidar to make sure lidar
is stopped, maybe get sample rate

*/
bool Lidar_Sensor::stop_scan()
{
   send_lidar_command(lidar_stop_scan);
   return false;
}


/*
 *
 * Sending a reset command will revert the Lidar to a similar state as if
 * the sensor has been booted up, wait about 2 ms before sending next command
 * we should send command to check status of lidar to ensure lidar is stopped
 * maybe get sample rate
 *
 *
 * */

bool Lidar_Sensor::reset_core()
{
   send_lidar_command(lidar_reset_core);
    return 0;
}


/*
 *
 *Start the Lidar scan and expect to get values from the lidar
 *Perhap
 *
 */

void Lidar_Sensor::start_scan()
{
    send_lidar_command(lidar_start_scan);
}

bool Lidar_Sensor::start_express_scan()
{
   send_lidar_command(lidar_start_exp_scan);
    return 0;
}

bool Lidar_Sensor::start_force_scan()
{
    send_lidar_command(lidar_start_force_scan);
    return 0;
}

void Lidar_Sensor::get_info(info_data_packet_t *lidar_info)
{

    send_lidar_command(lidar_get_info); //do nothing


}


//this should return something or put something somewhere maybe a struct??
void Lidar_Sensor::get_health(health_data_packet_t *health_data)
{
    send_lidar_command(lidar_get_health);
}

void Lidar_Sensor::get_sample_rate(sample_rate_packet_t *sample_rate)
{
    send_lidar_command(lidar_get_sample_rate);
}

void Lidar_Sensor::set_motor_speed(uint8_t RPM)
{
    sample_rate_packet_t sample_rate;

    get_sample_rate(&sample_rate);
}

int8_t Lidar_Sensor::det_smol_angle(){
    uint8_t i = 4;
    if(lane_lut[i])
        return 0;
    for (int j = 1; j <= 4; j--)
    {
        if (lane_lut[i-j])
            return -1*j;
        else if(lane_lut[i+j])
            return j;
    }
    return -5;
}


//put received thingies in a queue maybe??
char Lidar_Sensor::receive_lidar_data()
{
    char str[1];
    if(u2.getChar(str,portMAX_DELAY))
        return str[0];
    return 0xFF;
}

/*
 *
 * This function will check the
 *
 * */
void Lidar_Sensor::check_start_scan()
{

    for(uint8_t i=0; i<7 ;i++)
        if(arr[i]!= receive_lidar_data())
            return;

    check_start = true;
}

//this task can be turnned on and off every time there is a
//start and stop command...so in the rplidar.start_scan function enable this task
//in rplidar.stop_scan(); turn off this task
bool Lidar_Sensor::update_lanes()//this function is obsolete<<<<<<
{
    SENSOR_DATA_t SensorData;

    SensorData.LIDAR_neg80 = !lane_lut[0];
    SensorData.LIDAR_neg60 = !lane_lut[1];
    SensorData.LIDAR_neg40 = !lane_lut[2];
    SensorData.LIDAR_neg20 = !lane_lut[3];
    SensorData.LIDAR_0 = !lane_lut[4];
    SensorData.LIDAR_20 = !lane_lut[5];
    SensorData.LIDAR_40 = !lane_lut[6];
    SensorData.LIDAR_60 = !lane_lut[7];
    SensorData.LIDAR_80 = !lane_lut[8];

    dbc_encode_and_send_SENSOR_DATA(&SensorData);
    return true;

}

float Lidar_Sensor::get_quality_value()
{
    float quality = rplidar.receive_lidar_data;
    return quality;
}

float Lidar_Sensor::get_angle_value()
{
    uint16_t angle;
    float angle_deg;
    uint16_t temp;
    angle = (uint16_t)(rplidar.receive_lidar_data()); //get da angle_1
    temp = (uint16_t)(rplidar.receive_lidar_data()); //get da angle_2
    angle = angle>>1;
    angle |= temp<<7;
    angle_deg = (float)(angle)/64.0;
    return angle_deg;
}

float Lidar_Sensor::get_distance_value()
{
    uint16_t temp1;
    uint16_t angle;
    uint16_t distance;
    float distance_q6;

    distance = (uint16_t)(rplidar.receive_lidar_data()); //get da distance_1
    temp1 = (uint16_t)(rplidar.receive_lidar_data()); //get da distance 2
    distance |= temp1<<8;
    distance_q6 = (float)(distance)/4.0;
    return (distance_q6*100.0);
}

bool Lidar_Sensor::update_lane_lut()
{

    static const float object_range = 30.0;
    static const uint8_t data = 10;
    static uint8_t count = 0;
    static uint8_t lane = 8;
    static bool local_lanes [9];

    float quality;
    float angle_deg;
    float distance_cm;


    quality = get_quality_value;
    angle_deg = get_angle_value;//return angle in degrees
    distance_cm = get_distance_value;//return distance in cm

    if (angle_deg>=270&&angle_deg<290)
    {
        if(lane == 8)
        {
            if(count>data)
            {
                local_lanes[8] = false;
            }
            else
            {
                local_lanes[8] = true;
            }

            count = 0;
        }

        if(distance_cm <= object_range)
        {
            count++;
        }
        lane = 0;

    }

    else if(angle_deg>=290&&angle_deg<310)
    {
        if(lane == 0)
        {
            if(count>data)
            {
                local_lanes[0] = false;
            }
            else
            {
                local_lanes[0] = true;
            }

            count = 0;
        }

        if(distance_cm <= object_range)
        {
            count++;
        }
        lane = 1;

    }
    else if(angle_deg>=310&&angle_deg<330)
    {
        if(lane == 1)
        {
            if(count>data)
            {
                local_lanes[1] = false;
            }
            else
            {
                local_lanes[1] = true;
            }

            count = 0;
        }

        if(distance_cm <= object_range)
        {
            count++;
        }
        lane = 2;
    }
    else if(angle_deg>=330&&angle_deg<350)
    {
        if(lane == 2)
        {
            if(count>data)
            {
                local_lanes[2] = false;
            }
            else
            {
                local_lanes[2] = true;
            }

            count = 0;
        }

        if(distance_cm <= object_range)
        {
            count++;
        }
        lane = 3;
    }
    else if((angle_deg>=350&&angle_deg<360) | (angle_deg>=0&&angle_deg<10))
    {
        if(lane == 3)
        {
            if(count>data)
            {
                local_lanes[3] = false;
            }
            else
            {
                local_lanes[3] = true;
            }

            count = 0;
        }

        if(distance_cm <= object_range)
        {
            count++;
        }
        lane = 4;
    }
    else if(angle_deg>=10&&angle_deg<30)
    {
        if(lane == 4)
        {
            if(count>data)
            {
                local_lanes[4] = false;
            }
            else
            {
                local_lanes[4] = true;
            }
            count = 0;
        }

        if(distance_cm <= object_range)
        {
            count++;
        }
        lane = 5;
    }
    else if(angle_deg>=30&&angle_deg<50)
    {
        if(lane == 5)
        {
            if(count>data)
            {
                local_lanes[5] = false;
            }
            else
            {
                local_lanes[5] = true;
            }

            count = 0;
        }

        if(distance_cm <= object_range)
        {
            count++;
        }
        lane = 6;
    }
    else if(angle_deg>=50&&angle_deg<70)
    {
        if(lane == 6)
        {
            if(count>data)
            {
                local_lanes[6] = false;
            }
            else
            {
                local_lanes[6] = true;
            }

            count = 0;
        }

        if(distance_cm <= object_range)
        {
            count++;
        }
        lane = 7;
    }
    else if(angle_deg>=70&&angle_deg<90)
    {
        if(lane == 7)
        {
            if(count>data)
            {
                local_lanes[7] = false;
            }
            else
            {
                local_lanes[7] = true;
            }
            count = 0;
        }

        if(distance_cm <= object_range)
        {
            count++;
        }

        lane = 8;
    }

    rplidar.lane_lut = local_lanes;


    return true;
}


