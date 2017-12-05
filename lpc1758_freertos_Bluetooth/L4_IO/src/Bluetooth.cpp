#include "Bluetooth.hpp"
#include "string.h"
#include "stdlib.h"
#include "io.hpp"
#include "math.h"

Uart2& u2 = Uart2::getInstance();
//Bluetooth b;

extern int mode;
extern int cpNo;
int request = 0;
int cpCount = 0;

double latitude[20] = {0};
double longitude[20] = {0};

extern ANDROID_CMD_t android_cmd;
extern ANDROID_LOCATION_t android_loc;
extern can_msg_t can_msg_cmd;


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

bool Bluetooth::getBluetoothData(char* rxBuffer, int rxQueue, int timeout) {
    if(u2.getRxQueueSize() > 0)
        return u2.gets(rxBuffer, rxQueue, timeout);
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
    //printf("ID: %c\n", id); AL

    if(id == '$') {
        if(rx[2] == '-')
            mode = 0;
        else
            mode = 1;

        //speed = 3;

        //printf("Speed: %d\n",speed);
        LE.on(1);
        signalType = 1;
    }
    else if(id == '@') {
        //Send Stop
        LE.off(1);
        signalType = 2;
    }
    else if(id == '^') {
        //Send GPS
        LE.toggle(1);
        signalType = 3;
    }
    //LE.off(1);
    return signalType;
}

bool Bluetooth::sendCanData(ANDROID_CMD_t android_cmd, ANDROID_LOCATION_t android_loc, can_msg_t can_msg_cmd, int signalType){//, int cpNo) {
    //int cpCount = 0;

    if(signalType == 1) {
        android_cmd.ANDROID_CMD_start = 1;
        android_cmd.ANDROID_CMD_speed = desiredSpeed;
        android_cmd.ANDROID_CMD_mode = mode;
        sendStartSpeed(android_cmd, can_msg_cmd);
    }
    else if(signalType == 2) {
        android_cmd.ANDROID_CMD_start = 0;
        android_cmd.ANDROID_CMD_speed = 0;
        sendStartSpeed(android_cmd, can_msg_cmd);
    }

    /*if(request == 1){

        //android_cmd.ANDROID_CMD_start = 1;
        android_loc.ANDROID_CMD_lat = latitude[];
        android_loc.ANDROID_CMD_long = longitude[];

            LD.setNumber(10);
    }*/
    LE.off(3);

    /*if() {
        sendStartSpeed(android_cmd, can_msg_cmd);
        sendLocation(android_loc, can_msg_loc);
    }*/
    return true;
}

bool Bluetooth::getCanData() {
    GPS_DATA_t gps_can_msg = {0};
    COMPASS_t compass_can_msg = {0};
    MOTOR_TELEMETRY_t motel_can_msg = {0};
    MASTER_REQUEST_t request_can_msg = {0};
    HEARTBEAT_t heartbeat_can_msg = {HEARTBEAT_cmd_NOOP};
    can_msg_t can_msg;
    dbc_msg_hdr_t can_msg_h;

    while (CAN_rx(can1, &can_msg, 0))
    {
        can_msg_h.dlc = can_msg.frame_fields.data_len;
        can_msg_h.mid = can_msg.msg_id;
        switch(can_msg_h.mid) {
            case 120:
                dbc_decode_HEARTBEAT(&heartbeat_can_msg, can_msg.data.bytes, &can_msg_h);
                if(heartbeat_can_msg.HEARTBEAT_cmd == HEARTBEAT_cmd_SYNC) {
                    LE.toggle(4);
                }
                else
                    LE.off(4);
                break;

            case 125:
                dbc_decode_MASTER_REQUEST(&request_can_msg, can_msg.data.bytes, &can_msg_h);
                if(request_can_msg.MASTER_REQUEST_cmd == 15) {
                    if(cpCount < cpNo) {
                        LE.toggle(3);
                        android_loc.ANDROID_CMD_lat = latitude[cpCount];
                        android_loc.ANDROID_CMD_long = longitude[cpCount];
                        //printf("LAT_125: %f\n",latitude[cpCount]);
                        //printf("LNG_125: %f\n",longitude[cpCount]);
                        if(cpCount == cpNo - 1)
                            android_loc.ANDROID_CMD_isLast = 1;
                        else
                            android_loc.ANDROID_CMD_isLast = 0;
                        //printf("IsLast: %d\n",android_loc.ANDROID_CMD_isLast);
                        //printf("CpCount: %d\n",cpCount);
                        cpCount++;
                        sendLocation(android_loc, can_msg_cmd);
                    }

                    LD.clear();
                    LD.setNumber(15);
                }
                else
                    request = 0;
                break;

            case 160:
                dbc_decode_GPS_DATA(&gps_can_msg, can_msg.data.bytes, &can_msg_h);
                if(gps_can_msg.GPS_LATITUDE != 0 && gps_can_msg.GPS_LONGITUDE != 0) {
                    currentLat = gps_can_msg.GPS_LATITUDE;
                    currentLong = gps_can_msg.GPS_LONGITUDE;
                    //sendCurrentData(1);
                    LE.toggle(2);
                }
                //printf("LAT: %f",gps_can_msg.GPS_LATITUDE); AL
                break;

            case 170:
                dbc_decode_COMPASS(&compass_can_msg, can_msg.data.bytes, &can_msg_h);
                //if(compass_can_msg.CMP_HEADING != 0) {
                currentHeading = (double)(compass_can_msg.CMP_HEADING);
                currentBearing = (double)(compass_can_msg.CMP_BEARING);
                cpDistance = (double)(compass_can_msg.DISTANCE_CHECKPOINT);
                defAngle = (double)(compass_can_msg.DEFLECTION_ANGLE);
                sendCurrentData(2);
                LE.toggle(2);
                //printf("HEADING: %f",compass_can_msg.CMP_HEADING); AL
                break;

            case 190:
                dbc_decode_MOTOR_TELEMETRY(&motel_can_msg, can_msg.data.bytes, &can_msg_h);
                if(motel_can_msg.MOTOR_TELEMETRY_kph != 0) {
                    currentSpeed = motel_can_msg.MOTOR_TELEMETRY_kph;
                    //sendCurrentData(3);
                    LE.toggle(2);
                }
                //printf("LAT: %f",gps_can_msg.GPS_LATITUDE); AL
                break;
        }
    }
    return true;
}
int Bluetooth::getCPNum(char* rx) {
    /*int cpNo= 0;
    int count = 1;
    cpNo = rx[1] - '0';*/
    //int cpNo = 0;
    char* startPos;
    char* endPos;
    char cp[10];
    startPos = strchr(rx, '^');
    startPos += 1;
    endPos = strchr(rx, '(');
    strncpy(cp, rx + 1, endPos - startPos);
    cpNo = atoi(cp);
    //printf("CP: %d\n",cpNo);
    return cpNo;
}

void Bluetooth::getLatLong(char* rx) {
    int temp = 0;
    char* startPos;
    char* endPos;
    char* separatorPos;
    char latChar[10];
    char lngChar[10];

    while(temp<cpNo) {
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
        //printf("LAT: %f\n", latitude[temp]); //AL
        //printf("LONG: %f\n", longitude[temp]); //AL
        rx = endPos + 1;
        temp++;
    }
}

/*
bool Bluetooth::sendSpeed() {
    char* desiredSpeed = new char;
    if(SW.getSwitch(1)) {
        currentLat = 123.474457;
        currentLong = 35.343677;
        sprintf(desiredSpeed,"G|%f|%f",currentLat,currentLong);
        //desiredSpeed = "G|123.474457|35.343677";
        (u2.putline(desiredSpeed, 0));
        //printf("Sent 2\n"); AL
    }
    else if(SW.getSwitch(2)) {
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
        }

    return true;
}
*/

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
    LD.clear();
    LD.setNumber(5);
    dbc_msg_hdr_t msg_hdr = dbc_encode_ANDROID_LOCATION(can_msg.data.bytes, &android_loc);
    can_msg.msg_id = msg_hdr.mid;
    can_msg.frame_fields.data_len = msg_hdr.dlc;
    CAN_tx(can1, &can_msg, 0);
    return true;
}

bool Bluetooth::sendCurrentData(int txType) {
    char* txData = new char[75];
    //char txData[100] = "G37.334709|-121.892403H0.000000S0.000000B169.899994A169.899994D68.949997&";
    //sprintf(txData,"G%f|%fC%fS%f&",currentLat, currentLong, currentHeading, currentSpeed);
    sprintf(txData,"G%f|%fH%fS%fB%fA%fD%f&",currentLat, currentLong, currentHeading, currentSpeed, currentBearing, defAngle, cpDistance);
    /*switch(txType) {
        case 1:
            char lat[50];
            char lng[50];
            sprintf(txData,"G%f|%fH%fS%fB%fA%fD%f&",currentLat, currentLong, currentHeading, currentSpeed, currentBearing, defAngle, cpDistance);
            //txData = "G|" + lat + '|' + lng;
            //u2.putline(txData);
            break;
        case 2:
            sprintf(txData,"G%f|%fC%fS%f&",currentLat, currentLong, currentHeading, currentSpeed);
            //u2.putline(txData);
            break;
        case 3:
            sprintf(txData,"G%f|%fC%fS%f&",currentLat, currentLong, currentHeading, currentSpeed);
            //u2.putline(txData);
            break;
    }*/
    u2.putline(txData);
    delete [] txData;
    return true;
}

void Bluetooth::getDesiredSpeed(char* rx) {
    //int speed;
    if(mode == 1) {
        char* startPos;
        char* endPos;
        char speedChar[10];

        startPos = strchr(rx, '$');
        startPos += 1;
        endPos = strchr(rx, 'S');
        strncpy(speedChar, rx + 1, endPos - startPos);
        desiredSpeed = atoi(speedChar);
    }
    else if(mode == 0)
        desiredSpeed = 3;
}

