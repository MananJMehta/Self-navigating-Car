#ifndef SENSOR_FUNCTIONS_HPP_
#define SENSOR_FUNCTIONS_HPP_

#include "lidar_sensor.hpp"
#include "_can_dbc/generated_can.h"
#include "sonar_sensor.hpp"
#include "io.hpp"
#include "can.h"

void initialize_can();
bool check_heartbeat();
void send_lidar_sonar_data();
void send_distance_values();
void add_some_data_to_msg(LIDAR_DATA_VALUES_t *from);
void send_three_values(LIDAR_DATA_VALUES_t *from);
void send_lane_distance_values ();

#endif /* LIDAR_SENSOR_HPP_ */
