

#ifndef LIDAR_SENSOR_HPP_
#define LIDAR_SENSOR_HPP_
#include <stdint.h>
#include "uart2.hpp"




/**
 * Lidar Sensor class used to get lidar data reading from the RPLIDAR Sensor
 * Rplidar returns sensor daya broken up as so.
 * First Byte Sent is the quality of the data
 * Second byte sent is
 * and it can also provide the movement activity of the board.
 *
 * @ingroup BoardIO
 */

class Lidar_Sensor : public SingletonTemplate<Lidar_Sensor>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful

        bool lidar_stop();//this is a unidriectional command we wil return true for success
        bool lidar_reset();//unidirectional, send true for succesful reset
        bool lidar_scan();//pass a data structure to this function, return true for succces
        bool lidar_express_scan();//pass data structure, return true
        bool lidar_force_scan();
        void get_info();
        void get_health();
        void get_sample_rate();
        void set_motor_speed(uint8_t PWM, uint8_t frequency ,uint8_t duty_cycle);

    private:
        Lidar_Sensor() { }  ///< Private constructor of this Singleton class
        friend class SingletonTemplate<Lidar_Sensor>;  ///< Friend class used for Singleton Template
};


#endif /* LIDAR_SENSOR_HPP_ */
