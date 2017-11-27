/*
 * copass.hpp
 *
 *  Created on: Nov 20, 2017
 *      Author: deshm
 */

#ifndef COMPASS_HPP_
#define COMPASS_HPP_



#define COMPASS_I2C_ADDR 0xc0                   //I2C Address of Compass
#define COMMAND_REG_ADDR 0x00                   //Address of command register of compass
#define COMPASS_BEARING_16BIT_REG_ADDR 0x02     //Address of register containing compass bearing
#define READ_ONE_REGISTER 1                     //Read one register
#define READ_TWO_REGISTERS 2                    //Read two registers

class Compass
{
    public:
        float Get_Compass_Heading();
        void Calibrate_Compass();
        void Stop_Calibrate();
        void Original_Firmware_Calibration();
};


#endif /* COMPASS_HPP_ */
