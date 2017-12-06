
#include "sensor_functions.hpp"

extern Sonar_Sensor sonar;

bool sync_flag = true;//set this false for actual run
bool noop_flag = false;
bool reboot_flag = false;


void initialize_can()
{
    CAN_init(can1, 100, 10, 10, NULL, NULL);
    const can_std_id_t slist[] = { CAN_gen_sid(can1, 100), CAN_gen_sid(can1, 120)};
    CAN_setup_filter(slist, 2, NULL, 0, NULL, 0, NULL, 0);
    CAN_reset_bus(can1);
}

bool check_heartbeat()
{
    HEARTBEAT_t heart;
    dbc_msg_hdr_t heart_hdr;
    can_msg_t rx_msg;

    if(CAN_rx(can1,&rx_msg,1) && (rx_msg.msg_id == 120))
    {
            LE.toggle(3);
    }
    if(dbc_decode_HEARTBEAT(&heart, rx_msg.data.bytes, &heart_hdr))
    {
        //write some code here
        if(heart.HEARTBEAT_cmd == HEARTBEAT_cmd_SYNC)
        {
            sync_flag = true;
        }
        else if(heart.HEARTBEAT_cmd == HEARTBEAT_cmd_NOOP)
        {
            noop_flag = true;
        }
        else if(heart.HEARTBEAT_cmd == HEARTBEAT_cmd_REBOOT)
        {
            reboot_flag = true;
        }
        else
        {
            //die
        }

        return true;
    }
    else
    {
        return false;
    }
}

///thinking about seperating sonar and lidar data
void send_lidar_sonar_data()
{
    SENSOR_DATA_t SensorData;

    SensorData.LIDAR_neg80 = rplidar.lane_lut[0];
    SensorData.LIDAR_neg60 = rplidar.lane_lut[1];
    SensorData.LIDAR_neg40 = rplidar.lane_lut[2];
    SensorData.LIDAR_neg20 = rplidar.lane_lut[3];
    SensorData.LIDAR_0 = rplidar.lane_lut[4];
    SensorData.LIDAR_20 = rplidar.lane_lut[5];
    SensorData.LIDAR_40 = rplidar.lane_lut[6];
    SensorData.LIDAR_60 = rplidar.lane_lut[7];
    SensorData.LIDAR_80 = rplidar.lane_lut[8];
    SensorData.SONAR_left = sonar.leftDist;
    SensorData.SONAR_right = sonar.rightDist;
    SensorData.SONAR_back = sonar.backDist;

    //why are we printing?? Remove this if its not needed
    printf("\n %f %f %f",sonar.backDist,sonar.leftDist,sonar.rightDist);
    if(sync_flag && !noop_flag)
    {
        if(dbc_encode_and_send_SENSOR_DATA(&SensorData))
            LE.toggle(4);
    }

}

void send_lidar_lane_data()
{
    LIDAR_LANES_t LaneData;

    LaneData.LIDAR_neg160 = rplidar.lane_lut[14];
    LaneData.LIDAR_neg140 = rplidar.lane_lut[15];
    LaneData.LIDAR_neg120 = rplidar.lane_lut[16];
    LaneData.LIDAR_neg100 = rplidar.lane_lut[17];
    LaneData.LIDAR_neg80 = rplidar.lane_lut[0];
    LaneData.LIDAR_neg60 = rplidar.lane_lut[1];
    LaneData.LIDAR_neg40 = rplidar.lane_lut[2];
    LaneData.LIDAR_neg20 = rplidar.lane_lut[3];
    LaneData.LIDAR_0 = rplidar.lane_lut[4];
    LaneData.LIDAR_20 = rplidar.lane_lut[5];
    LaneData.LIDAR_40 = rplidar.lane_lut[6];
    LaneData.LIDAR_60 = rplidar.lane_lut[7];
    LaneData.LIDAR_80 = rplidar.lane_lut[8];
    LaneData.LIDAR_100 = rplidar.lane_lut[9];
    LaneData.LIDAR_120 = rplidar.lane_lut[10];
    LaneData.LIDAR_140 = rplidar.lane_lut[11];
    LaneData.LIDAR_160 = rplidar.lane_lut[12];
    LaneData.LIDAR_180 = rplidar.lane_lut[13];
    if(sync_flag && !noop_flag)
    {
        dbc_encode_and_send_LIDAR_LANES(&LaneData);
    }
}


//this was a test and has now benn deprecated, remove at yor earliest convenience
void send_distance_values()
{
    SENSOR_VALUES_t SensorValue;

    SensorValue.LIDAR_DISTANCE_CM = rplidar.distance_value_cm;
    SensorValue.LIDAR_ANGLE_DEG = rplidar.angle_value_deg;
    SensorValue.LIDAR_QUALITY_DEG = rplidar.quality_value;
    //add the Sonar Sensor code here
    if(sync_flag && !noop_flag)
    {
        dbc_encode_and_send_SENSOR_VALUES(&SensorValue);
    }

}


//this is deprecated
void add_some_data_to_msg(LIDAR_DATA_VALUES_t *from)
{
    static uint8_t count = 0;

    if (count == 0)
    {
        from->LIDAR_DISTANCE_CM_1 = rplidar.distance_value_cm;
        from->LIDAR_ANGLE_DEG_1 = rplidar.angle_value_deg;
        count++;
    }
    else if (count == 1)
    {
        from->LIDAR_DISTANCE_CM_2 = rplidar.distance_value_cm;
        from->LIDAR_ANGLE_DEG_2 = rplidar.angle_value_deg;
        count++;
    }
    else if (count == 2)
    {
        from->LIDAR_DISTANCE_CM_3 = rplidar.distance_value_cm;
        from->LIDAR_ANGLE_DEG_3 = rplidar.angle_value_deg;
        count = 0;
    }
}

//this is deprecated
void send_three_values(LIDAR_DATA_VALUES_t *from)
{
    if(sync_flag && !noop_flag)
    {
        dbc_encode_and_send_LIDAR_DATA_VALUES(from);
    }
}


void send_lane_distance_values ()
{
    LIDAR_VALUES_0_TO_P100_t data1;
    LIDAR_VALUES_P120_TO_N140_t data2;
    LIDAR_VALUES_N120_TO_N20_t data3;

    if(rplidar.flag)
    {
        data1.LIDAR_DISTANCE_CM_0 = rplidar.lane_distances[4];
        data1.LIDAR_DISTANCE_CM_P_20 = rplidar.lane_distances[5];
        data1.LIDAR_DISTANCE_CM_P_40 = rplidar.lane_distances[6];
        data1.LIDAR_DISTANCE_CM_P_60= rplidar.lane_distances[7];
        data1.LIDAR_DISTANCE_CM_P_80 = rplidar.lane_distances[8];
        data1.LIDAR_DISTANCE_CM_P_100 = rplidar.lane_distances[9];

        data2.LIDAR_DISTANCE_CM_P_120 = rplidar.lane_distances[10];
        data2.LIDAR_DISTANCE_CM_P_140 = rplidar.lane_distances[11];
        data2.LIDAR_DISTANCE_CM_P_160 = rplidar.lane_distances[12];
        data2.LIDAR_DISTANCE_CM_P_180 = rplidar.lane_distances[13];
        data2.LIDAR_DISTANCE_CM_N_160 = rplidar.lane_distances[14];
        data2.LIDAR_DISTANCE_CM_N_140 = rplidar.lane_distances[15];

        data3.LIDAR_DISTANCE_CM_N_120 = rplidar.lane_distances[16];
        data3.LIDAR_DISTANCE_CM_N_100 = rplidar.lane_distances[17];
        data3.LIDAR_DISTANCE_CM_N_80 = rplidar.lane_distances[0];
        data3.LIDAR_DISTANCE_CM_N_60 = rplidar.lane_distances[1];
        data3.LIDAR_DISTANCE_CM_N_40 = rplidar.lane_distances[2];
        data3.LIDAR_DISTANCE_CM_N_20 = rplidar.lane_distances[3];

        if(sync_flag && !noop_flag)
        {
            dbc_encode_and_send_LIDAR_VALUES_0_TO_P100(&data1);
            dbc_encode_and_send_LIDAR_VALUES_P120_TO_N140(&data2);
            dbc_encode_and_send_LIDAR_VALUES_N120_TO_N20(&data3);
        }

        rplidar.flag = false;
    }
}
