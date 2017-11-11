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
void Speed::setSpeed(float speed) ///< @param sets floating-point speed of motor
{
    //speed values start from ~15.4 for forward. values uptil 18 have been tested to be very fast
    this->set(speed);
    this->FixedSpeed=speed;
}


float Speed::maintain_speed(float val)
{
    if((rpm_s.rpms - RefCts.ref_count_medium) > 1)
    {
        val=this->getSpeed() - 0.1;
        LE.on(3);
    }
    else if((RefCts.ref_count_medium - rpm_s.rpms) > 1)
    {
        val=this->getSpeed() + 0.1;
        LE.on(4);
    }
    else
    {
        //        LE.off(3);
        //        LE.off(4);
    }
    return val;
}

float Speed::rpm_meter()
{
    rpm_s.second_count++;
    rpm_s.rps = rpm_s.cut_count - rpm_s.old_count;
    // rpm_s.speed = rpm_s.rps/2*36.5*3600/(100*1000);
    // printf("RPS: %d Speed: %f\n", rps, speed);
    LD.setNumber(rpm_s.rpms);
    rpm_s.speed = (36.5 * rpm_s.rps * 3600) / (4*1000*100);
    rpm_s.ReferenceRps = rpm_s.ReferencSpeed * 4* 1000 *100 / (36.5 * 3600);
    rpm_s.old_count = rpm_s.cut_count;

    if(rpm_s.second_count == 60)
    {
        rpm_s.second_count = 0;
        rpm_s.cut_count = 0;
        rpm_s.old_count = 0;
        rpm_s.old_countms=0;
    }
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
