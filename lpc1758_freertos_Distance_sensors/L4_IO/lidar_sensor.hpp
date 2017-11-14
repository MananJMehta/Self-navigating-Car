

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
        bool update_lanes();
        bool update_lane_lut();
        int8_t det_smol_angle();
        void send_angles();

        char receive_lidar_data();

        scan_data_packet_t data_packet [360];

        uint16_t quality_value = 0;
        uint16_t distance_value_cm = 0;
        uint16_t angle_value_deg = 0;
        uint16_t lane_distances[9];

        uint16_t *quality_value_ptr;
        uint16_t *distance_value_cm_ptr;
        uint16_t *angle_value_deg_ptr;

        bool flag = false;
        bool check_start = false;
        bool lane_lut[9] ;
        uint16_t Lane_LUT;
        bool one=true;
        float lookup1[9];

    private:
        void check_for_lane_angle(uint16_t *distance, uint16_t *angle);
        void lane_algorithm();
        uint16_t get_quality_value();
        uint16_t get_angle_value();//return angle in degrees
        uint16_t get_distance_value();//return distance in cm

        char arr[8] = { 0xa5 , 0x5a , 0x05 , 0x00 , 0x00 , 0x40 , 0x81};

        void send_lidar_command(lidar_cmd_t lidar_cmd);//we will send an enum data type

        Lidar_Sensor() {

            //create the lane array
            //0 means this lane is clear
            for(int i =0; i < 9; i++)
            {
                lane_distances[i] = 0;
                lane_lut[i] = false;
            }
            Lane_LUT = 0;

            quality_value_ptr = &quality_value;
            distance_value_cm_ptr = &distance_value_cm;
            angle_value_deg_ptr = &angle_value_deg;

            //Create 360 Structs for Data and put the pointers to those structs
            //in the queue

            u2.init(115200, 50, 2);
        }
        friend class SingletonTemplate<Lidar_Sensor>;
};

#define rplidar  Lidar_Sensor::getInstance()
#endif /* LIDAR_SENSOR_HPP_ */
