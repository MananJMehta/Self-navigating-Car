/*
 * Speed.hpp

 *
 *  Created on: Oct 14, 2017
 *      Author: mehtamanan
 */

#ifndef SPEED_HPP_
#define SPEED_HPP_
#include "lpc_pwm.hpp"
#include "string"
#include <stdint.h>



class Speed : public SingletonTemplate<Speed>
{
public:

	bool init();

	float getSpeed();   ///< @returns floating-point reading of feedback rpm sensor
	void setSpeed(speedOfCar value); ///< @param sets floating-point speed of motor
	//string getName = "Brushless Motor Speed Class";  ///< @returns short description of class
	PWM MotorSpeed;
private: 
	/// Private constructor of this Singleton class
	Speed() {MotorSpeed(PWM::pwm2, 8);}
	friend class SingletonTemplate<Speed>;  ///< Friend class used for Singleton Template
};

typedef enum {
    VERYFAST,
    FAST,
    MEDIUM,
    SLOW,
    VERYSLOW
}speedOfCar;

#endif /* SPEED_HPP_ */
