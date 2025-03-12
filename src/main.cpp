/*
 * main.cpp
 *
 *  Created on: Feb 22, 2025
 *      Author: Evgeny
 */

#include <stdint.h>
#include <stddef.h>


template<typename Type>
constexpr Type Pi() {
	return { 3.14159265358979323846 };
	//return { 3.14159265358979 };
}

extern "C" int32_t sinFixed( const uint32_t x );
extern "C" int32_t cosFixed( const uint32_t x );


static constexpr const size_t SinResultCount = 360*60*60 + 1;


int32_t test() {

	int32_t  sumMaxErr   = 0;
	int32_t  sumMaxAbsErr = 0;
	int32_t  maxErrAngle = 0;

	for( uint32_t index = 0; index < SinResultCount; index++ ) {
		const uint32_t stepSize =  ((static_cast<uint64_t>(1) << 32) / SinResultCount);

		const uint32_t arg = stepSize * index;

		const int32_t signedIndex = static_cast<int32_t>(arg);
		const double angleGrad = ( static_cast<double>(signedIndex) * 180 ) / (static_cast<uint32_t>(1) << 31);

		const int32_t resSin = sinFixed( arg );
		const int32_t resCos = cosFixed( arg );

		const uint64_t resSinQuad = static_cast<int64_t>(resSin) * resSin;
		const uint64_t resCosQuad = static_cast<int64_t>(resCos) * resCos;
	    const uint64_t sinCosSum = ( resSinQuad + resCosQuad ) >> 32;
	    const int32_t  sinCosSumErr = static_cast<int32_t>(0x40000000) - sinCosSum;

		const double resSinF = static_cast<double>(resSin) / ( static_cast<uint64_t>(1) << 31 );
		const double resCosF = static_cast<double>(resCos) / ( static_cast<uint64_t>(1) << 31 );

		const double maxAbsErr = ( sinCosSumErr < 0 ) ? -sinCosSumErr : sinCosSumErr;

		if ( maxAbsErr > sumMaxAbsErr ) {
			sumMaxAbsErr = maxAbsErr;
			sumMaxErr    = sinCosSumErr;
			maxErrAngle  = angleGrad;
		}

	}
	asm("bkpt");
	return 0;
}







extern "C" int main(void){
	test();
	return 0;
}
