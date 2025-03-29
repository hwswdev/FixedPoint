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
	constexpr const uint32_t dAngle = 0x10000;

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

	if ( maxDiffVal   > 7.2e-10 )   return false;
	if ( minDiffVal   > 7.2e-10 )   return false;
	if ( dispRadius   > 2.5e-10 )  return false;
	if ( radiusAbsErr > 2.5e-10 )  return false;

	return true;
}

extern "C" bool sinCosDispTest() {
	constexpr const uint64_t One = static_cast<uint64_t>(1) << 31;
	constexpr const uint32_t dAngle   = 0x10000;
	constexpr const uint32_t MaxAngle = 0xFFFF0000;

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

	if ( maxDiffVal   > 9.0e-10 )  return false;
	if ( minDiffVal   > 9.0e-10 )  return false;
	if ( dispRadius   > 2.5e-10 ) return false;
	if ( centerAbsErr > 5.0e-10 ) return false;

	return true;
}


extern "C" bool sinCosFixedTest() {

	const bool pi4DispTestSuccess = sinCosDispPi4Test();
	if ( !pi4DispTestSuccess ) return false;

	const bool dispTestSuccess = sinCosDispTest();
	if ( !dispTestSuccess ) return false;

	return true;
}

