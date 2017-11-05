/*
 * Speed.cpp
 *
 *  Created on: Oct 14, 2017
 *      Author: mehtaman
 */

#include "Speed.hpp"
#include "IO.hpp"



bool Speed::init()
{
    this->setSpeed(STOP);
    LE.on(1);
    return true;
}

float Speed::getSpeed()   ///< @returns floating-point reading of feedback rpm sensor
{
    return this->FixedSpeed;
}
void Speed::setSpeed(int speed) ///< @param sets floating-point speed of motor
{
    //speed values start from ~15.4 for forward. values uptil 18 have been tested to be very fast
            this->set(speed);
            this->FixedSpeed=speed;
}


