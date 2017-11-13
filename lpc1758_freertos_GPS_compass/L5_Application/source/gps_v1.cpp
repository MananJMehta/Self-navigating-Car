/*
 * gps_v1.cpp
 *
 *  Created on: Oct 5, 2017
 *      Author: kalkikapoor
 */

#include <cstring>
#include <stdlib.h>
#include "io.hpp"
#include "uart2.hpp"
#include "printf_lib.h"
#include "GPS_Commands.h"
#include "gps_v1.hpp"

//char gps_buffer[UART_RECEIVE_BUFFER_SIZE];

bool Uart2_GPS::init()
{
    Uart2::getInstance().init(9600, UART_RECEIVE_BUFFER_SIZE, UART_TRANSMIT_BUFFER_SIZE);   // Initial GPS baud rate should be 9600
    Uart2::getInstance().putline(PMTK_SET_BAUD_57600, portMAX_DELAY);                       // Change GPS baud rate to 57600
    Uart2::getInstance().init(57600, UART_RECEIVE_BUFFER_SIZE, UART_TRANSMIT_BUFFER_SIZE);  // Change SJ One baud rate to 57600
    transmit();

    return true;
}

void Uart2_GPS::transmit()
{
    Uart2::getInstance().putline(PMTK_SET_NMEA_OUTPUT_RMCONLY, portMAX_DELAY);
    Uart2::getInstance().putline(PMTK_SET_NMEA_UPDATE_1HZ, portMAX_DELAY);
    Uart2::getInstance().putline(PMTK_API_SET_FIX_CTL_1HZ, portMAX_DELAY);
//    Uart2::getInstance().putline(PGCMD_ANTENNA, portMAX_DELAY);
}

void Uart2_GPS::receive()
{
    if(Uart2::getInstance().gets(getBuffer(), UART_RECEIVE_BUFFER_SIZE, 50))
    {
        LE.toggle(1);
    }
    else
    {
        LE.on(1);
    }
//    memset(gps_data, 0, sizeof(gps_data));
}

char* Uart2_GPS::getBuffer()
{
    return buffer;
}

bool Uart2_GPS::parse_data()
{
    if(!memcmp(buffer, "$GPRMC", 6) && buffer[18]=='A' && buffer[30]=='N' && buffer[43]=='W')
    {
        char lat[10] = "";
        char lon[11] = "";

        memcpy(lat, (buffer+20), 9);
        memcpy(lon, (buffer+32), 10);
        u0_dbg_printf("lat: %s\n", lat);

        latitude = (float)atof(lat);        // Limit float values---------------------------------------
        longitude = (float)atof(lon);

        LE.toggle(2);
        return true;
    }
    else
    {
        LE.on(2);
    }

    return false;
}

void Uart2_GPS::setLatitude(float lat)
{
    latitude = lat;
}

void Uart2_GPS::setLongitude(float lon)
{
    longitude = lon;
}

float Uart2_GPS::getLatitude()
{
    return latitude;
}

float Uart2_GPS::getLongitude()
{
    return longitude;
}
