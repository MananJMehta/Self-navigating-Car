#include <stdint.h>

#include "lidar_sensor.hpp"




bool Lidar_Sensor::init()
{
    Uart2 &u2 = Uart2::getInstance();
    u2.init(115200, 360, 360);

    return true;
}
bool Lidar_Sensor::lidar_stop()
{
    return 0;
}
bool Lidar_Sensor::lidar_reset()
{
    return 0;
}

bool Lidar_Sensor::lidar_scan()
{
    return 0;
}

bool Lidar_Sensor::lidar_express_scan()
{
    return 0;
}

bool Lidar_Sensor::lidar_force_scan()
{
    return 0;
}

void Lidar_Sensor::get_info()
{

}

void Lidar_Sensor::get_health()
{

}

void Lidar_Sensor::get_sample_rate()
{

}

void Lidar_Sensor::set_motor_speed(uint8_t PWM, uint8_t frequency, uint8_t duty_cycle)
{

}
