#include "Bluetooth.hpp"
#include "string.h"
#include "stdlib.h"
#include "io.hpp"
#include "math.h"

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

bool Bluetooth::getBluetoothData(char* rxBuffer, int length, int timeout) {
    if(u2.getRxQueueSize() > 0)
        return u2.gets(rxBuffer, length, timeout);
    else
        return false;
}

/*bool Bluetooth::sendBluetoothData() {

    return true;
}*/

int Bluetooth::getSignalType(char* rx) {
    int signalType = 0;
    //int idIndex = strlen(rx) - 1;
    char id = rx[0];
    char temp;
    int tempNum=0;
    printf("ID: %c\n", id);

    if(id == '$') {
        char *speedChar = new char;
        char* endPos = new char;



        //speed = 3;

        //printf("Speed: %d\n",speed);
        LE.on(1);
        signalType = 1;
    }
    else if(id == '@') {
        //Send Stop
        LE.toggle(1);
        signalType = 2;
    }
    else if(id == '^') {
        //Send GPS
        LE.off(1);
        signalType = 3;
    }
    //LE.off(1);
    return signalType;
}

bool Bluetooth::sendCanData(ANDROID_CMD_t android_cmd, ANDROID_LOCATION_t android_loc, can_msg_t can_msg_cmd, can_msg_t can_msg_loc, int signalType) {

    if(signalType == 1) {
        android_cmd.ANDROID_CMD_start = 1;
        android_cmd.ANDROID_CMD_speed = speed;
    }
    else if(signalType == 2)
    {
        android_cmd.ANDROID_CMD_start = 0;
        android_cmd.ANDROID_CMD_speed = 0;
    }
    else
    {
        android_cmd.ANDROID_CMD_start = 0;
        android_loc.ANDROID_CMD_lat = latitude[0];
        android_loc.ANDROID_CMD_long = longitude[0];
    }

    sendStartSpeed(android_cmd, can_msg_cmd);
    sendLocation(android_loc, can_msg_loc);

    return true;
}

bool Bluetooth::getCanData() {
    GPS_DATA_t gps_can_msg = {0};
    COMPASS_t compass_can_msg = {0};
    MOTOR_TELEMETRY_t motel_can_msg = {0};
    can_msg_t can_msg;
    dbc_msg_hdr_t can_msg_h;

    while (CAN_rx(can1, &can_msg, 0))
    {
        can_msg_h.dlc = can_msg.frame_fields.data_len;
        can_msg_h.mid = can_msg.msg_id;
        switch(can_msg_h.mid) {
            case 160:
                dbc_decode_GPS_DATA(&gps_can_msg, can_msg.data.bytes, &can_msg_h);
                if(gps_can_msg.GPS_LATITUDE != 0 && gps_can_msg.GPS_LONGITUDE != 0) {
                    currentLat = gps_can_msg.GPS_LATITUDE;
                    currentLong = gps_can_msg.GPS_LONGITUDE;
                    sendCurrentData(1);
                    LE.toggle(2);
                }
                printf("LAT: %f",gps_can_msg.GPS_LATITUDE);
                break;

            case 170:
                dbc_decode_COMPASS(&compass_can_msg, can_msg.data.bytes, &can_msg_h);
                if(compass_can_msg.CMP_HEADING != 0) {
                    currentHeading = compass_can_msg.CMP_HEADING;
                    sendCurrentData(2);
                    LE.toggle(2);
                }
                printf("HEADING: %f",compass_can_msg.CMP_HEADING);
                break;

            case 190:
                dbc_decode_MOTOR_TELEMETRY(&motel_can_msg, can_msg.data.bytes, &can_msg_h);
                if(motel_can_msg.MOTOR_TELEMETRY_kph != 0) {
                    currentSpeed = motel_can_msg.MOTOR_TELEMETRY_kph;
                    sendCurrentData(3);
                    LE.toggle(2);
                }
                printf("LAT: %f",gps_can_msg.GPS_LATITUDE);
                break;
        }
    }
    return true;
}
int Bluetooth::getCPNum(char* rx) {
    int cpNo= 0;
    int count = 1;
    //cpNo = rx[1] - '0';
    while(*(rx+count) != '(') {
        cpNo = cpNo + ((int)rx[count] * pow(10, count));
        count++;
    }
    return cpNo;
}

void Bluetooth::getLatLong(char* rx, int count) {
    int temp = 0;
    char* startPos;
    char* endPos;
    char* separatorPos;
    char latChar[10];
    char lngChar[10];

    while(temp<count) {
        startPos = strchr(rx, '(');
        separatorPos = strchr(rx, ',');
        endPos = strchr(rx, ')');

        //printf("\n%c %c %c",*startPos,*separatorPos,*endPos);
        int startIndex = (int)(startPos -  rx);
        int endIndex = (int)(endPos - rx);
        int separatorIndex = (int)(separatorPos - rx);
        /*printf("STINDEX: %d\n",startIndex);
        printf("EINDEX: %d\n",endIndex);
        printf("SEPINDEX: %d\n",separatorIndex);*/
        strncpy(latChar, rx + startIndex + 1, separatorIndex - startIndex);
        strncpy(lngChar, rx + separatorIndex + 1, endIndex - separatorIndex);

        latitude[temp]  = atof(latChar);
        longitude[temp] = atof(lngChar);
        printf("LAT: %f\n", latitude[temp]);
        printf("LONG: %f\n", longitude[temp]);
        rx = endPos + 1;
        temp++;
    }
}

bool Bluetooth::sendSpeed() {
    char* desiredSpeed = new char;
    if(SW.getSwitch(1)) {

                        desiredSpeed = "G|123.474457|35.343677";
                                (u2.putline(desiredSpeed, 0));
                                    printf("Sent 2\n");
    }
    /*else if(SW.getSwitch(2)) {
        desiredSpeed = '5';
        if(u2.putChar(desiredSpeed, 0))
            printf("Sent 5\n");
    }
    else if(SW.getSwitch(3)) {
        desiredSpeed = '7';
        if(u2.putChar(desiredSpeed, 0))
            printf("Sent 7\n");
        }
    else if(SW.getSwitch(4)) {
        desiredSpeed = '9';
        if(u2.putChar(desiredSpeed, 0))
            printf("Sent 9\n");
        }*/

    return true;
}

bool Bluetooth::flushBuffer() {
    u2.flush();
    return true;
}

bool Bluetooth::sendStartSpeed(ANDROID_CMD_t android_cmd, can_msg_t can_msg) {
    dbc_msg_hdr_t msg_hdr = dbc_encode_ANDROID_CMD(can_msg.data.bytes, &android_cmd);
    can_msg.msg_id = msg_hdr.mid;
    can_msg.frame_fields.data_len = msg_hdr.dlc;
    CAN_tx(can1, &can_msg, 0);
    return true;
}

bool Bluetooth::sendLocation(ANDROID_LOCATION_t android_loc, can_msg_t can_msg) {
    dbc_msg_hdr_t msg_hdr = dbc_encode_ANDROID_LOCATION(can_msg.data.bytes, &android_loc);
    can_msg.msg_id = msg_hdr.mid;
    can_msg.frame_fields.data_len = msg_hdr.dlc;
    CAN_tx(can1, &can_msg, 0);
    return true;
}

bool Bluetooth::sendCurrentData(int txType) {
    char* txData = new char;
    switch(txType) {
        case 1:
            char lat[50];
            char lng[50];
            sprintf(txData,"G|%f|%f",currentLat,currentLong);
            //txData = "G|" + lat + '|' + lng;
            u2.putline(txData);
            break;
        case 2:
            sprintf(txData,"C|%f",currentHeading);
            u2.putline(txData);
            break;
        case 3:
            sprintf(txData,"S|%f",currentSpeed);
            u2.putline(txData);
            break;
    }
    return true;
}
