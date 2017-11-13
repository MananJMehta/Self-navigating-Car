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
    int getSignalType(char* rx, char* latitudeChar, char* longitudeChar); //Send data on CAN bus
    void getLatLong(char* rx, char* latitude, char* longitude); //Parse data to get Lat and Long
    bool sendCanData(int flag, ANDROID_CMD_t cmd, can_msg_t msg);

    double latitude;
    double longitude;

    Bluetooth() {
        latitude = 0.0;
        longitude = 0.0;
    };
    //friend class SingletonTemplate<Bluetooth>;  ///< Friend class used for Singleton Template
};



#endif /* BLUETOOTH_H_*/

