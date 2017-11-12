#include <cstdlib>
#include <ctime>
#include <stdio.h>

extern "C" {
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
}
