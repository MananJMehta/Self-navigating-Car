/*
 * sonar_sensor.hpp
 *
 *  Created on: 21-Oct-2017
 *      Author: Shashank Iyer
 */
#ifndef SONAR_SENSOR_HPP_
#define SONAR_SENSOR_HPP_
#include <stdint.h>
#include "gpio.hpp"


/*
 * Sonar Sensor class used to get sonar reading from the MaxSonar board using the PWM
 * add more details here.....
 * 
 * @ingroup BoardIO
 */

class Sonar_Sensor //: public SingletonTemplate<Sonar_Sensor>
{
    public:
        bool obstacle_1();
        bool obstacle_2();
        bool obstacle_3();
        void start_operation();
        bool sensor_detect_fall();
        void init();
        Sonar_Sensor() {}
    private:

        //friend class SingletonTemplate<Sonar_Sensor>;
};

#endif
