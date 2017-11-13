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
    bool sendCanData(ANDROID_CMD_t cmd, can_msg_t can_msg, int signalType);

    double latitude[10];
    double longitude[10];
    int checkpointCounter;

    Bluetooth() {
        latitude[10] = 0;
        longitude[10] = 0;
        checkpointCounter = sizeof(latitude) / sizeof(double);
    };
    //friend class SingletonTemplate<Bluetooth>;  ///< Friend class used for Singleton Template
};



#endif /* BLUETOOTH_H_*/

