#include "functions.h"

int readSensorValue () 
{
    return (int)mock();
}

int getMotorPWM()
{
    int sensorValue = readSensorValue();
    if(sensorValue>=0&&sensorValue<50)
    {
        return 8;
    }
    else if (sensorValue>=50&&sensorValue<=100)
    {
        return 9;
    }
    else if (sensorValue>100||sensorValue<0)
    {
        return -1;
    }

    return 1;
}

void lidar_init()
{

            lane[0].min = 270;
            lane[1].min = 290;
            lane[2].min = 310;
            lane[3].min = 330;
            lane[4].min = 350;
            lane[5].min = 10;
            lane[6].min = 30;
            lane[7].min = 50;
            lane[8].min = 70;
            lane[9].min = 90;
            lane[10].min = 110;
            lane[11].min = 130;
            lane[12].min = 150;
            lane[13].min = 170;
            lane[14].min = 190;
            lane[15].min = 210;
            lane[16].min = 230;
            lane[17].min = 250;

            lane[0].max = 290;
            lane[1].max = 310;
            lane[2].max = 330;
            lane[3].max = 350;
            lane[4].max = 10;
            lane[5].max = 30;
            lane[6].max = 50;
            lane[7].max = 70;
            lane[8].max = 90;
            lane[9].max = 110;
            lane[10].max = 130;
            lane[11].max = 150;
            lane[12].max = 170;
            lane[13].max = 190;
            lane[14].max = 210;
            lane[15].max = 230;
            lane[16].max = 250;
            lane[17].max = 270; 
}

bool angle_is_in_range_of_current_lane(uint8_t current_lane)
{
    if(current_lane != 4)
    {
        if (angle_value_deg >= lane[current_lane].min && angle_value_deg < lane[current_lane].max)
        {
            return true;
        }

        else
        {
            return false;
        }
    }
    else
    {
        if (((angle_value_deg >= lane[current_lane].min) && (angle_value_deg < 360)) | ((angle_value_deg >= 0) && (angle_value_deg < lane[current_lane].max)))
        {
            return true;
        }

        else
        {
            return false;
        }
    }

}

bool angle_is_in_range_of_next_lane(uint8_t next_lane)
{
    if(next_lane != 4)
    {
        if (angle_value_deg >= lane[next_lane].min && angle_value_deg < lane[next_lane].max)
        {
            return true;
        }

        else
        {
            return false;
        }
    }
    else
    {
        if (((angle_value_deg >= lane[next_lane].min) && (angle_value_deg < 360)) | ((angle_value_deg >= 0) && (angle_value_deg < lane[next_lane].max)))
        {
            return true;
        }

        else
        {
            return false;
        }
    }
}


bool lane_algorithm(void)
{
    static uint8_t count = 0;
    static bool local_lanes [18];
    static const float object_range = 200;
    static const uint8_t data = 5;
    static uint16_t min = 600;
    static uint8_t current_lane = 4;//start at lane 4
    static uint8_t next_lane = 5;


    //data should be starting from lane 4 so there should be only 2 if statement
    //one that does cheking and processing for current lane and one that checks fort the next lane
    if((angle_value_deg<360) && (angle_value_deg>0))
    {
    if(angle_is_in_range_of_current_lane(current_lane)) //in the range for current lane
    {
        //will catch all the data points minus the first
       if(distance_value_cm <= 600 && distance_value_cm>0)
        {
           if(distance_value_cm<min)
           {
               min = distance_value_cm;
           }

           if(distance_value_cm <= object_range)
            {
                count++;
            }
        }


    }
    //this if statement will:
    //-determine if prev lane was occupied
    //-determine the min value of the prev lane
    else if (angle_is_in_range_of_next_lane(next_lane))
    {
        //check if prev lane was accupied
        if(count>data)
        {
            local_lanes[current_lane] = true;
        }
        else
        {
            local_lanes[current_lane] = false;
        }

        lane_distances[current_lane] = min;//this will set lane_distance to the smallest value from prev lane
        min = 600;//set this equal to the first value in the lane
        count = 0;//reset the count for this lane
        if (next_lane == 17)
        {
            next_lane = 0;
            current_lane = 17;
        }
        else if (next_lane == 0)
        {
            next_lane = 1;
            current_lane = 0;
        }
        else
        {
            next_lane++;
            current_lane++;
        }

        if(distance_value_cm <= object_range && distance_value_cm>0)
        {
            if(distance_value_cm<min)
            {
                min = distance_value_cm;
            }
            count++;
        }
    }

    else// data is not in sync. Wait, but send out error msg
    {


        return false;
    }

    }

     memcpy(lane_lut, local_lanes, sizeof(lane_lut));
//    memcpy(lane_lut, local_lanes, sizeof(lane_lut));
    return true;
}


/*

void lane_algorithm()
{

    static uint8_t count = 0;
    static bool local_lanes [9];
    static const float object_range = 150.0;
    static const uint8_t data = 5;
    static uint8_t lane = 8;
    static uint16_t min = 0;

    if(distance_value_cm>0)
    {


    //this is for lane from -90 to -70
    if (angle_value_deg>=270&&angle_value_deg<290)
        {
            //if this is true this is the first time entering this loop
            //should only enter this loop once
            //this if statement checks te count of the previous lane and determines
            //if there i something in that lane, if so then that local_lanes bit is set true
            if(lane == 8)
            {
                if(count>data)
                {
                    local_lanes[8] = true;
                }
                else
                {
                    local_lanes[8] =false;
                }

                lane_distances[8] = min;//this will set lane_distance to the smallest value from prev lane
                min = distance_value_cm;//set this equal to the first value in the lane
                lane = 0;//change the lane value so we dont enter here again
                count = 0;//reset the count for this lane
            }

            //this will increase the count if there is an object in the 6m range
            //and set the min value seen
            if(distance_value_cm <= object_range && distance_value_cm>=0)
            {
                if(distance_value_cm<min)
                {
                    min = distance_value_cm;
                }
                count++;
            }
        }


        //-60
        else if(angle_value_deg>=290&&angle_value_deg<310)
        {
            if(lane == 0)
            {
                if(count>data)
                {
                    local_lanes[0] = false;
                }
                else
                {
                    local_lanes[0] = true;
                }

                count = 0;
            }

            if(distance_value_cm <= object_range&&distance_value_cm>1)
            {
                lane_distances[1] = distance_value_cm;
                count++;
            }
            lane = 1;

        }

        //-40
        else if(angle_value_deg>=310&&angle_value_deg<330)
        {
            if(lane == 1)
            {
                if(count>data)
                {
                    local_lanes[1] = false;
                }
                else
                {
                    local_lanes[1] = true;
                }

                count = 0;
            }

            if(distance_value_cm <= object_range&&distance_value_cm>1)
            {
                lane_distances[2] = distance_value_cm;
                count++;
            }

            lane = 2;
        }

        //-20
        else if(angle_value_deg>=330&&angle_value_deg<350)
        {
            if(lane == 2)
            {
                if(count>data)
                {
                    local_lanes[2] = false;
                }
                else
                {
                    local_lanes[2] = true;
                }

                count = 0;
            }

            if(distance_value_cm <= object_range&&distance_value_cm>1)
            {
                lane_distances[3] = distance_value_cm;
                count++;
            }
            lane = 3;
        }

        //0
        else if((angle_value_deg>=350&&angle_value_deg<360) | (angle_value_deg>=0&&angle_value_deg<10))
        {
            if(lane == 3)
            {
                if(count>data)
                {
                    local_lanes[3] = false;
                }
                else
                {
                    local_lanes[3] = true;
                }

                count = 0;
            }

            if(distance_value_cm <= object_range&&distance_value_cm>1)
            {
                lane_distances[4] = distance_value_cm;
                count++;
            }



            lane = 4;
        }

        //20
        else if(angle_value_deg>=10&&angle_value_deg<30)
        {
            if(lane == 4)
            {
                if(count>data)
                {
                    local_lanes[4] = false;
                }
                else
                {
                    local_lanes[4] = true;
                }
                count = 0;
            }

            if(distance_value_cm <= object_range&&distance_value_cm>1)
            {
                lane_distances[5] = distance_value_cm;
                count++;
            }


            lane = 5;
        }

        //40
        else if(angle_value_deg>=30&&angle_value_deg<50)
        {
            if(lane == 5)
            {
                if(count>data)
                {
                    local_lanes[5] = false;
                }
                else
                {
                    local_lanes[5] = true;
                }

                count = 0;
            }

            if(distance_value_cm <= object_range&&distance_value_cm>1)
            {
                lane_distances[6] = distance_value_cm;
                count++;
            }


            lane = 6;
        }

        //60
        else if(angle_value_deg>=50&&angle_value_deg<70)
        {
            if(lane == 6)
            {
                if(count>data)
                {
                    local_lanes[6] = false;
                }
                else
                {
                    local_lanes[6] = true;
                }

                count = 0;
            }

            if(distance_value_cm <= object_range&&distance_value_cm>1)
            {
                lane_distances[7] = distance_value_cm;
                count++;
            }


            lane = 7;
        }

        //80
        else if(angle_value_deg>=70&&angle_value_deg<90)
        {
            if(lane == 7)
            {
                if(count>data)
                {
                    local_lanes[7] = false;
                }
                else
                {
                    local_lanes[7] = true;
                }
                count = 0;
            }

            if(distance_value_cm <= object_range&&distance_value_cm>1)
            {
                lane_distances[8] = distance_value_cm;
                count++;
            }



            lane = 8;
        }

        memcpy(lane_lut, local_lanes, sizeof(lane_lut));

    }

}


*/