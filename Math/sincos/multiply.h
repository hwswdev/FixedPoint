/*
  ********************************************************************************************
  * @file      multiply.h dedicated to ARM Cortex M devices
  * @author    Evgeny Sobolev
  * @version   V1.0.0
  * @date      2026-10-17
*/


inline uint32_t smull_32( int32_t a, int32_t b ) {
	int32_t result;
	int32_t temp;

	__asm__ __volatile__ (
	    "smull %0, %1, %2, %3 \n\t"
	    : "=&r" (temp), "=r" (result)
	    : "r" (a), "r" (b) );

	return result;
}

inline uint32_t umull_32( uint32_t a, uint32_t b ) {
	uint32_t result;
	uint32_t temp;

	__asm__ __volatile__ (
	    "umull %0, %1, %2, %3 \n\t"
	    : "=r" (temp), "=r" (result)
	    : "r" (a), "r" (b) );

	return result;
}

inline uint64_t umull_64( uint32_t a, uint32_t b ) {
	uint32_t result_hi;
	uint32_t result_low;

	__asm__ __volatile__ (
	    "umull %0, %1, %2, %3 \n\t"
	    : "=r" (result_low), "=r" (result_hi)
	    : "r" (a), "r" (b) );

	return ( ( static_cast<uint64_t>(result_hi) << 32 ) | result_low );
}

inline uint32_t umlal_32( uint32_t accumulator, uint32_t a, uint32_t b ) {
	uint32_t temp_reg = 0;

    __asm__ __volatile__ (
        "umlal %0, %1, %2, %3 \n\t"
        : "+r" ( temp_reg ), "+r" (accumulator)
        : "r" (a), "r" (b)
    );

    return accumulator;
}

inline uint64_t umlal_64(uint64_t accumulator, uint32_t a, uint32_t b ) {
	uint32_t acc_hi = static_cast<uint32_t>( accumulator >> 32 );
	uint32_t acc_low = static_cast<uint32_t>( accumulator );

	__asm__ __volatile__ (
	    "umlal %0, %1, %2, %3 \n\t"
	    : "+r" (acc_low), "+r" (acc_hi)
	    : "r" (a), "r" (b) );

	return ( ( static_cast<uint64_t>(acc_hi) << 32 ) | acc_low );
}


inline uint64_t umslal_64( uint64_t accumulator, uint32_t a, uint32_t b ) {
	uint32_t temp_low;
	uint32_t temp_high;
	uint32_t acc_hi = static_cast<uint32_t>( accumulator >> 32 );
	uint32_t acc_low = static_cast<uint32_t>( accumulator );

    __asm__ __volatile__ (
        "umull %0, %1, %4, %5 \n\t"
    	"subs %2, %2, %0 \n\t"
        "sbc  %3, %3, %1 \n\t"

        : "=&r" ( temp_low ), "=&r" (temp_high), "+r" (acc_low), "+r"(acc_hi)
        : "r" (a), "r" (b)
    );

    return ( ( static_cast<uint64_t>(acc_hi) << 32 ) | acc_low );
}


inline uint32_t umslal_32(uint32_t accumulator, uint32_t a, uint32_t b ) {
	uint32_t temp_low;
	uint32_t temp_high;

    __asm__ __volatile__ (
        "umull %0, %1, %3, %4 \n\t"
    	"sub %2, %2, %1"
        : "=&r" ( temp_low ), "=&r" (temp_high), "+r" (accumulator)
        : "r" (a), "r" (b)
    );

	return accumulator;
}


