

#ifndef LIDAR_SENSOR_HPP_
#define LIDAR_SENSOR_HPP_
#include <stdint.h>
#include "uart2.hpp"
#include "lidar_sensor.h"

#define u2  Uart2::getInstance()   ///< Temperature Sensor


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


        bool stop_scan();//this is a unidriectional command we wil return true for success
        bool reset_core();//unidirectional, send true for succesful reset
        void start_scan();//pass a data structure to this function, return true for succces
        bool start_express_scan();//pass data structure, return true
        bool start_force_scan();
        void get_info(info_data_packet_t *lidar_info);
        void get_health(health_data_packet_t *health_data);
        void get_sample_rate(sample_rate_packet_t *sample_rate);
        void set_motor_speed(uint8_t RPM);

        int8_t det_smol_angle();

        char receive_lidar_data();
        bool lane_lut[9] ;

        scan_data_packet_t data_packet [360];

    private:




        void send_lidar_command(lidar_cmd_t lidar_cmd);//we will send an enum data type

        Lidar_Sensor() {

            //create the lane array
            //0 means this lane is clear
            for(int i =0; i < 9; i++)
            {
                lane_lut[i] = false;
            }



            //Create 360 Structs for Data and put the pointers to those structs
            //in the queue



            //set frequency
            //set_PWM
            //set duty_cycle

            u2.init(115200, 50, 2);
        }  ///< Private constructor of this Singleton class
        friend class SingletonTemplate<Lidar_Sensor>;  ///< Friend class used for Singleton Template
};


#endif /* LIDAR_SENSOR_HPP_ */
