#include <stdint.h>

#include "lidar_sensor.hpp"


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



//put received thingies in a queue maybe??
void Lidar_Sensor::receive_lidar_data(lidar_cmd_t lidar_cmd)
{
    //u2.gets(pBuff, maxLen, timeout);
}
