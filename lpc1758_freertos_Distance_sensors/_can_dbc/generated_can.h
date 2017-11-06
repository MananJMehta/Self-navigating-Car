/// DBC file: ../../common_dbc/243.dbc    Self node: 'SENSOR'  (ALL = 0)
/// This file can be included by a source file, for example: #include "generated.h"
#ifndef __GENEARTED_DBC_PARSER
#define __GENERATED_DBC_PARSER
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>



/// Extern function needed for dbc_encode_and_send()
extern bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]);

/// Missing in Action structure
typedef struct {
    uint32_t is_mia : 1;          ///< Missing in action flag
    uint32_t mia_counter_ms : 31; ///< Missing in action counter
} dbc_mia_info_t;

/// CAN message header structure
typedef struct { 
    uint32_t mid; ///< Message ID of the message
    uint8_t  dlc; ///< Data length of the message
} dbc_msg_hdr_t; 

static const dbc_msg_hdr_t HEARTBEAT_HDR =                        {  120, 1 };
// static const dbc_msg_hdr_t ANDROID_CMD_HDR =                      {  130, 8 };
// static const dbc_msg_hdr_t CAR_CONTROL_HDR =                      {  140, 2 };
static const dbc_msg_hdr_t SENSOR_DATA_HDR =                      {  150, 2 };
// static const dbc_msg_hdr_t GPS_DATA_HDR =                         {  160, 8 };
// static const dbc_msg_hdr_t COMPASS_HDR =                          {  170, 8 };
// static const dbc_msg_hdr_t MOTOR_DATA_HDR =                       {  180, 1 };

/// Enumeration(s) for Message: 'HEARTBEAT' from 'MASTER'
typedef enum {
    HEARTBEAT_cmd_NOOP = 0,
    HEARTBEAT_cmd_SYNC = 1,
    HEARTBEAT_cmd_REBOOT = 2,
} HEARTBEAT_cmd_E ;




/// Message: HEARTBEAT from 'MASTER', DLC: 1 byte(s), MID: 120
typedef struct {
    HEARTBEAT_cmd_E HEARTBEAT_cmd;            ///< B7:0   Destination: SENSOR,MOTOR,ANDROID,GEO

    dbc_mia_info_t mia_info;
} HEARTBEAT_t;


/// Message: SENSOR_DATA from 'SENSOR', DLC: 2 byte(s), MID: 150
typedef struct {
    uint8_t LIDAR_neg80 : 1;                  ///< B0:0  Min: 0 Max: 1   Destination: MASTER,MOTOR,ANDROID
    uint8_t LIDAR_neg60 : 1;                  ///< B1:1  Min: 0 Max: 1   Destination: MASTER,MOTOR,ANDROID
    uint8_t LIDAR_neg40 : 1;                  ///< B2:2  Min: 0 Max: 1   Destination: MASTER,MOTOR,ANDROID
    uint8_t LIDAR_neg20 : 1;                  ///< B3:3  Min: 0 Max: 1   Destination: MASTER,MOTOR,ANDROID
    uint8_t LIDAR_0 : 1;                      ///< B4:4  Min: 0 Max: 1   Destination: MASTER,MOTOR,ANDROID
    uint8_t LIDAR_20 : 1;                     ///< B5:5  Min: 0 Max: 1   Destination: MASTER,MOTOR,ANDROID
    uint8_t LIDAR_40 : 1;                     ///< B6:6  Min: 0 Max: 1   Destination: MASTER,MOTOR,ANDROID
    uint8_t LIDAR_60 : 1;                     ///< B7:7  Min: 0 Max: 1   Destination: MASTER,MOTOR,ANDROID
    uint8_t LIDAR_80 : 1;                     ///< B8:8  Min: 0 Max: 1   Destination: MASTER,MOTOR,ANDROID
    uint8_t SONAR_left : 2;                   ///< B11:10  Min: 0 Max: 2   Destination: MASTER,MOTOR,ANDROID
    uint8_t SONAR_right : 2;                  ///< B13:12  Min: 0 Max: 2   Destination: MASTER,MOTOR,ANDROID
    uint8_t SONAR_back : 2;                   ///< B15:14  Min: 0 Max: 2   Destination: MASTER,MOTOR,ANDROID

    // No dbc_mia_info_t for a message that we will send
} SENSOR_DATA_t;


/// @{ These 'externs' need to be defined in a source file of your project
extern const uint32_t                             HEARTBEAT__MIA_MS;
extern const HEARTBEAT_t                          HEARTBEAT__MIA_MSG;
/// @}


/// Not generating code for dbc_encode_HEARTBEAT() since the sender is MASTER and we are SENSOR

/// Not generating code for dbc_encode_ANDROID_CMD() since the sender is ANDROID and we are SENSOR

/// Not generating code for dbc_encode_CAR_CONTROL() since the sender is MASTER and we are SENSOR

/// Encode SENSOR's 'SENSOR_DATA' message
/// @returns the message header of this message
static inline dbc_msg_hdr_t dbc_encode_SENSOR_DATA(uint8_t bytes[8], SENSOR_DATA_t *from)
{
    uint32_t raw;
    bytes[0]=bytes[1]=bytes[2]=bytes[3]=bytes[4]=bytes[5]=bytes[6]=bytes[7]=0;

    // Not doing min value check since the signal is unsigned already
    if(from->LIDAR_neg80 > 1) { from->LIDAR_neg80 = 1; } // Max value: 1
    raw = ((uint32_t)(((from->LIDAR_neg80)))) & 0x01;
    bytes[0] |= (((uint8_t)(raw) & 0x01)); ///< 1 bit(s) starting from B0

    // Not doing min value check since the signal is unsigned already
    if(from->LIDAR_neg60 > 1) { from->LIDAR_neg60 = 1; } // Max value: 1
    raw = ((uint32_t)(((from->LIDAR_neg60)))) & 0x01;
    bytes[0] |= (((uint8_t)(raw) & 0x01) << 1); ///< 1 bit(s) starting from B1

    // Not doing min value check since the signal is unsigned already
    if(from->LIDAR_neg40 > 1) { from->LIDAR_neg40 = 1; } // Max value: 1
    raw = ((uint32_t)(((from->LIDAR_neg40)))) & 0x01;
    bytes[0] |= (((uint8_t)(raw) & 0x01) << 2); ///< 1 bit(s) starting from B2

    // Not doing min value check since the signal is unsigned already
    if(from->LIDAR_neg20 > 1) { from->LIDAR_neg20 = 1; } // Max value: 1
    raw = ((uint32_t)(((from->LIDAR_neg20)))) & 0x01;
    bytes[0] |= (((uint8_t)(raw) & 0x01) << 3); ///< 1 bit(s) starting from B3

    // Not doing min value check since the signal is unsigned already
    if(from->LIDAR_0 > 1) { from->LIDAR_0 = 1; } // Max value: 1
    raw = ((uint32_t)(((from->LIDAR_0)))) & 0x01;
    bytes[0] |= (((uint8_t)(raw) & 0x01) << 4); ///< 1 bit(s) starting from B4

    // Not doing min value check since the signal is unsigned already
    if(from->LIDAR_20 > 1) { from->LIDAR_20 = 1; } // Max value: 1
    raw = ((uint32_t)(((from->LIDAR_20)))) & 0x01;
    bytes[0] |= (((uint8_t)(raw) & 0x01) << 5); ///< 1 bit(s) starting from B5

    // Not doing min value check since the signal is unsigned already
    if(from->LIDAR_40 > 1) { from->LIDAR_40 = 1; } // Max value: 1
    raw = ((uint32_t)(((from->LIDAR_40)))) & 0x01;
    bytes[0] |= (((uint8_t)(raw) & 0x01) << 6); ///< 1 bit(s) starting from B6

    // Not doing min value check since the signal is unsigned already
    if(from->LIDAR_60 > 1) { from->LIDAR_60 = 1; } // Max value: 1
    raw = ((uint32_t)(((from->LIDAR_60)))) & 0x01;
    bytes[0] |= (((uint8_t)(raw) & 0x01) << 7); ///< 1 bit(s) starting from B7

    // Not doing min value check since the signal is unsigned already
    if(from->LIDAR_80 > 1) { from->LIDAR_80 = 1; } // Max value: 1
    raw = ((uint32_t)(((from->LIDAR_80)))) & 0x01;
    bytes[1] |= (((uint8_t)(raw) & 0x01)); ///< 1 bit(s) starting from B8

    // Not doing min value check since the signal is unsigned already
    if(from->SONAR_left > 2) { from->SONAR_left = 2; } // Max value: 2
    raw = ((uint32_t)(((from->SONAR_left)))) & 0x03;
    bytes[1] |= (((uint8_t)(raw) & 0x03) << 2); ///< 2 bit(s) starting from B10

    // Not doing min value check since the signal is unsigned already
    if(from->SONAR_right > 2) { from->SONAR_right = 2; } // Max value: 2
    raw = ((uint32_t)(((from->SONAR_right)))) & 0x03;
    bytes[1] |= (((uint8_t)(raw) & 0x03) << 4); ///< 2 bit(s) starting from B12

    // Not doing min value check since the signal is unsigned already
    if(from->SONAR_back > 2) { from->SONAR_back = 2; } // Max value: 2
    raw = ((uint32_t)(((from->SONAR_back)))) & 0x03;
    bytes[1] |= (((uint8_t)(raw) & 0x03) << 6); ///< 2 bit(s) starting from B14

    return SENSOR_DATA_HDR;
}

/// Encode and send for dbc_encode_SENSOR_DATA() message
static inline bool dbc_encode_and_send_SENSOR_DATA(SENSOR_DATA_t *from)
{
    uint8_t bytes[8];
    const dbc_msg_hdr_t hdr = dbc_encode_SENSOR_DATA(bytes, from);
    return dbc_app_send_can_msg(hdr.mid, hdr.dlc, bytes);
}



/// Not generating code for dbc_encode_GPS_DATA() since the sender is GEO and we are SENSOR

/// Not generating code for dbc_encode_COMPASS() since the sender is GEO and we are SENSOR

/// Not generating code for dbc_encode_MOTOR_DATA() since the sender is MOTOR and we are SENSOR

/// Decode MASTER's 'HEARTBEAT' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool dbc_decode_HEARTBEAT(HEARTBEAT_t *to, const uint8_t bytes[8], const dbc_msg_hdr_t *hdr)
{
    const bool success = true;
    // If msg header is provided, check if the DLC and the MID match
    if (NULL != hdr && (hdr->dlc != HEARTBEAT_HDR.dlc || hdr->mid != HEARTBEAT_HDR.mid)) {
        return !success;
    }

    uint32_t raw;
    raw  = ((uint32_t)((bytes[0]))); ///< 8 bit(s) from B0
    to->HEARTBEAT_cmd = (HEARTBEAT_cmd_E)((raw));

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter

    return success;
}


/// Not generating code for dbc_decode_ANDROID_CMD() since 'SENSOR' is not the recipient of any of the signals

/// Not generating code for dbc_decode_CAR_CONTROL() since 'SENSOR' is not the recipient of any of the signals

/// Not generating code for dbc_decode_SENSOR_DATA() since 'SENSOR' is not the recipient of any of the signals

/// Not generating code for dbc_decode_GPS_DATA() since 'SENSOR' is not the recipient of any of the signals

/// Not generating code for dbc_decode_COMPASS() since 'SENSOR' is not the recipient of any of the signals

/// Not generating code for dbc_decode_MOTOR_DATA() since 'SENSOR' is not the recipient of any of the signals

/// Handle the MIA for MASTER's HEARTBEAT message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter reaches the MIA threshold, MIA struct will be copied to *msg
static inline bool dbc_handle_mia_HEARTBEAT(HEARTBEAT_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const dbc_mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= HEARTBEAT__MIA_MS);

    if (!msg->mia_info.is_mia) { // Not MIA yet, so keep incrementing the MIA counter
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { // Previously not MIA, but it is MIA now
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = HEARTBEAT__MIA_MSG;
        msg->mia_info.mia_counter_ms = HEARTBEAT__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

#endif
