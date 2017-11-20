

#ifndef LIDAR_SENSOR_HPP_
#define LIDAR_SENSOR_HPP_
#include <stdint.h>
#include "uart2.hpp"
#include "lidar_sensor.h"
#include "stdio.h"

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
        void check_start_scan();
        bool update_lane_lut();
        bool uart_active();
        bool angle_is_in_range_of_current_lane(uint8_t current_lane);
        bool angle_is_in_range_of_next_lane(uint8_t next_lane);
        char receive_lidar_data();

        scan_data_packet_t data_packet [360];


        struct lane_params
        {
            uint16_t min;//min angle value of this lane
            uint16_t max;
        };

        struct lane_params lane[8];//this should be placed .hpp and initialized in init

        
        uint16_t quality_value = 0;
        uint16_t distance_value_cm = 0;
        uint16_t angle_value_deg = 0;
        uint16_t lane_distances[9];


        bool flag = false;
        bool check_start = false;
        bool lane_lut[9] ;
        bool one=true;
        float lookup1[9];

        bool lane_algorithm();
        uint16_t get_quality_value();
        uint16_t get_angle_value();//return angle in degrees
        uint16_t get_distance_value();//return distance in cm

        char arr[8] = { (char)0xa5 , (char)0x5a , (char)0x05 , (char)0x00 , (char)0x00 , (char)0x40 , (char)0x81};

        void send_lidar_command(lidar_cmd_t lidar_cmd);//we will send an enum data type
    private:
        Lidar_Sensor() {

            //create the lane array
            //0 means this lane is clear
            for(int i =0; i < 9; i++)
            {
                lane_distances[i] = 0;
                lane_lut[i] = false;
            }

            //Create 360 Structs for Data and put the pointers to those structs
            //in the queue

            u2.init(115200, 50, 2);
        }
        friend class SingletonTemplate<Lidar_Sensor>;
};

#define rplidar  Lidar_Sensor::getInstance()
#endif /* LIDAR_SENSOR_HPP_ */
