#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// sinFixed(angle), returns sin(x),
// where x is value 0..0xFFFFFFFF, mapped to 0..2*PI-1LSB
// result is mapped to 0x80000000..0x7FFFFFFFF is mapped to -1 .. +1 - 1LSB
int32_t  sinFixed( uint32_t arg );

// sinFixed(angle), returns sin(x),
// where x is value 0..0xFFFFFFFF, mapped to 0..2*PI-1LSB
// result is mapped to 0x80000000..0x7FFFFFFFF is mapped to -1 .. +1 - 1LSB
int32_t  cosFixed( uint32_t arg );

// rSinFixed(angle, radius), returns radius * sin(x),
// where x is value 0..0xFFFFFFFF, mapped to 0..2*PI-1LSB
int32_t rSinFixed( uint32_t angle, uint32_t radius );

// rCosFixed(angle, radius), returns radius * cos(x),
// where x is value 0..0xFFFFFFFF, mapped to 0..2*PI-1LSB
int32_t rCosFixed( uint32_t angle, uint32_t radius );


#ifdef __cplusplus
}
#endif

