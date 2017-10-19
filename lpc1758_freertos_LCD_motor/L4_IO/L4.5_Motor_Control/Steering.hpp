/*
 * Steering.h
 *
 *  Created on: Oct 14, 2017
 *      Author: mehtaman
 */

#ifndef STEERING_HPP_
#define STEERING_HPP_
#include "lpc_pwm.hpp"

class Steering: public PWM

{
public:
    typedef enum {
        ExtremeLeft,
        ExtremeRight,
        HardLeft,
        Left,
        SoftLeft,
        Center,
        SoftRight,
        Right,
        HardRight
    }directionOfCar;

    bool init();

    directionOfCar getDirection();   ///< @returns value set previously
    void setDirection(directionOfCar value); ///< @param sets direction of car

    ///Constructor
    Steering():PWM(pwmType::pwm1, 8){}
    directionOfCar currentDirection;
};


#endif /* STEERING_HPP_ */
