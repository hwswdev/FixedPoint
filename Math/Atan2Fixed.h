
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

// atan2Fixed
// arg: 	 Q1.31, Q1.31
// result:	 Q32
uint32_t atan2Fixed( int32_t arg1, int32_t arg2 );


#ifdef __cplusplus
}
#endif

