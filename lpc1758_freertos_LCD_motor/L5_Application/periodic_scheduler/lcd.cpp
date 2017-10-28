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

enum lcd_led{Sensor, Motor, Comm, Geo};

//TODO Acknowledgment from LCD screen
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

void display_bus_reset()
{
    static char bus_reset_count;
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01);   //Write Object
    u2.putChar(0x09);   //Object ID for bus_reset
    u2.putChar(0x00);
    u2.putChar(0x00);   //MSB
    u2.putChar(++bus_reset_count); //LSB
    char checkSum = 0x01^0x09^0x00^0x00^ bus_reset_count;
    u2.putChar(checkSum);
}

void display_lidar_spectrum(char pos, char val)
{
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01);   //Write Object
    u2.putChar(0x18);   //Object ID for bus_reset
    u2.putChar(0x00);
    u2.putChar(pos);   //MSB
    u2.putChar(val); //LSB
    char checkSum = 0x01^0x18^0x00^pos^ val;
    u2.putChar(checkSum);
}

void display_lcd_led(char led_num, char state)
{
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01);   //Write Object
    u2.putChar(0x0E);   //Object ID for bus_reset
    u2.putChar(led_num);
    u2.putChar(0x00);
    u2.putChar(state); //LSB
    char checkSum = 0x01^0x0E^led_num^0x00^state;
    u2.putChar(checkSum);
}

void display_LCD_health(char led_num, char state)
{
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01);   //Write Object
    u2.putChar(0x13);   //Object ID for bus_reset
    u2.putChar(led_num);
    u2.putChar(0x00);
    u2.putChar(state); //LSB
    char checkSum = 0x01^0x13^led_num^0x00^state;
    u2.putChar(checkSum);
}


