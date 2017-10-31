/*
 * lidar_sensor.h
 *
 *  Created on: Oct 21, 2017
 *      Author: Hugo Quiroz
 */

#ifndef LIDAR_SENSOR_H_
#define LIDAR_SENSOR_H_




typedef enum {
    lidar_header = 0xA5,
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
    uint8_t angle_1; //the angle of the data point
    uint8_t angle_2; //
    uint8_t distance_1; //first byte describing distance
    uint8_t distance_2; //second byte describing distance
} __attribute__((__packed__)) scan_data_packet_t, *scan_data_packet_ptr;

//Struct that contains the 5 bytes of data coming from lidar scan command
typedef struct {
    uint8_t tstandard1; //the quality of the data reading
    uint8_t tstandard2; //the angle of the data point
    uint8_t texpress1; //first byte describing distance
    uint8_t texpress2; //second byte describing distance
} __attribute__((__packed__)) sample_rate_packet_t;

//Struct that contains the 5 bytes of data coming from lidar scan command
typedef struct {
    uint8_t model; //the quality of the data reading
    uint8_t firmware_minor; //the angle of the data point
    uint8_t firmware_major; //first byte describing distance
    uint8_t hardware; //second byte describing distance
    uint8_t serial_number [16];
} __attribute__((__packed__)) info_data_packet_t;

//Struct that contains the 5 bytes of data coming from lidar scan command
typedef struct {
    uint8_t status; //the quality of the data reading
    uint8_t error_code1; //the angle of the data point
    uint8_t error_code2; //first byte describing distance
} __attribute__((__packed__)) health_data_packet_t;

#endif /* LIDAR_SENSOR_H_ */
