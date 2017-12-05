/*
 * Speed.hpp

 *
 *  Created on: Oct 14, 2017
 *      Author: mehtamanan
 */

#ifndef SPEED_HPP_
#define SPEED_HPP_
#include "lpc_pwm.hpp"
#include "rpm_Check.hpp"


#define VERYFAST    18.0  //Avoid Using.
#define FAST        17.0
#define MEDIUM      16.0
#define SLOW        15.6
#define VERYSLOW    15
#define STOP        14
#define SLOWREVERSE 12.6
#define REVERSEFAST 12

class Speed: public PWM

{
public:

    bool init();
    RPM_s rpm_s;
    ReferenceCounts RefCts;
    float getSpeed();   ///< @returns floating-point reading of feedback rpm sensor
    void setSpeed(float speed); ///< @param sets speed of motor
    float FixedSpeed=0.0;
    float maintain_speed(float val);
    float rpm_meter();
    float speed_check(bool flag,float val);
    ///Constructor of this Singleton class
    Speed():PWM(pwmType::pwm2, 8){}
};





#endif /* SPEED_HPP_ */
