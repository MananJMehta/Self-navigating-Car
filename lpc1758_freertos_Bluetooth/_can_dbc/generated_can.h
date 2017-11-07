/// DBC file: ../../common_dbc/243.dbc    Self node: 'ANDROID'  (ALL = 0)
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
static const dbc_msg_hdr_t ANDROID_CMD_HDR =                      {  130, 8 };
// static const dbc_msg_hdr_t CAR_CONTROL_HDR =                      {  140, 2 };
static const dbc_msg_hdr_t SENSOR_DATA_HDR =                      {  150, 2 };
static const dbc_msg_hdr_t GPS_DATA_HDR =                         {  160, 8 };
static const dbc_msg_hdr_t COMPASS_HDR =                          {  170, 8 };
// static const dbc_msg_hdr_t MOTOR_DATA_HDR =                       {  180, 1 };

/// Enumeration(s) for Message: 'HEARTBEAT' from 'MASTER'
typedef enum {
    HEARTBEAT_cmd_REBOOT = 2,
    HEARTBEAT_cmd_NOOP = 0,
    HEARTBEAT_cmd_SYNC = 1,
} HEARTBEAT_cmd_E ;




/// Message: HEARTBEAT from 'MASTER', DLC: 1 byte(s), MID: 120
typedef struct {
    HEARTBEAT_cmd_E HEARTBEAT_cmd;            ///< B7:0   Destination: SENSOR,MOTOR,ANDROID,GEO

    dbc_mia_info_t mia_info;
} HEARTBEAT_t;


/// Message: ANDROID_CMD from 'ANDROID', DLC: 8 byte(s), MID: 130
typedef struct {
    uint8_t ANDROID_CMD_kill : 1;             ///< B0:0   Destination: MASTER
    uint8_t ANDROID_CMD_start : 1;            ///< B1:1   Destination: MASTER
    float ANDROID_CMD_lat;                    ///< B29:2  Min: -90 Max: 90   Destination: MASTER
    float ANDROID_CMD_long;                   ///< B58:30  Min: -180 Max: 180   Destination: MASTER

    // No dbc_mia_info_t for a message that we will send
} ANDROID_CMD_t;


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

    dbc_mia_info_t mia_info;
} SENSOR_DATA_t;


/// Message: GPS_DATA from 'GEO', DLC: 8 byte(s), MID: 160
typedef struct {
    float GPS_LATITUDE;                       ///< B27:0  Min: -90 Max: 90   Destination: MASTER,MOTOR,ANDROID
    float GPS_LONGITUDE;                      ///< B56:28  Min: -180 Max: 180   Destination: MASTER,MOTOR,ANDROID

    dbc_mia_info_t mia_info;
} GPS_DATA_t;


/// Message: COMPASS from 'GEO', DLC: 8 byte(s), MID: 170
typedef struct {
    uint32_t CMP_HEADING;                     ///< B31:0   Destination: MASTER,MOTOR,ANDROID
    uint32_t CMP_BEARING;                     ///< B63:32   Destination: MASTER,MOTOR,ANDROID

    dbc_mia_info_t mia_info;
} COMPASS_t;


/// @{ These 'externs' need to be defined in a source file of your project
extern const uint32_t                             HEARTBEAT__MIA_MS;
extern const HEARTBEAT_t                          HEARTBEAT__MIA_MSG;
extern const uint32_t                             SENSOR_DATA__MIA_MS;
extern const SENSOR_DATA_t                        SENSOR_DATA__MIA_MSG;
extern const uint32_t                             GPS_DATA__MIA_MS;
extern const GPS_DATA_t                           GPS_DATA__MIA_MSG;
extern const uint32_t                             COMPASS__MIA_MS;
extern const COMPASS_t                            COMPASS__MIA_MSG;
/// @}


/// Not generating code for dbc_encode_HEARTBEAT() since the sender is MASTER and we are ANDROID

/// Encode ANDROID's 'ANDROID_CMD' message
/// @returns the message header of this message
static inline dbc_msg_hdr_t dbc_encode_ANDROID_CMD(uint8_t bytes[8], ANDROID_CMD_t *from)
{
    uint32_t raw;
    bytes[0]=bytes[1]=bytes[2]=bytes[3]=bytes[4]=bytes[5]=bytes[6]=bytes[7]=0;

    raw = ((uint32_t)(((from->ANDROID_CMD_kill)))) & 0x01;
    bytes[0] |= (((uint8_t)(raw) & 0x01)); ///< 1 bit(s) starting from B0

    raw = ((uint32_t)(((from->ANDROID_CMD_start)))) & 0x01;
    bytes[0] |= (((uint8_t)(raw) & 0x01) << 1); ///< 1 bit(s) starting from B1

    if(from->ANDROID_CMD_lat < -90) { from->ANDROID_CMD_lat = -90; } // Min value: -90
    if(from->ANDROID_CMD_lat > 90) { from->ANDROID_CMD_lat = 90; } // Max value: 90
    raw = ((uint32_t)(((from->ANDROID_CMD_lat - (-90.000000)) / 1e-06) + 0.5)) & 0xfffffff;
    bytes[0] |= (((uint8_t)(raw) & 0x3f) << 2); ///< 6 bit(s) starting from B2
    bytes[1] |= (((uint8_t)(raw >> 6) & 0xff)); ///< 8 bit(s) starting from B8
    bytes[2] |= (((uint8_t)(raw >> 14) & 0xff)); ///< 8 bit(s) starting from B16
    bytes[3] |= (((uint8_t)(raw >> 22) & 0x3f)); ///< 6 bit(s) starting from B24

    if(from->ANDROID_CMD_long < -180) { from->ANDROID_CMD_long = -180; } // Min value: -180
    if(from->ANDROID_CMD_long > 180) { from->ANDROID_CMD_long = 180; } // Max value: 180
    raw = ((uint32_t)(((from->ANDROID_CMD_long - (-180.000000)) / 1e-06) + 0.5)) & 0x1fffffff;
    bytes[3] |= (((uint8_t)(raw) & 0x03) << 6); ///< 2 bit(s) starting from B30
    bytes[4] |= (((uint8_t)(raw >> 2) & 0xff)); ///< 8 bit(s) starting from B32
    bytes[5] |= (((uint8_t)(raw >> 10) & 0xff)); ///< 8 bit(s) starting from B40
    bytes[6] |= (((uint8_t)(raw >> 18) & 0xff)); ///< 8 bit(s) starting from B48
    bytes[7] |= (((uint8_t)(raw >> 26) & 0x07)); ///< 3 bit(s) starting from B56

    return ANDROID_CMD_HDR;
}

/// Encode and send for dbc_encode_ANDROID_CMD() message
static inline bool dbc_encode_and_send_ANDROID_CMD(ANDROID_CMD_t *from)
{
    uint8_t bytes[8];
    const dbc_msg_hdr_t hdr = dbc_encode_ANDROID_CMD(bytes, from);
    return dbc_app_send_can_msg(hdr.mid, hdr.dlc, bytes);
}



/// Not generating code for dbc_encode_CAR_CONTROL() since the sender is MASTER and we are ANDROID

/// Not generating code for dbc_encode_SENSOR_DATA() since the sender is SENSOR and we are ANDROID

/// Not generating code for dbc_encode_GPS_DATA() since the sender is GEO and we are ANDROID

/// Not generating code for dbc_encode_COMPASS() since the sender is GEO and we are ANDROID

/// Not generating code for dbc_encode_MOTOR_DATA() since the sender is MOTOR and we are ANDROID

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


/// Not generating code for dbc_decode_ANDROID_CMD() since 'ANDROID' is not the recipient of any of the signals

/// Not generating code for dbc_decode_CAR_CONTROL() since 'ANDROID' is not the recipient of any of the signals

/// Decode SENSOR's 'SENSOR_DATA' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool dbc_decode_SENSOR_DATA(SENSOR_DATA_t *to, const uint8_t bytes[8], const dbc_msg_hdr_t *hdr)
{
    const bool success = true;
    // If msg header is provided, check if the DLC and the MID match
    if (NULL != hdr && (hdr->dlc != SENSOR_DATA_HDR.dlc || hdr->mid != SENSOR_DATA_HDR.mid)) {
        return !success;
    }

    uint32_t raw;
    raw  = ((uint32_t)((bytes[0]) & 0x01)); ///< 1 bit(s) from B0
    to->LIDAR_neg80 = ((raw));
    raw  = ((uint32_t)((bytes[0] >> 1) & 0x01)); ///< 1 bit(s) from B1
    to->LIDAR_neg60 = ((raw));
    raw  = ((uint32_t)((bytes[0] >> 2) & 0x01)); ///< 1 bit(s) from B2
    to->LIDAR_neg40 = ((raw));
    raw  = ((uint32_t)((bytes[0] >> 3) & 0x01)); ///< 1 bit(s) from B3
    to->LIDAR_neg20 = ((raw));
    raw  = ((uint32_t)((bytes[0] >> 4) & 0x01)); ///< 1 bit(s) from B4
    to->LIDAR_0 = ((raw));
    raw  = ((uint32_t)((bytes[0] >> 5) & 0x01)); ///< 1 bit(s) from B5
    to->LIDAR_20 = ((raw));
    raw  = ((uint32_t)((bytes[0] >> 6) & 0x01)); ///< 1 bit(s) from B6
    to->LIDAR_40 = ((raw));
    raw  = ((uint32_t)((bytes[0] >> 7) & 0x01)); ///< 1 bit(s) from B7
    to->LIDAR_60 = ((raw));
    raw  = ((uint32_t)((bytes[1]) & 0x01)); ///< 1 bit(s) from B8
    to->LIDAR_80 = ((raw));
    raw  = ((uint32_t)((bytes[1] >> 2) & 0x03)); ///< 2 bit(s) from B10
    to->SONAR_left = ((raw));
    raw  = ((uint32_t)((bytes[1] >> 4) & 0x03)); ///< 2 bit(s) from B12
    to->SONAR_right = ((raw));
    raw  = ((uint32_t)((bytes[1] >> 6) & 0x03)); ///< 2 bit(s) from B14
    to->SONAR_back = ((raw));

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter

    return success;
}


/// Decode GEO's 'GPS_DATA' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool dbc_decode_GPS_DATA(GPS_DATA_t *to, const uint8_t bytes[8], const dbc_msg_hdr_t *hdr)
{
    const bool success = true;
    // If msg header is provided, check if the DLC and the MID match
    if (NULL != hdr && (hdr->dlc != GPS_DATA_HDR.dlc || hdr->mid != GPS_DATA_HDR.mid)) {
        return !success;
    }

    uint32_t raw;
    raw  = ((uint32_t)((bytes[0]))); ///< 8 bit(s) from B0
    raw |= ((uint32_t)((bytes[1]))) << 8; ///< 8 bit(s) from B8
    raw |= ((uint32_t)((bytes[2]))) << 16; ///< 8 bit(s) from B16
    raw |= ((uint32_t)((bytes[3]) & 0x0f)) << 24; ///< 4 bit(s) from B24
    to->GPS_LATITUDE = ((raw * 1e-06) + (-90.000000));
    raw  = ((uint32_t)((bytes[3] >> 4) & 0x0f)); ///< 4 bit(s) from B28
    raw |= ((uint32_t)((bytes[4]))) << 4; ///< 8 bit(s) from B32
    raw |= ((uint32_t)((bytes[5]))) << 12; ///< 8 bit(s) from B40
    raw |= ((uint32_t)((bytes[6]))) << 20; ///< 8 bit(s) from B48
    raw |= ((uint32_t)((bytes[7]) & 0x01)) << 28; ///< 1 bit(s) from B56
    to->GPS_LONGITUDE = ((raw * 1e-06) + (-180.000000));

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter

    return success;
}


/// Decode GEO's 'COMPASS' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool dbc_decode_COMPASS(COMPASS_t *to, const uint8_t bytes[8], const dbc_msg_hdr_t *hdr)
{
    const bool success = true;
    // If msg header is provided, check if the DLC and the MID match
    if (NULL != hdr && (hdr->dlc != COMPASS_HDR.dlc || hdr->mid != COMPASS_HDR.mid)) {
        return !success;
    }

    uint32_t raw;
    raw  = ((uint32_t)((bytes[0]))); ///< 8 bit(s) from B0
    raw |= ((uint32_t)((bytes[1]))) << 8; ///< 8 bit(s) from B8
    raw |= ((uint32_t)((bytes[2]))) << 16; ///< 8 bit(s) from B16
    raw |= ((uint32_t)((bytes[3]))) << 24; ///< 8 bit(s) from B24
    to->CMP_HEADING = ((raw));
    raw  = ((uint32_t)((bytes[4]))); ///< 8 bit(s) from B32
    raw |= ((uint32_t)((bytes[5]))) << 8; ///< 8 bit(s) from B40
    raw |= ((uint32_t)((bytes[6]))) << 16; ///< 8 bit(s) from B48
    raw |= ((uint32_t)((bytes[7]))) << 24; ///< 8 bit(s) from B56
    to->CMP_BEARING = ((raw));

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter

    return success;
}


/// Not generating code for dbc_decode_MOTOR_DATA() since 'ANDROID' is not the recipient of any of the signals

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

/// Handle the MIA for SENSOR's SENSOR_DATA message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter reaches the MIA threshold, MIA struct will be copied to *msg
static inline bool dbc_handle_mia_SENSOR_DATA(SENSOR_DATA_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const dbc_mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= SENSOR_DATA__MIA_MS);

    if (!msg->mia_info.is_mia) { // Not MIA yet, so keep incrementing the MIA counter
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { // Previously not MIA, but it is MIA now
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = SENSOR_DATA__MIA_MSG;
        msg->mia_info.mia_counter_ms = SENSOR_DATA__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

/// Handle the MIA for GEO's GPS_DATA message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter reaches the MIA threshold, MIA struct will be copied to *msg
static inline bool dbc_handle_mia_GPS_DATA(GPS_DATA_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const dbc_mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= GPS_DATA__MIA_MS);

    if (!msg->mia_info.is_mia) { // Not MIA yet, so keep incrementing the MIA counter
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { // Previously not MIA, but it is MIA now
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = GPS_DATA__MIA_MSG;
        msg->mia_info.mia_counter_ms = GPS_DATA__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

/// Handle the MIA for GEO's COMPASS message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter reaches the MIA threshold, MIA struct will be copied to *msg
static inline bool dbc_handle_mia_COMPASS(COMPASS_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const dbc_mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= COMPASS__MIA_MS);

    if (!msg->mia_info.is_mia) { // Not MIA yet, so keep incrementing the MIA counter
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { // Previously not MIA, but it is MIA now
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = COMPASS__MIA_MSG;
        msg->mia_info.mia_counter_ms = COMPASS__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

#endif
