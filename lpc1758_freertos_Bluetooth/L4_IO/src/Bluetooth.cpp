#include "Bluetooth.hpp"


Uart2& u2 = Uart2::getInstance();

bool Bluetooth::init(int baudRate, int rxQueue, int txQueue) {
    u2.init(baudRate,rxQueue,txQueue);
    return 0;
}

//bool Bluetooth::getGPS(char *rxBuffer,int length,int timeout) {
bool Bluetooth::getGPS(int length,int timeout) {
    char rx[length];
    if(u2.gets(rx, length, timeout))
        printf("\nChar: %s",rx);
    /*else
        printf(".");*/
    //rxBuffer = "";
    return true;
}

bool Bluetooth::sendGPS() {

    return true;
}

