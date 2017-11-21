
#include "sensor_functions.hpp"

extern Sonar_Sensor sonar;




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
        return true;
    }
    else
    {
        return false;
    }
}

//this function will send lane data ad sonar sensor data should
//be renamed appropriately
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

    printf("\n %f %f %f",sonar.backDist,sonar.leftDist,sonar.rightDist);

    if(dbc_encode_and_send_SENSOR_DATA(&SensorData))
        LE.toggle(4);

}

void send_distance_values()
{
    SENSOR_VALUES_t SensorValue;

    SensorValue.LIDAR_DISTANCE_CM = rplidar.distance_value_cm;
    SensorValue.LIDAR_ANGLE_DEG = rplidar.angle_value_deg;
    SensorValue.LIDAR_QUALITY_DEG = rplidar.quality_value;
    //add the Sonar Sensor code here
    dbc_encode_and_send_SENSOR_VALUES(&SensorValue);

}

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

void send_three_values(LIDAR_DATA_VALUES_t *from)
{
    dbc_encode_and_send_LIDAR_DATA_VALUES(from);
}


void send_lane_distance_values ()
{
    LIDAR_LANE_VALUES_t data;

    if(rplidar.flag)
    {
        data.LIDAR_DISTANCE_CM_N_40 = rplidar.lane_distances[2];
        data.LIDAR_DISTANCE_CM_N_20 = rplidar.lane_distances[3];
        data.LIDAR_DISTANCE_CM_0 = rplidar.lane_distances[4];
        data.LIDAR_DISTANCE_CM_P_20 = rplidar.lane_distances[5];
        data.LIDAR_DISTANCE_CM_P_40 = rplidar.lane_distances[6];

        for(uint8_t i = 2; i<=6;i++)
        {
            rplidar.lane_distances[i] = 0;
        }


        dbc_encode_and_send_LIDAR_LANE_VALUES(&data);
        rplidar.flag = false;
    }
}
