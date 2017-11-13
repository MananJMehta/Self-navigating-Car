/*
 * lcd.hpp
 *
 *  Created on: Oct 25, 2017
 *      Author: venka
 */

#ifndef LCD_HPP_
#define LCD_HPP_

#include "uart2.hpp"
#include <cstdlib>
#include <ctime>
#include "lcd.hpp"
#include <stdio.h>
#include <utilities.h>
#include "lcd_utilities.cpp"

int get_random_int(int max_val);
void display_speedometer(char random_speed);
void display_bus_reset();
//void display_lidar_spectrum(char pos, char val);
void display_lcd_led(char led_num, char state);
void display_LCD_large_led(char led_num, char state);
//void display_lcd_numbers(char display_num, uint16_t msb, uint16_t lsb);
void display_lcd_numbers(char display_num, uint16_t value);
void display_lcd_bar(char display_num, uint16_t value);
void display_lidar_led(char display_num, uint16_t value);
char check_form();
bool ack();
bool getButtonState();
void display_lcd_startStop();
void display_lcd_geo(uint8_t placeholder, uint32_t geo);
void convert16_to_hex(uint16_t number, uint8_t *lsb_val, uint8_t *msb_val);

#endif /* LCD_HPP_ */
