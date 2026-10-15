
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>


typedef uint32_t SqrtArgQ32;
typedef uint32_t SqrtResQ16d16;

// sqrtFixed
// arg: 	 Q32
// result:	 Q16.16
SqrtResQ16d16 sqrtFixed( SqrtArgQ32 arg );


#ifdef __cplusplus
}
#endif

