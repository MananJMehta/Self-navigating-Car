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

void send_distance_values()
{
    SENSOR_VALUES_t SensorValue;

    SensorValue.LIDAR_DISTANCE_CM = rplidar.distance_value_cm;
    SensorValue.LIDAR_ANGLE_DEG = rplidar.angle_value_deg;
    SensorValue.LIDAR_QUALITY_DEG = rplidar.quality_value;
    //add the Sonar Sensor code here
    dbc_encode_and_send_SENSOR_VALUES(&SensorValue);

}

void add_some_data_to_msg(LIDAR_DATA_VALUES_t *from)
{
    static uint8_t count = 0;

    if (count == 0)
    {
        from->LIDAR_DISTANCE_CM_1 = rplidar.distance_value_cm;
        from->LIDAR_ANGLE_DEG_1 = rplidar.angle_value_deg;
        count++;
    }
    else if (count == 1)
    {
        from->LIDAR_DISTANCE_CM_2 = rplidar.distance_value_cm;
        from->LIDAR_ANGLE_DEG_2 = rplidar.angle_value_deg;
        count++;
    }
    else if (count == 2)
    {
        from->LIDAR_DISTANCE_CM_3 = rplidar.distance_value_cm;
        from->LIDAR_ANGLE_DEG_3 = rplidar.angle_value_deg;
        count = 0;
    }
}

void send_three_values(LIDAR_DATA_VALUES_t *from)
{
    dbc_encode_and_send_LIDAR_DATA_VALUES(from);
}




#endif /* SENSOR_FUNCTIONS_H_ */
