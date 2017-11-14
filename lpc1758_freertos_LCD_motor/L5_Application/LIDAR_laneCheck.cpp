#include "_can_dbc/generated_can.h"

void check_lane(SENSOR_DATA_t sensor_can_msg)
{
    if(sensor_can_msg.LIDAR_0)
    {
        if(sensor_can_msg.LIDAR_20)
        {
            //checked
            if (!sensor_can_msg.LIDAR_neg20)
            {

                if(!sensor_can_msg.LIDAR_neg40)
                {
                    //turn left
                }
                else if (!sensor_can_msg.LIDAR_40)
                {
                    //turn right *condition
                }
                else
                {//stop;
                }
            }
            //checked
            else if (sensor_can_msg.LIDAR_neg20)
            {
                //lower priority?? //shouldn't com into picture due to  initial condition
                if(!sensor_can_msg.LIDAR_neg40)
                {
                    //turn left *condition
                }
                else if (!sensor_can_msg.LIDAR_40)
                {
                    //turn right *condition
                }
                else
                {//stop;
                }
            }

        }
        //checked
        else if (!sensor_can_msg.LIDAR_20)
        {
            if (sensor_can_msg.LIDAR_neg20)
            {
                if(!sensor_can_msg.LIDAR_40)
                {
                    //turn right
                }
                else if (!sensor_can_msg.LIDAR_neg40)
                {
                    //turn left //* condition
                }
                else
                {//stop;
                }
            }
            //end checked
            //checked
            else if (!sensor_can_msg.LIDAR_neg20)
            {

                if(!sensor_can_msg.LIDAR_40)
                {
                    //turn right
                }
                else if (!sensor_can_msg.LIDAR_neg40)
                {
                    //turn left
                }
                else
                {//stop;
                }
            }
            //end check
        }
    }
    else if(!sensor_can_msg.LIDAR_0)
    {
        if(!sensor_can_msg.LIDAR_20)
        {//checked
            if(!sensor_can_msg.LIDAR_neg20)
            {
                // go straight?? nothing on 0, 20 and n20
            }
            else if (sensor_can_msg.LIDAR_neg20)
            {
                if(!sensor_can_msg.LIDAR_40)
                {
                    //turn right
                }
                else if (!sensor_can_msg.LIDAR_neg40)
                {
                    //turn left *condition
                }
                else
                {
                    //stop;
                }
            }
            //endcheck
        }
        else if(sensor_can_msg.LIDAR_20)
        {//checked
            if(!sensor_can_msg.LIDAR_neg20)
            {
                if(!sensor_can_msg.LIDAR_neg40)
                {
                    //turn left
                }
                else if (!sensor_can_msg.LIDAR_40)
                {
                    //turn right *condition
                }
                else
                {//stop;
                }
                //endcheck
            }
            else if (sensor_can_msg.LIDAR_neg20)
            {//all *conditions
                //checked
                if(!sensor_can_msg.LIDAR_40)
                {
                    //turn right
                }
                else if (!sensor_can_msg.LIDAR_neg40)
                {
                    //turn left
                }
                else
                {//stop;
                }
                //checked
            }
        }
    }

}

