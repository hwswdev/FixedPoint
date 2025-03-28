/*
 * SinCosTest.cpp
 *
 *  Created on: Mar 27, 2025
 *      Author: Evgeny
 */


#include "SinCosFixed.h"

extern "C" bool sinCosFixedTest() {

	const uint32_t MaxSinCos0Pi4DiffAtPI4 = 1;
	const uint32_t MaxSinCos0Pi4Value = 0xFFFFFFFF;
	const uint32_t MinSinCos0Pi4Value = 0x00000000;

	const uint64_t One = static_cast<uint64_t>(1) << 32;
	const double PIf = 3.14159265358979;
	const double PI4f = PIf / 4;
	const uint32_t PI4 = PI4f * One;

	uint32_t maxAbsQError0Pi4 = 0;
	uint32_t maxAbsQErrorAngle0Pi4 = 0;

	const uint32_t sin0 = sinFixed_0ToPi4( 0 );
	const uint32_t cos0 = cosFixed_0ToPi4( 0 );
	const uint32_t sinPI4 = sinFixed_0ToPi4( PI4 );
	const uint32_t cosPI4 = cosFixed_0ToPi4( PI4 );

	const uint32_t sinCosPi4Err = ( sinPI4 < cosPI4 ) ? ( cosPI4 - sinPI4 ) : ( sinPI4 - cosPI4 );

	// Check sin value at 0
	if ( MinSinCos0Pi4Value != sin0 ) return false;
	// Check cos value at 0
	if ( MaxSinCos0Pi4Value != cos0 ) return false;
	// So, this is mean that error, at X = PI/4 less then 2LSB
	// 1 / (2^32) equals to 2.3 * (10 ^ -10). 2 LSB is about 4.6 * (10 ^ -10)
	if ( sinCosPi4Err > MaxSinCos0Pi4DiffAtPI4 )   return false;

	for ( uint32_t angle = 0; angle < PI4; angle++ ) {

		const uint32_t sinVal = sinFixed_0ToPi4( angle );
		const uint32_t cosVal = cosFixed_0ToPi4( angle );

		const uint64_t sinQ = ( static_cast<uint64_t>(sinVal) * sinVal ) >> 2;
		const uint64_t cosQ = ( static_cast<uint64_t>(cosVal) * cosVal ) >> 2;
		const uint64_t sum = (sinQ + cosQ) >> 30;
		const uint32_t absSquareError = (One > sum) ? ( One - sum ) : ( sum - One );

		if ( absSquareError > maxAbsQError0Pi4 ) {
			maxAbsQError0Pi4 = absSquareError;
			maxAbsQErrorAngle0Pi4 = angle;
		}

	}

	uint32_t maxAbsQError = 0;
	uint32_t maxAbsQErrorAngle = 0;

	for ( uint32_t angle = 0; angle < 0xFFFFF000; angle++ ) {
		const int32_t sinVal = sinFixed( angle );
		const int32_t cosVal = cosFixed( angle );

		const uint32_t sinAbsVal = ( sinVal < 0 ) ? -sinVal : sinVal;
		const uint32_t cosAbsVal = ( cosVal < 0 ) ? -cosVal : cosVal;

		const uint64_t sinQ = ( static_cast<uint64_t>(sinAbsVal) * sinAbsVal );
		const uint64_t cosQ = ( static_cast<uint64_t>(cosAbsVal) * cosAbsVal );
		const uint64_t sum = (sinQ + cosQ) >> 30;
		const uint32_t absSquareError = (One > sum) ? ( One - sum ) : ( sum - One );

		if ( absSquareError > maxAbsQError ) {
			maxAbsQError = absSquareError;
			maxAbsQErrorAngle = angle;
		}

	}

	// Check if error is lower then sqrt(7) i.e. 2.7 * LSB,
	// but I think it is about +/-1 LSB, because of max cos value is 0xFFFFFFFF
	if ( maxAbsQError0Pi4 > 7) return false;

	// Check if error is lower then sqrt(9), i.e 3 * LSB, but I think it is less about
	if ( maxAbsQError > 8 ) return false;

	// So. what is it mean ERROR equals to 3LSB ?
	// It means that 3* 1 / (2^32), i.e 7 * (10 ^ -10)

	// It means if you use it in CNC, and want to make circle which radius is 1 meter
	// You will get error about 0.0000007mm
	// OR if you will start to circle with Earth radius, you will get error about 28mm.
	// OR if you get sputnik space distance with radius 100.000KM, you will get error about 70mm, i.e. 0.07m

	// Success
	return true;
}

