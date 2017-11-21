/*
 * Bluetooth.h
 *
 *  Created on: Oct 21, 2017
 *      Author: ashis
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_
#include "uart2.hpp"
#include "can.h"
#include "stdio.h"
#include "_can_dbc/generated_can.h"

class Bluetooth {
public:
    bool init(int baudRate, int rxQueue, int txQueue); //Init UART
    //bool initCan(can_t can, uint32_t baudRate, uint16_t rxQSize, uint16_t txQSize);
    bool getBluetoothData(char* rxBuffer, int length, int timeout); //Get data from App
    //bool sendBluetoothData(char);
    int getSignalType(char* rx); //Send data on CAN bus
    int getCPNum(char* rx);
    void getLatLong(char* rx, int count); //Parse data to get Lat and Long
    bool sendCanData(ANDROID_CMD_t android_cmd, ANDROID_LOCATION_t android_loc, can_msg_t can_msg_cmd, can_msg_t can_msg_loc, int signalType);
    bool getCanData();
    bool sendSpeed();
    bool flushBuffer();
    bool sendStartSpeed(ANDROID_CMD_t android_cmd, can_msg_t can_msg); //Send Speed entered with Start on CAN Bus
    bool sendLocation(ANDROID_LOCATION_t android_loc, can_msg_t can_msg);
    bool sendCurrentData(int txType); //Send data to App based on type
    //int getSpeed(char* rx);

    double latitude[10];
    double longitude[10];
    double currentLat;
    double currentLong;
    double currentHeading;
    float currentSpeed; //Send current speed to App
    int speed; //Get desired speed from App
    int checkpointCounter;

    Bluetooth() {
        latitude[10] = 0;
        longitude[10] = 0;
        currentLat = 0;
        currentLong = 0;
        currentHeading = 0;
        currentSpeed = 0;
        speed = 0;
        checkpointCounter = sizeof(latitude) / sizeof(double);
    };
    //friend class SingletonTemplate<Bluetooth>;  ///< Friend class used for Singleton Template
};



#endif /* BLUETOOTH_H_*/

