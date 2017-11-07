/*
 * Bluetooth.h
 *
 *  Created on: Oct 21, 2017
 *      Author: ashis
 */

#include "uart2.hpp"
#include "stdio.h"

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

class Bluetooth {//: public SingletonTemplate<Bluetooth> {
public:
    bool init(int baudRate, int rxQueue, int txQueue);
    bool getGPS(int length, int timeout);
    bool sendGPS();

    Bluetooth() { };  ///< Private constructor of this Singleton class
    //friend class SingletonTemplate<Bluetooth>;  ///< Friend class used for Singleton Template
};



#endif /* BLUETOOTH_H_*/

