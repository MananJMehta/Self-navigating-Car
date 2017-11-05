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
#include <stdio.h>

enum lcd_led{Sensor, Motor, Comm, Geo};

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
    u2.putChar(0x01, portMAX_DELAY);   //Write Object
    u2.putChar(0x10, portMAX_DELAY);   //Object ID for speedometer
    u2.putChar(0x00, portMAX_DELAY);
    u2.putChar(0x00, portMAX_DELAY);   //MSB
    u2.putChar(random_speed, portMAX_DELAY); //LSB
    char checkSum = 0x01^0x10^0x00^0x00^ random_speed;
    u2.putChar(checkSum, portMAX_DELAY);
    ack();
}

void display_bus_reset()
{
    static char bus_reset_count;
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01, portMAX_DELAY);   //Write Object
    u2.putChar(0x09, portMAX_DELAY);   //Object ID for bus_reset
    u2.putChar(0x00, portMAX_DELAY);
    u2.putChar(0x00, portMAX_DELAY);   //MSB
    u2.putChar(++bus_reset_count, portMAX_DELAY); //LSB
    char checkSum = 0x01^0x09^0x00^0x00^ bus_reset_count;
    u2.putChar(checkSum, portMAX_DELAY);
    ack();
}

/**
void display_lidar_spectrum(char pos, char val)
{
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01);   //Write Object
    u2.putChar(0x18);   //Object ID for bus_reset
    u2.putChar(0x00);
    u2.putChar(pos);    //MSB
    u2.putChar(val);    //LSB
    char checkSum = 0x01^0x18^0x00^pos^ val;
    u2.putChar(checkSum);
}
**/

void display_lcd_led(char led_num, char state)
{
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01, portMAX_DELAY);   //Write Object
    u2.putChar(0x0E, portMAX_DELAY);   //Object ID for bus_reset
    u2.putChar(led_num, portMAX_DELAY);
    u2.putChar(0x00, portMAX_DELAY);
    u2.putChar(state, portMAX_DELAY);  //LSB
    char checkSum = 0x01^0x0E^led_num^0x00^state;
    u2.putChar(checkSum, portMAX_DELAY);
    ack();
}

void display_LCD_large_led(char led_num, char state)
{
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01, portMAX_DELAY);   //Write Object
    u2.putChar(0x13, portMAX_DELAY);   //Object ID for bus_reset
    u2.putChar(led_num, portMAX_DELAY);
    u2.putChar(0x00, portMAX_DELAY);
    u2.putChar(state, portMAX_DELAY); //LSB
    char checkSum = 0x01^0x13^led_num^0x00^state;
    u2.putChar(checkSum, portMAX_DELAY);
    ack();
}

void convert16_to_hex(uint16_t number, uint8_t *lsb_val, uint8_t *msb_val)
{
    *lsb_val = ((uint8_t)(number & 0xFF)) ;
    *msb_val = ((uint8_t)((number>>8) & 0xFF));
}

void display_lcd_numbers(char display_num, uint16_t value)
{
    uint8_t lsb, msb;
    convert16_to_hex(value, &lsb, &msb);
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01, portMAX_DELAY);   //Write Object
    u2.putChar(0x0F, portMAX_DELAY);   //Object ID for bus_reset
    u2.putChar(display_num, portMAX_DELAY);
    u2.putChar(msb, portMAX_DELAY);
    u2.putChar(lsb, portMAX_DELAY);    //LSB
    char checkSum = 0x01^0x0F^display_num^msb^lsb;
    u2.putChar(checkSum, portMAX_DELAY);
    ack();
}

void display_lcd_bar(char display_num, uint16_t value)
{
    uint8_t lsb, msb;
    convert16_to_hex(value, &lsb, &msb);
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01, portMAX_DELAY);   //Write Object
    u2.putChar(0x0B, portMAX_DELAY);   //Object ID for bus_reset
    u2.putChar(display_num, portMAX_DELAY);
    u2.putChar(msb, portMAX_DELAY);
    u2.putChar(lsb, portMAX_DELAY);    //LSB
    char checkSum = 0x01^0x0B^display_num^msb^lsb;
    u2.putChar(checkSum, portMAX_DELAY);
    ack();
}

char check_form()
{
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x00);
    u2.putChar(0x0A);
    u2.putChar(0x00);
    u2.putChar(0x0A);
    char hello[6] = {'\0'};
    char test[1] = {'\0'};
    char *input = hello;
    char *form_value = test;
    u2.getChar(input, portMAX_DELAY);
    u2.getChar(++input, portMAX_DELAY);
    u2.getChar(++input, portMAX_DELAY);
    u2.getChar(++input, portMAX_DELAY);
    u2.getChar(form_value, portMAX_DELAY);
    u2.getChar(++input, portMAX_DELAY);
    printf("%d\n", *form_value);
    return test[0];
}


void ack()
{
    Uart2& u2 = Uart2::getInstance();
       char test[1] = {'\0'};
        char *ack_val = test;
        u2.getChar(ack_val, portMAX_DELAY);
//        printf("%c", ack_val);
}


