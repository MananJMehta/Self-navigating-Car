#include <stdint.h>

#include "lidar_sensor.hpp"

#define u2  Uart2::getInstance()   ///< Temperature Sensor


bool Lidar_Sensor::init()
{
    //set frequency
    //set_PWM
    //set duty_cycle

    u2.init(115200, 360, 360);
    return true;
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
    if(send_lidar_command(lidar_stop_scan)) return true;
    else return false;
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
    if(send_lidar_command(lidar_reset_core)) return true;
    return 0;
}


/*
 *
 *
 *
 */

bool Lidar_Sensor::start_scan(scan_data_packet_t *lidar_data)
{
    if(send_lidar_command(lidar_start_scan)) return true;
    return 0;
}

bool Lidar_Sensor::start_express_scan()
{
    if(send_lidar_command(lidar_start_exp_scan)) return true;
    return 0;
}

bool Lidar_Sensor::start_force_scan()
{
    if(send_lidar_command(lidar_start_force_scan)) return true;
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

bool Lidar_Sensor::send_lidar_command(lidar_cmd_t lidar_cmd)
{
    if(u2.printf("%x",lidar_cmd)) return true;
    else return false;
}

//put received thingies in a queue maybe??
void Lidar_Sensor::receive_lidar_data(lidar_cmd_t lidar_cmd)
{
    //u2.gets(pBuff, maxLen, timeout);
}
