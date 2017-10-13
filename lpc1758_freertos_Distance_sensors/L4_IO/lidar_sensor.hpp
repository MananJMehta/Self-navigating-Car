

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

        typedef enum {
            lidar_stop_scan = 0x25,
            lidar_reset_core = 0x40,
            lidar_start_scan = 0x20,
            lidar_start_exp_scan = 0x82,
            lidar_start_force_scan = 0x21,
            lidar_get_info = 0x50,
            lidar_get_health = 0x52,
            lidar_get_sample_rate = 0x59,
        } lidar_cmd_t;

        //Struct that contains the 5 bytes of data coming from lidar scan command
        typedef struct {
            uint8_t quality; //the quality of the data reading
            uint8_t angle; //the angle of the data point
            uint8_t distance_1; //first byte describing distance
            uint8_t distance_2; //second byte describing distance
        } __attribute__((__packed__)) lidar_data_t;


        bool init(); ///< Initializes this device, @returns true if successful

        bool stop_scan();//this is a unidriectional command we wil return true for success
        bool reset_core();//unidirectional, send true for succesful reset
        bool start_scan();//pass a data structure to this function, return true for succces
        bool start_express_scan();//pass data structure, return true
        bool start_force_scan();
        void get_info();
        void get_health();
        void get_sample_rate();
        void set_motor_speed(uint8_t PWM, uint8_t frequency ,uint8_t duty_cycle);

    private:
        bool send_lidar_command(lidar_cmd_t lidar_cmd);//we will send an enum data type
        void receive_lidar_command(lidar_cmd_t lidar_cmd);

        Lidar_Sensor() { }  ///< Private constructor of this Singleton class
        friend class SingletonTemplate<Lidar_Sensor>;  ///< Friend class used for Singleton Template
};


#endif /* LIDAR_SENSOR_HPP_ */
