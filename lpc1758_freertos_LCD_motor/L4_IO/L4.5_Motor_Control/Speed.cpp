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
    this->setSpeed(Stop);
    LE.on(1);
    return true;
}

float Speed::getSpeed()   ///< @returns floating-point reading of feedback rpm sensor
{
    return 0.00;
}
void Speed::setSpeed(Speed::speedOfCar speed) ///< @param sets floating-point speed of motor
{
//speed values start from ~15.4 for forward. values uptil 18 have been tested to be very fast
    switch(speed)
    {
        case  VERYFAST:
            this->set(15.8);
            break;
        case FAST:
            break;
        case MEDIUM:
            break;
        case SLOW:
            break;
        case VERYSLOW:
            break;
        case Stop:
            break;
        case ReverseSlow:
            this->set(0);
            break;
        case ReverseFast:
            this->set(0);
            break;
    }
}


