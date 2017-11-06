/*
 * sensor_functions.h
 *
 *  Created on: Nov 4, 2017
 *      Author: Hugo Quiroz
 */

#ifndef SENSOR_FUNCTIONS_H_
#define SENSOR_FUNCTIONS_H_

#include "lidar_sensor.hpp"
#include "_can_dbc/generated_can.h"
#include "sonar_sensor.hpp"

//Sonar_Sensor* sonar;


//this function will send lane data ad sonar sensor data should
//be renamed appropriately
void send_lidar_sonar_data()
{
    SENSOR_DATA_t SensorData;

    SensorData.LIDAR_neg80 = !rplidar.lane_lut[0];
    SensorData.LIDAR_neg60 = !rplidar.lane_lut[1];
    SensorData.LIDAR_neg40 = !rplidar.lane_lut[2];
    SensorData.LIDAR_neg20 = !rplidar.lane_lut[3];
    SensorData.LIDAR_0 = !rplidar.lane_lut[4];
    SensorData.LIDAR_20 = !rplidar.lane_lut[5];
    SensorData.LIDAR_40 = !rplidar.lane_lut[6];
    SensorData.LIDAR_60 = !rplidar.lane_lut[7];
    SensorData.LIDAR_80 = !rplidar.lane_lut[8];


    //add the Sonar Sensor code here



    dbc_encode_and_send_SENSOR_DATA(&SensorData);

}




#endif /* SENSOR_FUNCTIONS_H_ */
