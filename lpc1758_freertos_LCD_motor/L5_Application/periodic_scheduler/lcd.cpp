/*
 * lcd.cpp
 *
 *  Created on: Oct 25, 2017
 *      Author: venka
 */
#include "uart2.hpp"
#include <cstdlib>
#include <ctime>
#include "lcd.hpp"

int get_random_int(int max_val)
{
    srand((unsigned) time(0));
    int random_integer = rand();
    int value = random_integer % max_val;
    return value;
}

void display_speedometer(char random_speed)
{
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01);   //Write Object
    u2.putChar(0x10);   //Object ID for speedometer
    u2.putChar(0x00);
    u2.putChar(0x00);   //MSB
    u2.putChar(random_speed); //LSB
    char checkSum = 0x01^0x10^0x00^0x00^ random_speed;
    u2.putChar(checkSum);
}


