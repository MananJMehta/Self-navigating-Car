/*
 * Speed.hpp

 *
 *  Created on: Oct 14, 2017
 *      Author: mehtamanan
 */

#ifndef SPEED_HPP_
#define SPEED_HPP_
#include "lpc_pwm.hpp"

class Speed: public PWM

{
public:
    typedef enum {
        VERYFAST,
        FAST,
        MEDIUM,
        SLOW,
        VERYSLOW,
        Stop,
        ReverseSlow,
        ReverseFast
    }speedOfCar;

    bool init();

    float getSpeed();   ///< @returns floating-point reading of feedback rpm sensor
    void setSpeed(speedOfCar speed); ///< @param sets speed of motor

    ///Constructor of this Singleton class
    Speed():PWM(pwmType::pwm2, 8){}
};




#endif /* SPEED_HPP_ */
