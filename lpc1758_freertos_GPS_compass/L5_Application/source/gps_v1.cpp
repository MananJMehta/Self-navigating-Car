/*
 * gps_v1.cpp
 *
 *  Created on: Oct 5, 2017
 *      Author: kalkikapoor
 */

#include <cstring>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "io.hpp"
#include "uart2.hpp"
#include "printf_lib.h"
#include "GPS_Commands.h"
#include "gps_v1.hpp"
#include "eint.h"
#include "gpio.hpp"

using namespace std;

#define R   6371.0
#define PI  3.14159

GPIO gpio2_0(P2_0);

bool Uart2_GPS::init()
{
    gpio2_0.setAsInput();
    fixFlag = false;
//    Uart2::getInstance().init(9600, UART_RECEIVE_BUFFER_SIZE, UART_TRANSMIT_BUFFER_SIZE);   // Initial GPS baud rate should be 9600
//    Uart2::getInstance().putline(PMTK_SET_BAUD_57600, portMAX_DELAY);
    Uart2::getInstance().init(57600, UART_RECEIVE_BUFFER_SIZE, UART_TRANSMIT_BUFFER_SIZE);
    transmit();

    return true;
}

void Uart2_GPS::transmit()
{
    Uart2::getInstance().putline(PMTK_SET_NMEA_OUTPUT_RMCONLY, portMAX_DELAY);
    Uart2::getInstance().putline(PMTK_SET_NMEA_UPDATE_10HZ, portMAX_DELAY);
    Uart2::getInstance().putline(PMTK_API_SET_FIX_CTL_5HZ, portMAX_DELAY);
//    Uart2::getInstance().putline(PGCMD_ANTENNA, portMAX_DELAY);
}

void Uart2_GPS::receive()
{
//    char *temp = "";
//    setBuffer(temp);

    if(Uart2::getInstance().gets(getBuffer(), UART_RECEIVE_BUFFER_SIZE, 50))
    {
        LE.on(4);
    }
    else
    {
        LE.off(4);
    }
}

char* Uart2_GPS::getBuffer()
{
    return buffer;
}

void Uart2_GPS::setBuffer(char* array)
{
    strcpy(buffer, array);
}

bool Uart2_GPS::parse_data()
{
    if(!memcmp(buffer, "$GPRMC", 6) && buffer[18]=='A' && buffer[30]=='N' && buffer[43]=='W')
    {
        fixFlag = true;
        char lat[10] = "";
        char lon[11] = "";

        memcpy(lat, (buffer+20), 9);
        memcpy(lon, (buffer+32), 10);

        float tempLat = (float)atof(lat);        // Change float values to 6 digits
        latitude = (tempLat - 100*(int)(tempLat/100))/60.0;
        latitude += int(tempLat/100);

        float tempLon = (float)atof(lon);
        longitude = (tempLon - 100*(int)(tempLon/100))/60.0;
        longitude += int(tempLon/100);
        longitude = -longitude;

        LE.on(2);
        return true;
    }
    else
    {
        fixFlag = false;
        latitude = 0.0;
        longitude = 0.0;
        LE.off(2);
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

//Function which returns the bearing angle
float Uart2_GPS::bearingAngle(float lat, float lon)
{
    if (lat == 0.0 && lon == 0.0)
    {
        return 0;
    }
    float dLon = lon - longitude;
    float x = cos(lat * PI/180.0) * sin(dLon * PI/180.0);
    float y = cos(latitude * PI/180.0) * sin(lat * PI/180.0)
              - sin(latitude * PI/180.0) * cos(lat * PI/180.0) * cos(dLon * PI/180.0);

    float bearing = (atan2(x,y) * 180.0/PI);

    if(bearing < 0.0)
    {
        return 360.0 + bearing;
    }

    return bearing;
}

//Function which returns the distance between current location and checkpoint
float Uart2_GPS::distanceCheckpoint(float lat, float lon)
{
    float dLat = lat - latitude;
    float dLon = lon - longitude;

    float A = pow(sin((dLat/2.0) * (PI/180.0)), 2.0)
              + (cos(lat * PI/180.0) * cos(latitude * PI/180.0) * pow(sin(dLon/2 * (PI/180.0)), 2.0));
    float C = 2 * atan2(sqrt(A), sqrt(1-A));

    return R * C * 1000;
}

bool Uart2_GPS::fixLED()    //
{
    return gpio2_0.read();
}
