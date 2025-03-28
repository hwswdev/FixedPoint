#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint32_t sinFixed_0ToPi4( uint32_t arg );
int32_t  sinFixed( uint32_t arg );
int32_t  sinSignedFixed( int32_t arg );

uint32_t cosFixed_0ToPi4( uint32_t arg );
int32_t  cosFixed( uint32_t arg );
int32_t  cosSignedFixed( int32_t arg );

bool sinCosFixedTest();

#ifdef __cplusplus
}
#endif

