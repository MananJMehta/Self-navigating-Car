/*
 * lcd.cpp
 *
 *  Created on: Oct 25, 2017
 *      Author: venka
 */
#include "lcd.hpp"


enum lcd_led{Sensor, Motor, Comm, Geo};
int get_random_int(int max_val)
{
    srand((unsigned) time(0));
    int random_integer = rand();
    int value = random_integer % max_val;
    return value;
}

void convert16_to_hex(uint16_t number, uint8_t *lsb_val, uint8_t *msb_val)
{
    *lsb_val = ((uint8_t)(number & 0xFF)) ;
    *msb_val = ((uint8_t)((number>>8) & 0xFF));
}

void convert_geo(uint32_t geo, uint16_t *geo_left, uint16_t *geo_right)
{
    *geo_left = geo / 10000;
    *geo_right = geo % 10000;
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
    static char bus_reset_count=0;
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
    u2.putChar(0x13, portMAX_DELAY);   //Object ID
    u2.putChar(led_num, portMAX_DELAY);
    u2.putChar(0x00, portMAX_DELAY);
    u2.putChar(state, portMAX_DELAY); //LSB
    char checkSum = 0x01^0x13^led_num^0x00^state;
    u2.putChar(checkSum, portMAX_DELAY);
    ack();
}

void display_lcd_geo(uint8_t placeholder, uint32_t geo)
{
    uint16_t geo_left, geo_right;
    convert_geo(geo, &geo_left, &geo_right);
    display_lcd_numbers(placeholder, geo_left);
    display_lcd_numbers(placeholder + 4, geo_right);

}
void display_lcd_numbers(char display_num, uint16_t value)
{
    uint8_t lsb, msb;
    convert16_to_hex(value, &lsb, &msb);
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x01, portMAX_DELAY);   //Write Object
    u2.putChar(0x0F, portMAX_DELAY);   //Object ID
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
    u2.putChar(0x0B, portMAX_DELAY);   //Object ID
    u2.putChar(display_num, portMAX_DELAY);
    u2.putChar(msb, portMAX_DELAY);
    u2.putChar(lsb, portMAX_DELAY);    //LSB
    char checkSum = 0x01^0x0B^display_num^msb^lsb;
    u2.putChar(checkSum, portMAX_DELAY);
    ack();
}

char check_form()
{
    char rxChar = 'a';
    char form_value = 'z';

    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x00);
    u2.putChar(0x0A);
    u2.putChar(0x00);
    u2.putChar(0x0A);

    u2.getChar(&rxChar, portMAX_DELAY);
    u2.getChar(&rxChar, portMAX_DELAY);
    u2.getChar(&rxChar, portMAX_DELAY);
    u2.getChar(&rxChar, portMAX_DELAY);
    u2.getChar(&form_value, portMAX_DELAY);
    u2.getChar(&rxChar, portMAX_DELAY);
    return form_value;
}


bool ack()
{
    bool status = false;
    Uart2& u2 = Uart2::getInstance();
    char ack = 'a';
    if (u2.getChar(&ack, portMAX_DELAY)){
     status = true;
    }
    if(ack!=6)
    {
        u2.getChar(&ack, 0);
        u2.getChar(&ack, 0);
        u2.getChar(&ack, 0);
        u2.getChar(&ack, 0);
        u2.getChar(&ack, 0);
        u2.getChar(&ack, 0);

    }
    return status;
}

bool getButtonState()
{
    static bool status;
    display_lcd_startStop();
    Uart2& u2 = Uart2::getInstance();
    char rxChar, rxChar2, rxChar3, rxChar4 = 'a';
    char button_status = 'z';
    u2.getChar(&rxChar2, portMAX_DELAY);
    u2.getChar(&rxChar3, portMAX_DELAY);
    u2.getChar(&rxChar4, portMAX_DELAY);
    u2.getChar(&rxChar, portMAX_DELAY);
    u2.getChar(&button_status, portMAX_DELAY);
    u2.getChar(&button_status, portMAX_DELAY);
    if (rxChar3==7 || (rxChar2==7 && rxChar3==30))
    {
        status = !status;
    }
    return status;
}

void display_lcd_startStop()
{
    Uart2& u2 = Uart2::getInstance();
    u2.putChar(0x00);   //Write Object
    u2.putChar(0x1E);   //Object ID
    u2.putChar(0x00);
    u2.putChar(0x1E);
}


