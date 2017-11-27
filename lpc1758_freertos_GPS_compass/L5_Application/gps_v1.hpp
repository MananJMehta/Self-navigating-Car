/*
 * gps_v1.hpp
 *
 *  Created on: Oct 5, 2017
 *      Author: kalkikapoor
 */

#ifndef GPS_V1_HPP_
#define GPS_V1_HPP_

#include <stdio.h>

#define UART_RECEIVE_BUFFER_SIZE 120
#define UART_TRANSMIT_BUFFER_SIZE 60

class Uart2_GPS
{
public:
    bool init();
    void transmit();
    void receive();
    bool parse_data();
    char* getBuffer();
    void setLatitude(float lat);
    void setLongitude(float lon);
    float getLatitude();
    float getLongitude();
    float bearingAngle(float lat, float lon);
    float distanceCheckpoint(float lat, float lon);

private:
    char buffer[UART_RECEIVE_BUFFER_SIZE];
    float latitude;
    float longitude;
};

#endif /* GPS_V1_HPP_ */
