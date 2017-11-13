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

int Bluetooth::getSignalType(char* rx, char* latitudeChar, char* longitudeChar) {
    int signalType = 0;
    int idIndex = strlen(rx) - 1;
    char id = rx[idIndex];
    printf("ID: %c\n", id);

    if(id == '$') {
        printf("Sent Start\n");
        signalType = 1;
    }
    else if(id == '@') {
        //Send Stop
        signalType = 2;
    }
    else if(id == '(') {
        //Send GPS
        getLatLong(rx, latitudeChar, longitudeChar);
        latitude = strtod(latitudeChar, NULL);
        longitude = strtod(longitudeChar, NULL);
        signalType = 3;
    }
    return signalType;
}

void Bluetooth::getLatLong(char* rx, char* latitude, char* longitude) {
    char *separatorIndex; //Index for ','
    int index = 0;

    separatorIndex = strchr(rx, ',');
    index = int(separatorIndex - rx);
    strncpy(latitude, rx, index);
    strncpy(longitude, rx + index + 1, strlen(rx) - 1);
    printf("Index: %d\n", index);
}

bool Bluetooth::sendCanData(int flag, ANDROID_CMD_t cmd, can_msg_t msg) {
    if(flag == 1) {
        cmd.ANDROID_CMD_start = 1;
    }
    if(flag == 2) {
        cmd.ANDROID_CMD_kill = 1;
    }
    if(flag == 3) {
        cmd.ANDROID_CMD_lat = latitude;
        cmd.ANDROID_CMD_long = longitude;
    }

    dbc_msg_hdr_t msg_hdr = dbc_encode_MOTOR_CMD(can_msg.data.bytes, &motor_cmd);
    can_msg.msg_id = msg_hdr.mid;
    can_msg.frame_fields.data_len = msg_hdr.dlc;

        // Queue the CAN message to be sent out
    CAN_tx(can1, &can_msg, 0);
    return true;
}
