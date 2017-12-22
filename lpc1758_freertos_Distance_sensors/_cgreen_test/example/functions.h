#ifndef FUNCTIONS_H__
#define FUNCTIONS_H__
#ifdef __cplusplus
extern "C" {
#endif


uint16_t lane_distances[18] = {0};
uint16_t distance_value_cm = 0;
uint16_t angle_value_deg = 0;
bool lane_lut[18] = {0};

struct lane_params
{
    uint16_t min;//min angle value of this lane
    uint16_t max;
};

int16_t test_array[37] = 
{   -1, 0, 1,
    9, 10, 11,
    29, 30, 31,
    49, 50, 51,
    69, 70, 71, 
    89, 90, 91,
    180,
    269, 270, 271,
    289, 290, 291,
    309, 310, 311,
    329, 330, 331,
    349, 350, 351,
    359, 360, 361
    };

bool bunk_lanes_low[18] =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool bunk_lanes_high[18] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int16_t test_distance_values[7] = {-1, 0, 1, 300, 600, 601, 200};

struct lane_params lane[18];//this should be placed .hpp and initialized in init

void lidar_init(void);

int getMotorPWM(void);

int readSensorValue(void);

bool lane_algorithm(void);

bool angle_is_in_range_of_next_lane(uint8_t next_lane);

bool angle_is_in_range_of_current_lane(uint8_t current_lane);



#ifdef __cplusplus
}
#endif
#endif /* FUNCTIONS_H__ */
