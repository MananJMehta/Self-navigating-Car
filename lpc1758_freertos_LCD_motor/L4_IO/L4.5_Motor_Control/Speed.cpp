/*
 * Speed.cpp
 *
 *  Created on: Oct 14, 2017
 *      Author: mehtaman
 */

#include "Speed.hpp"

#define Motor      Speed::getInstance()            ///< 4 On-board Switches

bool init()
{
	return true;
}

float getSpeed()   ///< @returns floating-point reading of feedback rpm sensor
{
	return 0.00;
}
void setSpeed(Speed::speedOfCar speed) ///< @param sets floating-point speed of motor
{

    Motor.MotorSpeed();
}


