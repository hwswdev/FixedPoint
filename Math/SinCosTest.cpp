/*
 * SinCosTest.cpp
 *
 *  Created on: Mar 27, 2025
 *      Author: Evgeny
 */


#include "SinCosFixed.h"
#include <math.h>


extern "C" bool sinCosDispPi4Test() {
	constexpr const uint64_t One = static_cast<uint64_t>(1) << 32;
	constexpr const double   PIf = 3.14159265358979;
	constexpr const double   PI4f = PIf / 4;
	constexpr const uint32_t PI4 = PI4f * One;
	constexpr const uint32_t dAngle = 0x1000;

	double minRadiusVal    = 1000000000.0;
	double maxRadiusVal    = 0.0;
	double sumRadiusVal    = 0.0;
	uint32_t sumCycleCount = 0.0;


	// Calculate center value (math waiting),
	// Calculate maximum and minimum values
	for ( uint32_t angle = 0; angle <= PI4; angle += dAngle ) {

		const uint64_t sinVal = sinFixed_0ToPi4( angle );
		const uint64_t cosVal = cosFixed_0ToPi4( angle );

		const double sinX = static_cast<double>(sinVal) / One;
		const double cosX = static_cast<double>(cosVal) / One;

		const double quadRadius = (sinX * sinX) + (cosX * cosX);
		const double radius = sqrt(quadRadius);

		sumRadiusVal += radius;
		maxRadiusVal = ( radius > maxRadiusVal ) ? radius : maxRadiusVal;
		minRadiusVal = ( radius < minRadiusVal ) ? radius : minRadiusVal;

		sumCycleCount++;
	}

	const double mathWaitRadius = sumRadiusVal / sumCycleCount;

	// Calculate disperse
	double sumRadiusQuadDiff = 0.0;
	for ( uint32_t angle = 0; angle <= PI4; angle += dAngle ) {

		const uint64_t sinVal = sinFixed_0ToPi4( angle );
		const uint64_t cosVal = cosFixed_0ToPi4( angle );

		const double sinX = static_cast<double>(sinVal) / One;
		const double cosX = static_cast<double>(cosVal) / One;

		const double quadRadius = (sinX * sinX) + (cosX * cosX);
		const double radius = sqrt(quadRadius);
		const double diff = (radius - mathWaitRadius);
		const double diffQ = diff * diff;
		sumRadiusQuadDiff += diffQ;
	}

	const double midQuadDiffSum = sumRadiusQuadDiff / sumCycleCount;


	const double queryCenterValue = 1.0;

	const double centerVal    =  mathWaitRadius;
	const double dispRadius   =  sqrt( midQuadDiffSum );
	const double maxDiffVal   =  maxRadiusVal - mathWaitRadius;
	const double minDiffVal   =  mathWaitRadius - minRadiusVal;
	const double radiusErr    =  centerVal - queryCenterValue;
	const double radiusAbsErr = abs( radiusErr );

	if ( maxDiffVal   > 7.2e-10 )  return false;
	if ( minDiffVal   > 7.2e-10 )  return false;
	if ( dispRadius   > 2.33e-10 )  return false;
	if ( radiusAbsErr > 2.33e-10 )  return false;

	return true;
}

extern "C" bool sinCosDispTest() {
	constexpr const uint64_t One = static_cast<uint64_t>(1) << 31;
	constexpr const uint32_t dAngle   = 0x1000;
	constexpr const uint32_t MaxAngle = 0xFFFFF000;

	double minRadiusVal    = 1000000000.0;
	double maxRadiusVal    = 0.0;
	double sumRadiusVal    = 0.0;
	uint32_t sumCycleCount = 0.0;


	// Calculate center value (math waiting),
	// Calculate maximum and minimum values
	for ( uint32_t angle = 0; angle < MaxAngle; angle += dAngle ) {

		const int32_t sinVal = sinFixed( angle );
		const int32_t cosVal = cosFixed( angle );

		const double sinX = static_cast<double>(sinVal) / One;
		const double cosX = static_cast<double>(cosVal) / One;

		const double quadRadius = (sinX * sinX) + (cosX * cosX);
		const double radius = sqrt(quadRadius);

		sumRadiusVal += radius;
		maxRadiusVal = ( radius > maxRadiusVal ) ? radius : maxRadiusVal;
		minRadiusVal = ( radius < minRadiusVal ) ? radius : minRadiusVal;

		sumCycleCount++;
	}

	const double mathWaitRadius = sumRadiusVal / sumCycleCount;

	// Calculate disperse
	double sumRadiusQuadDiff = 0.0;
	for ( uint32_t angle = 0; angle < MaxAngle; angle += dAngle ) {

		const int32_t sinVal = sinFixed( angle );
		const int32_t cosVal = cosFixed( angle );

		const double sinX = static_cast<double>(sinVal) / One;
		const double cosX = static_cast<double>(cosVal) / One;

		const double quadRadius = (sinX * sinX) + (cosX * cosX);
		const double radius = sqrt(quadRadius);
		const double diff = (radius - mathWaitRadius);
		const double diffQ = diff * diff;
		sumRadiusQuadDiff += diffQ;
	}

	const double midQuadDiffSum = sumRadiusQuadDiff / sumCycleCount;


	const double queryCenterValue = 1.0;

	const double centerVal    =  mathWaitRadius;
	const double dispRadius   =  sqrt( midQuadDiffSum );
	const double maxDiffVal   =  maxRadiusVal - mathWaitRadius;
	const double minDiffVal   =  mathWaitRadius - minRadiusVal;
	const double centerErr    =  centerVal - queryCenterValue;
	const double centerAbsErr = abs( centerErr );

	if ( maxDiffVal   > 9.0e-10 ) return false;
	if ( minDiffVal   > 9.0e-10 ) return false;
	if ( dispRadius   > 2.33e-10 ) return false;
	if ( centerAbsErr > 4.66e-10 ) return false;

	return true;
}


bool sinCosTest(){
	constexpr const double   PIf = 3.14159265358979;
	constexpr const uint64_t One = static_cast<uint64_t>(1) << 32;

	double sinMaxDiff = 0;
	double sinMinDiff = 0;
	double cosMaxDiff = 0;
	double cosMinDiff = 0;
	double sinDispSum = 0;
	double cosDispSum = 0;
	uint32_t count = 0;

	for( uint32_t angle = 0; angle < 0xFFFF0000; angle += 0x10000 ) {
		const double angleF = static_cast<double>(angle) * ( PIf * 2 ) / One;

		const int32_t sinVal = sinFixed( angle );
		const int32_t cosVal = cosFixed( angle );

		const double sinF = static_cast<double>(sinVal) / (One >> 1);
		const double cosF = static_cast<double>(cosVal) / (One >> 1);

		const double origSinF = sin(angleF);
		const double origCosF = cos(angleF);

		const double sinDiff = sinF - origSinF;
		const double cosDiff = cosF - origCosF;

		sinMaxDiff = ( sinMaxDiff < sinDiff ) ? sinDiff : sinMaxDiff;
		sinMinDiff = ( sinMinDiff > sinDiff ) ? sinDiff : sinMinDiff;

		cosMaxDiff = ( cosMaxDiff < cosDiff ) ? cosDiff : cosMaxDiff;
		cosMinDiff = ( cosMinDiff > cosDiff ) ? cosDiff : cosMinDiff;

		const double sinQDiff = sinDiff * sinDiff;
		const double cosQDiff = cosDiff * cosDiff;

		sinDispSum += sinQDiff;
		cosDispSum += cosQDiff;

		count++;

	}

	const double sinDispQ = sinDispSum / count;
	const double cosDispQ = cosDispSum / count;

	const double sinDisp = sqrt( sinDispQ );
	const double cosDisp = sqrt( cosDispQ );

	if ( sinMaxDiff   > 9.0e-10 ) return false;
	if ( cosMaxDiff   > 9.0e-10 ) return false;
	if ( -cosMinDiff  > 9.0e-10 ) return false;
	if ( -sinMinDiff  > 9.0e-10 ) return false;
	if ( sinDisp  > 2.8e-10 ) return false;
	if ( cosDisp  > 2.8e-10 ) return false;

	return true;
}


extern "C" bool sinCosFixedTest() {
	const bool commonTestSuccess = sinCosTest();
	if ( !commonTestSuccess ) return false;

	const bool pi4DispTestSuccess = sinCosDispPi4Test();
	if ( !pi4DispTestSuccess ) return false;

	const bool dispTestSuccess = sinCosDispTest();
	if ( !dispTestSuccess ) return false;

	return true;
}

