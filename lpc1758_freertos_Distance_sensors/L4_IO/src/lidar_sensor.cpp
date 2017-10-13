#include <stdint.h>

#include "lidar_sensor.hpp"

#define u2  Uart2::getInstance()   ///< Temperature Sensor


bool Lidar_Sensor::init()
{
    u2.init(115200, 360, 360);
    return true;
}

bool Lidar_Sensor::stop_scan()
{
    if(send_lidar_command(lidar_stop_scan)) return true;
    else return false;
}
bool Lidar_Sensor::reset_core()
{
    return 0;
}

bool Lidar_Sensor::start_scan()
{
    return 0;
}

bool Lidar_Sensor::start_express_scan()
{
    return 0;
}

bool Lidar_Sensor::start_force_scan()
{
    return 0;
}

void Lidar_Sensor::get_info()
{

}


//this should return something or put something somewhere maybe a struct??
void Lidar_Sensor::get_health()
{
    send_lidar_command(lidar_get_health);
}

void Lidar_Sensor::get_sample_rate()
{

}

void Lidar_Sensor::set_motor_speed(uint8_t PWM, uint8_t frequency, uint8_t duty_cycle)
{

}

bool Lidar_Sensor::send_lidar_command(lidar_cmd_t lidar_cmd)
{
    if(u2.printf("%x",lidar_cmd)) return true;
    else return false;
}

//put received thingies in a queue maybe??
void Lidar_Sensor::receive_lidar_command(lidar_cmd_t lidar_cmd)
{
    //u2.gets(pBuff, maxLen, timeout);
}
