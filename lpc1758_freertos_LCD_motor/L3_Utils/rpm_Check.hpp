#include <io.hpp>

typedef struct{
const uint16_t ref_count_slow = 0; //2.63kmph
const uint16_t ref_count_low = 0;  //5kmph
const uint16_t ref_count_medium = 1; //
const uint16_t ref_count_high = 0; //left for future
}ReferenceCounts;

typedef struct{
    uint8_t second_count = 0;
    uint16_t cut_count = 0;
    uint16_t old_count = 0;
    uint16_t old_countms = 0;
    uint16_t rps = 0.0;
    uint16_t rpms = 0.0;
    uint16_t freq =10;
    uint16_t ReferenceRps=0;
    uint16_t ReferencSpeed=0;
    float speed =0.0;
}RPM_s;

