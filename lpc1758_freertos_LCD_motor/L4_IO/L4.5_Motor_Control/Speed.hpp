/*
 * Speed.hpp

 *
 *  Created on: Oct 14, 2017
 *      Author: mehtamanan
 */

#ifndef SPEED_HPP_
#define SPEED_HPP_
#include "lpc_pwm.hpp"

#define VERYFAST    18.0  //Avoid Using.
#define FAST        17.0
#define MEDIUM      16.0
#define SLOW        15.6
#define VERYSLOW    15
#define STOP        14
#define REVERSESLOW 15
#define REVERSEFAST 15

class Speed: public PWM

{
public:

    bool init();

    float getSpeed();   ///< @returns floating-point reading of feedback rpm sensor
    void setSpeed(float speed); ///< @param sets speed of motor
    float FixedSpeed=0.0;
    ///Constructor of this Singleton class
    Speed():PWM(pwmType::pwm2, 8){}
};




#endif /* SPEED_HPP_ */
