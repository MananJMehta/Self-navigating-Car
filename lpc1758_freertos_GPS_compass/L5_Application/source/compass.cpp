/*
 * compass.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: deshm
 */

#include "stdio.h"
#include "compass.hpp"
#include "io.hpp"
#include "utilities.h"

float Compass::Get_Compass_Heading()
{
        int heading_int = 0;
        float heading_float = 0;
        uint8_t buffer[2] = { 0 };

        if (I2C2::getInstance().readRegisters(COMPASS_I2C_ADDR, COMPASS_BEARING_16BIT_REG_ADDR, &buffer[0], READ_TWO_REGISTERS))
        {
            heading_int = buffer[0];
            heading_int <<= 8;
            heading_int += buffer[1];
            heading_float = (float)(heading_int/10.00);
            return heading_float;
        }
        return 0;
}


void Compass::Calibrate_Compass()
{
    uint8_t buffer[1] = { 0 };
    buffer[0] = 0xF0;
    I2C2::getInstance().writeRegisters(COMPASS_I2C_ADDR, COMMAND_REG_ADDR, &buffer[0], READ_ONE_REGISTER);
    delay_ms(30);
    buffer[0] = 0xF5;
    I2C2::getInstance().writeRegisters(COMPASS_I2C_ADDR, COMMAND_REG_ADDR, &buffer[0], READ_ONE_REGISTER);
    delay_ms(30);
    buffer[0] = 0xF6;
    I2C2::getInstance().writeRegisters(COMPASS_I2C_ADDR, COMMAND_REG_ADDR, &buffer[0], READ_ONE_REGISTER);
}


void Compass::Stop_Calibrate()
{
    uint8_t buffer[1] = { 0 };
    buffer[0] = 0xF8;
    I2C2::getInstance().writeRegisters(COMPASS_I2C_ADDR, COMMAND_REG_ADDR, &buffer[0], READ_ONE_REGISTER);
}

void Compass::Original_Firmware_Calibration()
{
    uint8_t buffer[1] = { 0 };
    buffer[0] = 0x20;
    I2C2::getInstance().writeRegisters(COMPASS_I2C_ADDR, COMMAND_REG_ADDR, &buffer[0], READ_ONE_REGISTER);
    delay_ms(30);
    buffer[0] = 0x2A;
    I2C2::getInstance().writeRegisters(COMPASS_I2C_ADDR, COMMAND_REG_ADDR, &buffer[0], READ_ONE_REGISTER);
    delay_ms(30);
    buffer[0] = 0x60;
    I2C2::getInstance().writeRegisters(COMPASS_I2C_ADDR, COMMAND_REG_ADDR, &buffer[0], READ_ONE_REGISTER);
}
