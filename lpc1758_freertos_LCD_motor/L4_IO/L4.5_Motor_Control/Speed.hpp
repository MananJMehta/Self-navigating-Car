/*
 * Speed.hpp

 *
 *  Created on: Oct 14, 2017
 *      Author: mehtamanan
 */

#ifndef SPEED_HPP_
#define SPEED_HPP_
#include "lpc_pwm.hpp"

#define VERYFAST    18
#define FAST        17.5
#define MEDIUM      17
#define SLOW        15.5
#define VERYSLOW    15.4
#define STOP        15
#define REVERSESLOW 15
#define REVERSEFAST 15

class Speed: public PWM

{
public:

    bool init();

    float getSpeed();   ///< @returns floating-point reading of feedback rpm sensor
    void setSpeed(int speed); ///< @param sets speed of motor
    float FixedSpeed=0.0;
    ///Constructor of this Singleton class
    Speed():PWM(pwmType::pwm2, 8){}
};




#endif /* SPEED_HPP_ */
