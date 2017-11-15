/*
 * Speed.cpp
 *
 *  Created on: Oct 14, 2017
 *      Author: mehtaman
 */

#include "Speed.hpp"
//#include "IO.hpp"
#include "speed_util.cpp"

bool Speed::init()
{
    this->setSpeed(STOP);
    //    LE.on(1);
    return true;
}

float Speed::getSpeed()   ///< @returns floating-point reading of feedback rpm sensor
{
    return this->FixedSpeed;
}
void Speed::setSpeed(float speed) ///< @param sets floating-point speed of motor
{
    //speed values start from ~15.4 for forward. values uptil 18 have been tested to be very fast
    this->set(speed);
    this->FixedSpeed=speed;
}


float Speed::maintain_speed(float val)
{
    check_speed_diff(rpm_s.rpms, RefCts.ref_count_medium, this->getSpeed(), &val);
    return val;
}

float Speed::rpm_meter()
{
    rpm_mtr_val(&rpm_s.second_count, &rpm_s.rps, &rpm_s.cut_count, &rpm_s.old_count, &rpm_s.speed,&rpm_s.old_countms);
    return rpm_s.speed;
}

float Speed::speed_check(bool flag, float val)
{
    if(flag == true)
    {
        rpm_s.rpms = rpm_s.cut_count - rpm_s.old_countms;
        rpm_s.old_countms = rpm_s.cut_count;
        // LD.setNumber(rpm_s.rpms);
        return this->maintain_speed(val);
    }
    return STOP;
}

