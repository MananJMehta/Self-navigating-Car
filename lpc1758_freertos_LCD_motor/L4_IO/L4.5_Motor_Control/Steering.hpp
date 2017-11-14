/*
 * Steering.h
 *
 *  Created on: Oct 14, 2017
 *      Author: mehtaman
 */

#ifndef STEERING_HPP_
#define STEERING_HPP_
#include "lpc_pwm.hpp"

#define EXTREMELEFT 11
#define EXTREMERIGHT 19
#define HARDLEFT 12
#define LEFT 13
#define SOFTLEFT 14
#define CENTER 15
#define SOFTRIGHT 16
#define RIGHT 17
#define HARDRIGHT 18

class Steering: public PWM

{
public:

    bool init();

    int getDirection();   ///< @returns value set previously
    void setDirection(float value); ///< @param sets direction of car

    ///Constructor
    Steering():PWM(pwmType::pwm3, 8){}
    int currentDirection;
};


#endif /* STEERING_HPP_ */
