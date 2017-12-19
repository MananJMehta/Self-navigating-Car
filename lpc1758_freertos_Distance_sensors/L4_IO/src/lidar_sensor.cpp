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



bool Lidar_Sensor::uart_active()
{
    //this will return true if uart was active in the last 1000ms
    bool x = u2.recentlyActive(500);
    return x;
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

    //Send the start scan lidar command to the lidar sensor
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

//empty UART 2 queue
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


uint16_t Lidar_Sensor::get_quality_value()
{
    float quality = (float)rplidar.receive_lidar_data();
    return (uint16_t)quality;
}

uint16_t Lidar_Sensor::get_angle_value()
{
    uint16_t angle;
    float angle_deg;
    uint16_t temp;

    angle = (uint16_t)(rplidar.receive_lidar_data()); //store bottom byte of angle value
    temp = (uint16_t)(rplidar.receive_lidar_data()); //store top byte of angle value
    angle = angle>>1;//shift out check bit
    angle |= temp<<7;//concatenate top and bottom angle value

    angle_deg = (float)(angle)/64.0;//divide by 64 to get actual angle

    return (uint16_t)angle_deg;//return as integer
}

uint16_t Lidar_Sensor::get_distance_value()
{
    uint16_t temp1;
    uint16_t distance;
    float distance_q6;

    distance = (uint16_t)(rplidar.receive_lidar_data()); //get da distance_1
    temp1 = (uint16_t)(rplidar.receive_lidar_data()); //get da distance 2
    distance |= temp1<<8;
    distance_q6 = (float)(distance)/4.0;
    return (uint16_t)(distance_q6/10);
}



bool Lidar_Sensor::update_lane_lut()
{

    quality_value = get_quality_value();//return quality value
    angle_value_deg = get_angle_value();//return angle in degrees
    distance_value_cm = get_distance_value();//return distance in cm

    if(lane_algorithm()) return true;

    else return false;
}




bool Lidar_Sensor::angle_is_in_range_of_current_lane(uint8_t current_lane)
{
    if(current_lane != 4)
    {
        if (angle_value_deg >= lane[current_lane].min && angle_value_deg < lane[current_lane].max)
        {
            return true;
        }

        else
        {
            return false;
        }
    }
    else
    {
        if (((angle_value_deg >= lane[current_lane].min) && (angle_value_deg < 360)) | ((angle_value_deg >= 0) && (angle_value_deg < lane[current_lane].max)))
        {
            return true;
        }

        else
        {
            return false;
        }
    }

}

bool Lidar_Sensor::angle_is_in_range_of_next_lane(uint8_t next_lane)
{
    if(next_lane != 4)
    {
        if (angle_value_deg >= lane[next_lane].min && angle_value_deg < lane[next_lane].max)
        {
            return true;
        }

        else
        {
            return false;
        }
    }
    else
    {
        if (((angle_value_deg >= lane[next_lane].min) && (angle_value_deg < 360)) | ((angle_value_deg >= 0) && (angle_value_deg < lane[next_lane].max)))
        {
            return true;
        }

        else
        {
            return false;
        }
    }
}




bool Lidar_Sensor::lane_algorithm(void)
{
    static uint8_t count = 0;
    static bool local_lanes [18];
    static const float object_range = 150;//set to 200
    static const uint8_t data = 5;
    static uint16_t min = 600;
    static uint8_t current_lane = 4;//start at lane 4
    static uint8_t next_lane = 5;


    //data should be starting from lane 4 so there should be only 2 if statement
    //one that does cheking and processing for current lane and one that checks fort the next lane
    if((angle_value_deg<360) && (angle_value_deg>0))
    {
    if(angle_is_in_range_of_current_lane(current_lane)) //in the range for current lane
    {
        //will catch all the data points minus the first
       if(distance_value_cm <= 600 && distance_value_cm>0)
        {
           if(distance_value_cm<min)
           {
               min = distance_value_cm;
           }

           if(distance_value_cm <= object_range)
            {
                count++;
            }
        }


    }
    //this if statement will:
    //-determine if prev lane was occupied
    //-determine the min value of the prev lane
    else if (angle_is_in_range_of_next_lane(next_lane))
    {
        //check if prev lane was accupied
        if(count>data)
        {
            local_lanes[current_lane] = true;
        }
        else
        {
            local_lanes[current_lane] = false;
        }

        lane_distances[current_lane] = min;//this will set lane_distance to the smallest value from prev lane
        min = 600;//set this equal to the first value in the lane
        count = 0;//reset the count for this lane
        if (next_lane == 17)
        {
            next_lane = 0;
            current_lane = 17;
        }
        else if (next_lane == 0)
        {
            next_lane = 1;
            current_lane = 0;
        }
        else
        {
            next_lane++;
            current_lane++;
        }

        if(distance_value_cm <= object_range && distance_value_cm>0)
        {
            if(distance_value_cm<min)
            {
                min = distance_value_cm;
            }
            count++;
        }
    }

    else// data is not in sync. Wait, but send out error msg
    {


        return false;
    }

    }

     memcpy(lane_lut, local_lanes, sizeof(lane_lut));
//    memcpy(lane_lut, local_lanes, sizeof(lane_lut));
    return true;
}



