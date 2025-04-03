
/*****************************************************
 *
 *  SinCosTest.cpp
 *
 *  Sin(x), Cos(x)
 *  Tests ....
 *  So. I think, I have to do it using FixedPoint
 *  But that's temp tests to check
 *
 *****************************************************/


#include "SinCosFixed.h"
#include <math.h>


constexpr uint32_t getScaledPiValue () {
	constexpr const uint32_t PI = (1 << 31);
	return PI;
}

bool sinCosTest(){

	constexpr const uint32_t PI = getScaledPiValue();
	constexpr const uint32_t AngleCount = 360 * 60 * 60; // 0°0'1''
	constexpr const uint32_t dAngle = PI / AngleCount;

	uint32_t num = 0;


	for ( uint32_t angle = 0; angle < PI; angle += dAngle ) {

		const int32_t angleN = -static_cast<int32_t>(angle);

		const int32_t sinValP = sinFixed( angle );
		const int32_t cosValP = cosFixed( angle );

		const int32_t sinValN = sinFixed( angleN );
		const int32_t cosValN = cosFixed( angleN );

		if ( cosValP != cosValN ) {
			// Not symmetric
			asm("bkpt");
			return false;
		}
		if ( sinValP != -sinValN ) {
			// Not symmetric
			asm("bkpt");
			return false;
		}

		const uint32_t sinAbsP = ((sinValP < 0) ? -sinValP : sinValP) << 1;
		const uint32_t cosAbsP = ((cosValP < 0) ? -cosValP : cosValP) << 1;

		const uint32_t sinAbsN = ((sinValN < 0) ? -sinValN : sinValN) << 1;
		const uint32_t cosAbsN = ((cosValN < 0) ? -cosValN : cosValN) << 1;


		const uint64_t sin1Q = ( static_cast<uint64_t>(sinAbsP) * sinAbsP ) >> 31;
		const uint64_t cos1Q = ( static_cast<uint64_t>(cosAbsP) * cosAbsP ) >> 31;

		const uint64_t sin2Q = ( static_cast<uint64_t>(sinAbsN) * sinAbsN ) >> 31;
		const uint64_t cos2Q = ( static_cast<uint64_t>(cosAbsN) * cosAbsN ) >> 31;

		const uint64_t r1Q = ( static_cast<uint64_t>(sin1Q) + static_cast<uint64_t>(cos1Q) ) >> 1;
		const uint64_t r2Q = ( static_cast<uint64_t>(sin2Q) + static_cast<uint64_t>(cos2Q) ) >> 1;

		const uint64_t maxRValue = ( static_cast<uint64_t>(1) << 32 ) + 1;
		const uint64_t minRValue = ( static_cast<uint64_t>(1) << 32 ) - 6;

		if ( ( minRValue > r1Q ) || ( maxRValue < r1Q  ) ||
			 ( minRValue > r2Q ) || ( maxRValue < r2Q  ) ) {
			// Here is radius error +0*LSB / -5*LSB, sqrt(5) => 2.24
			// i.e radius error is about 2.24*LSB
			// But, multiply error is about -2 * LSB,
			// because of sin / cos returns 31bit + sign, i.e. 31bit*31bit => (30bit)???.
			asm("bkpt");
			return false;
		}

		num++;

	}

	return true;
}



double calcRadiusError( uint32_t radius ) {

	const uint64_t Radius64BitQ = static_cast<uint64_t>(radius) * radius;
	uint64_t sumQDiff = 0;
	uint64_t maxRq = 0UL;
	uint64_t minRq = 0xFFFFFFFFFFFFFFFFUL;
	uint32_t count = 0;

	for(uint32_t angle = 0; angle < 0xFFFFFE00; angle += 0x100 ) {

		const int32_t sinR = rSinFixed( angle, radius );
		const int32_t cosR = rCosFixed( angle, radius );

		// So. Single bit is already lost because of value sign,
		// But I have to get correct result of multiply
		const uint32_t mSinR = ( (sinR < 0) ? -sinR : sinR );
		const uint32_t mCosR = ( (cosR < 0) ? -cosR : cosR );

		// So I already loose 2 bits here
		// Shift by one to allow sum of multiply without carry
		const uint64_t sinQ = ( static_cast<uint64_t>(mSinR) * static_cast<uint64_t>(mSinR) );
		const uint64_t cosQ = ( static_cast<uint64_t>(mCosR) * static_cast<uint64_t>(mCosR) );

		// We are already lost 2 bits, because of sin/cos gave 31bit pos value max
		// Multiply 31 * 31 gave us 30 bit, mapped to uint64_t.
		// Shift it to get 31 bit again mapped to uint64_t
		// 31bit value + 31bit value, can't be higher then 32bit, mapped to 64bit value
		const uint64_t rQ    = ( sinQ + cosQ );

		if ( rQ > maxRq ) { maxRq = rQ; }
		if ( rQ < minRq ) { minRq = rQ; }

		// diffQ = ( RealR * RealR  - CalculatedR * CalculatedR )
		const int64_t diffQ = ( rQ - Radius64BitQ );

		const uint64_t absDiffQ = (diffQ < 0) ? -diffQ : diffQ;

		sumQDiff += absDiffQ;
		count++;

	}

	const uint32_t rQuadDiff = sumQDiff / count;
	const double qErr = static_cast<double>(rQuadDiff) / ( static_cast<uint64_t>(1) << (32 - 1) ); // Is it correct ??? I mean 32bit value, but sign.
	const double radQ = static_cast<double>(Radius64BitQ);
	const double maxQ = static_cast<double>(maxRq);
	const double minQ = static_cast<double>(minRq);

	const double rErr     = sqrt(qErr);
	const double radMax   = sqrt(maxQ);
	const double radMin   = sqrt(minQ);

	const double dMax = radMax - radius;
	const double dMin = radius - radMin;

	return ( (dMax > dMin) ? dMax : dMin );

}

bool sinCosTestWithGeneric(){
	constexpr const double PIf = 3.14159265358979;
	constexpr const uint32_t PI = getScaledPiValue();
	constexpr const uint32_t AngleCount = 360 * 60 * 60; // 0°0'1''
	constexpr const uint32_t dAngle = PI / AngleCount;
	constexpr const uint32_t radius = 0x7FFFFFFF;

	uint32_t num = 0;


	for ( uint32_t angle = 0; angle < PI; angle += dAngle ) {

		const int32_t angleN = -static_cast<int32_t>(angle);

		const int32_t sinValP = rSinFixed( angle, radius );
		const int32_t cosValP = rCosFixed( angle, radius );

		const int32_t sinValN = rSinFixed( angleN, radius );
		const int32_t cosValN = rCosFixed( angleN, radius );

		const double angleF = PIf * angle / PI;
		const double nAngleF = -angleF;

		const int32_t sinPOrig = radius * sin( angleF );
		const int32_t cosPOrig = radius * cos( angleF );
		const int32_t sinNOrig = radius * sin( nAngleF );
		const int32_t cosNOrig = radius * cos( nAngleF );

		const int32_t dSinP = sinValP - sinPOrig;
		const int32_t dCosP = cosValP - cosPOrig;
		const int32_t dSinN = sinValN - sinNOrig;
		const int32_t dCosN = cosValN - cosNOrig;

		const int32_t dAbsSinP = abs(dSinP);
		const int32_t dAbsCosP = abs(dCosP);
		const int32_t dAbsSinN = abs(dSinN);
		const int32_t dAbsCosN = abs(dCosN);

		if ( ( dAbsSinP > 1) || ( dAbsCosP > 1) ||
			 ( dAbsSinN > 1) || ( dAbsCosN > 1) ) {
			asm("bkpt");

			const int32_t sinValPx = rSinFixed( angle, radius );
			const int32_t cosValPx = rCosFixed( angle, radius );
			const int32_t sinValNx = rSinFixed( angleN, radius );
			const int32_t cosValNx = rCosFixed( angleN, radius );

			asm("bkpt");
			return false;
		}

	}

	return true;
}



extern "C" bool sinCosFixedTest() {

	if (! sinCosTestWithGeneric() ) return false;
	double radiusError = calcRadiusError( 1000000000 );
	if ( radiusError > 1.0 ) return false;
	if ( !sinCosTest() ) return false;

	return true;
}

