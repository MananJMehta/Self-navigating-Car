/*
 * speed_util.cpp
 *
 *  Created on: Nov 14, 2017
 *      Author: venka
 */

#include<stdio.h>
float check_speed_diff(uint16_t rpm, uint16_t ref, float curr_speed, float *val)
{
    if ((rpm - ref) > 1) {
        *val = curr_speed - 0.1;
    }
    else if ((ref - rpm) > 1) {
        *val = curr_speed + 0.1;
    }
    else
    {
        *val = curr_speed;
    }
    if (*val > 15.5) {
        *val = 15.5;
    }

    return *val;
}


void rpm_mtr_val(uint8_t *second_count, uint16_t *rps, uint16_t *cut_count, uint16_t *old_count, float *speed, uint16_t *old_countms)
{
    (*second_count)++;
    *rps = *cut_count - *old_count;
    *speed = (36.5 * *rps * 3600) / (4*1000*100);
    *old_count = *cut_count;

    if(*second_count == 60)
    {
        *second_count = 0;
        *cut_count = 0;
        *old_count = 0;
        *old_countms=0;
    }
}
