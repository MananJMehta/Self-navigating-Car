#include "Bluetooth.hpp"
#include "string.h"
#include "stdlib.h"


Uart2& u2 = Uart2::getInstance();
//Bluetooth b;

bool Bluetooth::init(int baudRate, int rxQueue, int txQueue) {
    u2.init(baudRate,rxQueue,txQueue);
    return 0;
}

/*bool Bluetooth::initCan(can_t can, uint32_t baudRate, uint16_t rxQSize, uint16_t txQSize) {
    CAN_init(can, baudRate, rxQSize, txQSize, NULL, NULL);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can);
    return 0;
}*/

bool Bluetooth::getBluetoothData(char* rxBuffer, int length,int timeout) {
    int status = false;
    if(u2.gets(rxBuffer, length, timeout))
        status = true;

    return status;
}

/*bool Bluetooth::sendBluetoothData() {

    return true;
}*/

int Bluetooth::getSignalType(char* rx) {
    int signalType = 0;
    //int idIndex = strlen(rx) - 1;
    char id = rx[0];
    printf("ID: %c\n", id);

    if(id == '$') {
        printf("Sent Start\n");
        signalType = 1;
    }
    else if(id == '@') {
        //Send Stop
        signalType = 2;
    }
    else if(id == '^') {
        //Send GPS
        signalType = 3;
    }

    return signalType;
}

bool Bluetooth::sendCanData(ANDROID_CMD_t android_cmd, can_msg_t can_msg, int signalType) {

    if(signalType == 1)
        android_cmd.ANDROID_CMD_start = 1;
    else if(signalType == 2)
        android_cmd.ANDROID_CMD_start = 0;
    else
        android_cmd.ANDROID_CMD_start = 0;

    android_cmd.ANDROID_CMD_lat = latitude[0];
    android_cmd.ANDROID_CMD_long = longitude[0];

    dbc_msg_hdr_t msg_hdr = dbc_encode_ANDROID_CMD(can_msg.data.bytes, &android_cmd);
    can_msg.msg_id = msg_hdr.mid;
    can_msg.frame_fields.data_len = msg_hdr.dlc;

        // Queue the CAN message to be sent out
    CAN_tx(can1, &can_msg, 0);
    return true;
}

int Bluetooth::getCPNum(char* rx) {
    int cpNo= 0;
    cpNo = rx[1] - '0';
    return cpNo;
}

void Bluetooth::getLatLong(char* rx, int count) {
    char* startIndex;
    int temp=0;
    char* endIndex;
    char* separatorIndex;
    char* lat;
    char* lng;

    while(temp<count) {
        startIndex = strchr(rx, '(');
        separatorIndex = strchr(rx, ',');
        endIndex = strchr(rx, ')');

        //printf("\n%c %c %c",*startIndex,*separatorIndex,*endIndex);

        strncpy(lat, startIndex, (separatorIndex - startIndex));
        strncpy(lng, separatorIndex + 1, endIndex-separatorIndex+1);

        printf("\n%s",lng);

        /*latitude[temp] = strtod(lat, NULL);
        longitude[temp] = strtod(lng, NULL);
        rx = endIndex+1;
        printf("LATitude: %f\n",latitude[0]);
        printf("Longitude: %f\n",longitude[0]);*/
        temp++;
    }
}
