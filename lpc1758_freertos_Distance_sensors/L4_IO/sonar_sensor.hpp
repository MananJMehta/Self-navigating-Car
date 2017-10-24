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

typedef enum {
    fallingedge,
    risingedge
}interruptOn;

typedef enum {
    left,
    center,
    right
}position;

class Sonar_Sensor //: public SingletonTemplate<Sonar_Sensor>
{
    public:
        void start_operation();
        static void sensor_detect_fall_left();
        static void sensor_detect_rise_left();
        static void sensor_detect_fall_center();
        static void sensor_detect_rise_center();
        static void sensor_detect_fall_right();
        static void sensor_detect_rise_right();
        bool init();
        Sonar_Sensor() {}
    private:
        //int pin;
        //friend class SingletonTemplate<Sonar_Sensor>;
};

#endif
