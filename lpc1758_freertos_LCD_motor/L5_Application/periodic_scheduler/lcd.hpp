/*
 * lcd.hpp
 *
 *  Created on: Oct 25, 2017
 *      Author: venka
 */

#ifndef LCD_HPP_
#define LCD_HPP_


#include "_can_dbc/generated_can.h"
#include "uart2.hpp"
#include <cstdlib>
#include <ctime>
#include "lcd.hpp"
#include <stdio.h>
#include <utilities.h>
#include "lcd_utilities.cpp"

//enum lcd_led{Sensor, Motor, Comm, Geo};
enum lcd_digits{Miles_covered, Miles_remaining, Dest_lat, Dest_long, Current_lat, Current_long};
enum lcd_health{System, Battery, Degree0, Degree20, Degree40, Degree60, Degree80, Degree_neg20, Degree_neg40, Degree_neg60, Degree_neg80};
enum lcd_status{Off, On};
enum lcd_ultrasound{Ultrasound_left, Ultrasound_front, Ultrasound_right};
enum lcd_form{Main_page, Sensor_page, Distance_page, GPS_page};


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
void update_LCD_main_page(float value, uint32_t status);
void update_LCD_distance_page(float covered, int remain);
//void update_LCD_GPS_page(GPS_DATA_t gps, ANDROID_LOCATION_t androidDist);
//void update_LCD_sensor_page(SENSOR_DATA_t sen);
#endif /* LCD_HPP_ */
