#include "motor.h"





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
