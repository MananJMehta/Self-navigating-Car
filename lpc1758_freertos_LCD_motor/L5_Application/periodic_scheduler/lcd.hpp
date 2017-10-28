/*
 * lcd.hpp
 *
 *  Created on: Oct 25, 2017
 *      Author: venka
 */

#ifndef LCD_HPP_
#define LCD_HPP_

int get_random_int(int max_val);
void display_speedometer(char random_speed);
void display_bus_reset();
void display_lidar_spectrum(char pos, char val);
void display_lcd_led(char led_num, char state);
void display_LCD_health(char led_num, char state);
void display_lcd_numbers(char display_num, char msb, char lsb);

#endif /* LCD_HPP_ */
