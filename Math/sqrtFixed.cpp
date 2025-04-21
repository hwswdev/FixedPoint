

/*
  ********************************************************************************************
  * @file      sqrtFixed.cpp
  * @author    Evgeny Sobolev
  * @version   V1.0.0
  * @date      2025-04-20
  *
  * @description  Function unt32_t sqrtFixed( uint32_t x )
  *
  * @description  0  	        =>  0x00000000,
  * @description  65536         =>  0x00010000,
  * @description  2^32-1        =>  0xFFFFFFFF
  * @description  valid  x => [ 0x00000000 .. 0xFFFFFFFF ]
  *
  * @description  Return 32 bit value, where 16bit is decimal & 16bit is fractional
  *
  * @description  Where result, is scaled as
  * @description  0.0           =>  0x00000000,
  * @description  0.5           =>  0x00008000,
  * @description  0.25          =>  0x00004000,
  * @description  1.0           =>  0x00010000,
  * @description  32768.25      =>  0x80004000,
  * @description  65535.9999847 =>  0xFFFFFFFF
  *
  * @description  Not iterative sqrt(x) approxymation
  * @description  Accuracy is about +/-0.2
  * @description  God, thank you of your help.
  * 
  *******************************************************************************************/

#include <math.h>
#include <stdio.h>

typedef unsigned char      uint8_t;
typedef signed char        int8_t;
typedef unsigned short     uint16_t;
typedef signed short       int16_t;
typedef unsigned int       uint32_t;
typedef signed int         int32_t;
typedef unsigned long long uint64_t;
typedef signed long long   int64_t;


inline uint8_t getMsb( const uint32_t val ) {
	register uint32_t x = val;
	register uint8_t bitCount = 32;
	do {
		if ( x & 0x80000000 ) break;
		x <<= 1;
		bitCount--;
	} while(bitCount);
	return bitCount;
}

inline uint32_t mulab( uint32_t a, uint32_t b ) {
	register const uint64_t res64 = static_cast<uint64_t>(a) * static_cast<uint32_t>(b);
	register const uint32_t res = (res64 >> 32);
	return res;
}


inline uint32_t getCentredInverseParable( uint32_t x, uint8_t argBitCount ) {
	register const uint8_t bitCntToMax = 32 - argBitCount;
	register const uint32_t xShiftMax = (x << (bitCntToMax + 1));
	register const uint32_t xDiff = ( xShiftMax & 0x80000000 ) ? ( xShiftMax - 0x80000000 ) :  ( 0x7FFFFFFF - xShiftMax );
	register const uint32_t xDiffMax = xDiff << 1;
	register const uint32_t xDifQuad = mulab( xDiffMax, xDiffMax );
	register const uint32_t oneMinusXDifQuad = 0xFFFFFFFF - xDifQuad;
	return oneMinusXDifQuad;
}

inline uint32_t getSqrCorrX( uint32_t x, uint8_t argBitCount ) {
	// Constants
	static const double xValOnMaxErrorF = 1.0 / ( 4.0 * ( sqrt(2.0) - 1.0 ) * ( sqrt(2.0) - 1.0 ) );
	static const double xValOnMidF = 1.5;	
	static const double xMaxErrF = xValOnMidF - xValOnMaxErrorF;
	static const uint32_t xMaxErrMult4 = xMaxErrF * ( static_cast<uint64_t>(1) << (32 + 3) );
	
	// Calculations
	register const uint32_t centerParableVal = getCentredInverseParable( x, argBitCount );
	register const uint32_t xDiffScaledFour = mulab( xMaxErrMult4, centerParableVal );
	register const uint32_t xDiffScaled = xDiffScaledFour >> 4;
	register const uint32_t xDiff = xDiffScaled >> ( 32 - argBitCount );
	return xDiff;
}
 
inline uint32_t getSqrCorrY( uint32_t x, uint8_t argBitCount ) {
	// This is calculations of point where is maximum of approxymation error
	register const uint32_t xSqrCorr = getSqrCorrX(x, argBitCount );
	register const uint32_t xCorrected = x + xSqrCorr;
	register const uint32_t sqrValue = getCentredInverseParable( xCorrected, argBitCount );
	register const uint32_t absMaxSqrErrorScaled = (argBitCount & 0x01) ?  1220935711 : 1726663841;
	register const uint32_t sqrCorrValScaled = mulab( sqrValue, absMaxSqrErrorScaled );
	register const uint8_t  shift = ( (32 - argBitCount) >> 1 ) + 5;
	register const uint32_t sqrCorrVal = sqrCorrValScaled >> shift;
	return sqrCorrVal;
}

inline uint32_t getSqrtLinearApprox( uint32_t x, uint8_t argBitCount ) {
	// Constants
	static const double pow32 = static_cast<uint64_t>(1) << 32;
	static const double pow31 = static_cast<uint64_t>(1) << 31;
	static const double pow30 = static_cast<uint64_t>(1) << 30;
	static const uint32_t Y32 = sqrt(pow31) * (static_cast<uint32_t>(1) << 16);
	static const uint32_t Y31 = sqrt(pow30) * (static_cast<uint32_t>(1) << 16);
	static const uint32_t dY32 = ( sqrt(pow32) - sqrt(pow31) ) * (static_cast<uint32_t>(1) << 16);
	static const uint32_t dY31 = ( sqrt(pow31) - sqrt(pow30) ) * (static_cast<uint32_t>(1) << 16);
	// Calculations
	register const uint8_t bitCntToMax = 32 - argBitCount;
	register const uint32_t xShiftMax = (x << (bitCntToMax + 1));
	register const uint32_t kY = (argBitCount & 0x01) ? dY31 : dY32;
	register const uint32_t dYscaled = mulab( xShiftMax , kY );
	register const uint32_t Y0 =  (argBitCount & 0x01) ? Y31 : Y32;
	register const uint32_t approxScaled = dYscaled + Y0;
	register const uint32_t resShift = (bitCntToMax) >> 1;
	register const uint32_t approx = approxScaled >> resShift;
	return approx;
}


inline uint32_t getQuadCorr( uint32_t x, uint8_t argBitCount ) {
	// Constants
	static const double xValOnMaxErrorF = 1.0 / ( 4.0 * ( sqrt(2.0) - 1.0 ) * ( sqrt(2.0) - 1.0 ) );
	static const double xValOnStartF = 1.0;	
	static const double xMaxErrLF = xValOnMaxErrorF - xValOnStartF;
	static const double xMaxErrRF = xValOnStartF * 2.0 - xValOnMaxErrorF;
	static const double OneDivMaxErrLF = 1.0 / xMaxErrLF;
	static const double OneDivMaxErrRF = 1.0 / xMaxErrRF;
	static const double kCorr32FL = ((4.5 * sqrt(2.0)) * (1 << 16));
	static const double kCorr31FL = ((4.5 ) * (1 << 16));
	static const double kCorr32FR = ((4.1 * sqrt(2.0)) * (1 << 16));
	static const double kCorr31FR = ((4.1) * (1 << 16));

	static const uint32_t xMaxErrL = xMaxErrLF * ( static_cast<uint64_t>(1) << (32) );
	static const uint32_t xMaxErrR = xMaxErrRF * ( static_cast<uint64_t>(1) << (32) );
	static const uint32_t OneDivMaxErrL = OneDivMaxErrLF * ( static_cast<uint64_t>(1) << (30) );
	static const uint32_t OneDivMaxErrR = OneDivMaxErrRF * ( static_cast<uint64_t>(1) << (30) );
	static const uint32_t kCorrL[2] = { kCorr32FL, kCorr31FL };
	static const uint32_t kCorrR[2] = { kCorr32FR, kCorr31FR };


	// Calculate X axis value
	register const uint8_t bitCntToMax = 32 - argBitCount;
	register const uint32_t xShiftMax = ( x << ( bitCntToMax + 1 ) );
 	register const uint32_t xCorrScale = ( xShiftMax < xMaxErrL ) ? OneDivMaxErrL : OneDivMaxErrR;
	register const uint32_t xDiff = ( xShiftMax < xMaxErrL ) ? (xMaxErrL - xShiftMax) : ( xShiftMax - xMaxErrL);
	register const uint32_t qArgScaled = mulab( xDiff, xCorrScale );
	register const uint32_t qArg = qArgScaled << 2;
	register const uint32_t quad = mulab(qArg, qArg);
	register const uint32_t oneMinusQuad = 0xFFFFFFFF - quad;

	register const uint32_t halfDxScaled = (oneMinusQuad < 0x80000000) ?  (0x80000000 - oneMinusQuad) : (oneMinusQuad - 0x80000000);
	register const uint32_t halfDx = halfDxScaled << 1;
	register const uint32_t coorParable = mulab( halfDx, halfDx );
	register const uint32_t corrInvParable = 0xFFFFFFFF - coorParable;

	register const uint32_t kKorrScaled = ( xShiftMax < xMaxErrL ) ? kCorrL[ argBitCount &0x01 ] : kCorrR[ argBitCount &0x01 ];
	register const uint32_t kShift = bitCntToMax >> 1;
	register const uint32_t kKorr = kKorrScaled >> kShift;
	register const uint32_t korrScaled = mulab(kKorr, corrInvParable);

	return korrScaled;
}


uint32_t sqrtFixed( uint32_t x ) {
	// Skip zero value
	if ( 0 == x ) return 0;
	// Calculate square root
	register const uint8_t argBitCount = getMsb( x );
	register const uint32_t sqrtLinearApprox = getSqrtLinearApprox(x, argBitCount);
	register const uint32_t sqrtSqrErrApprox = getSqrCorrY(x, argBitCount);
	register const uint32_t sqrtSqrApproxVal = sqrtLinearApprox + sqrtSqrErrApprox;
	register const uint32_t sqrtQuadCorr = getQuadCorr(x, argBitCount);
	register const uint32_t sqrtVal = sqrtSqrApproxVal + sqrtQuadCorr;
	// Get doubled from integer
	return sqrtVal;
}

double sqrtX( uint32_t x ) {
	const double sqrtVal =  sqrtFixed(x);
	return (sqrtVal / 65536);
}


int main (int argc, char argv[]){

	double maxPositiveError = 0.0;
	double maxNegativeError = 0.0;

	for ( uint32_t lsb = 0; lsb <= 16; lsb++ ) { 

		for(uint32_t x = 0; x < 65536; x++ ) {
			const uint32_t xQuad = (x << lsb);
			const double valCalc = sqrtX( xQuad );
			const double valOrig = sqrt( static_cast<double>(xQuad) );
			const double err = valCalc - valOrig;

			if ( maxPositiveError < err ) {
				maxPositiveError = err;
			}

			if ( maxNegativeError > err ) {
				maxNegativeError = err;
			}
		}
	}


	for(uint32_t x = 0; x < 65536; x++ ) {
		const uint32_t xQuad = x * x;
		const double valCalc = sqrtX( xQuad );
		const double valOrig = sqrt( static_cast<double>(xQuad) );
		const double err = valCalc - valOrig;

		if ( maxPositiveError < err ) {
			maxPositiveError = err;
		}

		if ( maxNegativeError > err ) {
			maxNegativeError = err;
		}
	}

	printf("Max: %1.8f\n", maxPositiveError );
	printf("Min: %1.8f\n", maxNegativeError );

	return 0;
}
