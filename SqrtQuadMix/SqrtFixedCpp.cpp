
 /********************************************************************************************
  * @file      Sqrt.s dedicated to STM32Fxx device
  * @author    Evgeny Sobolev
  * @version   V1.0.0
  * @date      2026-011-01
  *
  *********************************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "clz.h"
#include "multiply.h"


inline uint32_t abs_diff(uint32_t a, uint32_t b) {
    uint32_t res;
    __asm__ __volatile__ (
        "subs  %0, %1, %2 \n\t"
    	"it    cc         \n\t"
        "rsbcc %0, %0, #0 \n\t"
        : "=&r" (res)
        : "r" (a), "r" (b)
        : "cc"
    );
    return res;
}


inline uint32_t next_sqrt_step( uint32_t res, uint32_t arg, uint32_t hyp ) {
	uint32_t temp_lo, temp_hi;
	__asm__ __volatile__ (
	    "umull  %0, %1, %3, %3 \n\t"
	    "rsbs   %0, %0, #0     \n\t"
	    "sbcs   %0, %4, %1     \n\t"
	    "it     pl             \n\t"
	    "movpl  %2, %3         \n\t"
	    : "=&r" (temp_lo), "=&r" (temp_hi), "+r" (res)
	    : "r" (hyp), "r" (arg)
	    : "cc"
	);

	return res;
}


// sqrt(x), input value is Q32 output value is Q16.16
extern "C" uint32_t sqrtFixed( uint32_t arg ) {

	constexpr const uint32_t Y0_32bit = 0xb504f333;   // 3037000499
	constexpr const uint32_t dY_32bit = 0x4afb0ccc;   // 1257966796
	constexpr const uint32_t Y0_31bit = 0x80000000;   // 2147483648
	constexpr const uint32_t dY_31bit = 0x3504f333;   // 889516851
	constexpr const uint32_t xMaxMid  = 0x80000000;   //
	constexpr const uint32_t xMaxErr_32 = 0x7504F333; //1963258675
	constexpr const uint32_t yMaxErr_31 = 0x2463002;  //38154242
	constexpr const uint32_t yMaxErr_32 = 0x3375666;  //53958246

	static const struct LinearConstValues {
		uint32_t Y0;
		uint32_t dY;
		uint32_t yMaxErr;
	} constVals[] = {  { Y0_31bit, dY_31bit, yMaxErr_31 }, { Y0_32bit, dY_32bit, yMaxErr_32 } };

	if ( 0 == arg ) return 0;

	// Get leading zeros
	uint32_t zeroBitsCount = clz(arg);

	// Shift value, most bit will be skipped, other bits are scaled to [0..0xFFFFFFFF]
	uint32_t bitsToShift = zeroBitsCount + 1;

	// Shift, xLinear is [0..0xFFFFFFFF]
	uint32_t xLinear = arg << bitsToShift;

	// sqrt(x), can't be directly shifted, because of power of sqrt(2)
	// But it can be split into sqrt(2) * 2 ^ N and 2 ^N, make it.
	uint32_t sel = bitsToShift & 0x01;
	uint32_t Y0 = constVals[sel].Y0;
	uint32_t dY = constVals[sel].dY;

	uint32_t scaledLinearRes = Y0 + umull_32( dY, xLinear );

	//uint32_t dxMidErr = ( xLinear < xMaxMid ) ? ( xMaxMid - xLinear ) : ( xLinear - xMaxMid );
	uint32_t dxMidErr = abs_diff( xLinear, xMaxMid );

	dxMidErr = ( dxMidErr < xMaxMid) ? ( dxMidErr << 1) : 0xFFFFFFFF;

	uint32_t qxMid = umull_32( dxMidErr, dxMidErr );
	uint32_t qxMidInv = 0xFFFFFFFE - qxMid;

	uint32_t xMaxErr = xMaxErr_32;

	//uint32_t dxMaxErr = ( xLinear < xMaxErr ) ?
	//					( xMaxErr - xLinear ) :
	//					( xLinear - xMaxErr );


	uint32_t dxMaxErr = abs_diff( xLinear, xMaxErr );

	uint32_t qxMax = umull_32( dxMaxErr, dxMaxErr );
	uint32_t qxMaxInv = ( 0x3FFFFFFF - qxMax ) << 1;

	uint32_t par1 = smull_32( qxMaxInv, qxMidInv >> 1 ) << 1;
	uint32_t par2 = umull_32( qxMidInv, qxMid >> 1 );
	//par2 = umlal_32( par2, par2, 156766306 );
	par2 = umlal_32( par2, par2, 156706176 );

	uint32_t sum = umull_32( par1 +  par2, constVals[sel].yMaxErr << 1);

	uint32_t scaledResult = scaledLinearRes + sum;

	// Reverse right shift of y(x) is 2x lower then left shift of (x)
	uint32_t bitsToRevShift = (zeroBitsCount ) >> 1;

#if 0
	uint32_t bitsToDirShift = ( zeroBitsCount >> 1 ) << 1;
	scaledResult -= 44524;

	uint32_t maxErr = (44524 + 44531) / 2;
	uint32_t scaledArgValue32 = static_cast<uint64_t>(arg) << bitsToDirShift;

	// Step 1
	uint32_t hyp = scaledResult + maxErr;
	scaledResult = next_sqrt_step(scaledResult, scaledArgValue32, hyp );
	maxErr >>= 1;

	// Step 2
	hyp = scaledResult + maxErr;
	scaledResult = next_sqrt_step(scaledResult, scaledArgValue32, hyp );
	maxErr >>= 1;

	// Step 3
	hyp = scaledResult + maxErr;
	scaledResult = next_sqrt_step(scaledResult, scaledArgValue32, hyp );
	maxErr >>= 1;

	// Step 4
	hyp = scaledResult + maxErr;
	scaledResult = next_sqrt_step(scaledResult, scaledArgValue32, hyp );
	maxErr >>= 1;

	// Step 5
	hyp = scaledResult + maxErr;
	scaledResult = next_sqrt_step(scaledResult, scaledArgValue32, hyp );
	maxErr >>= 1;

	// Step 6
	hyp = scaledResult + maxErr;
	scaledResult = next_sqrt_step(scaledResult, scaledArgValue32, hyp );
	maxErr >>= 1;

	// Step 7
	hyp = scaledResult + maxErr;
	scaledResult = next_sqrt_step(scaledResult, scaledArgValue32, hyp );
	maxErr >>= 1;

	// Step 8
	hyp = scaledResult + maxErr;
	scaledResult = next_sqrt_step(scaledResult, scaledArgValue32, hyp );
	maxErr >>= 1;

	// Step 9
	hyp = scaledResult + maxErr;
	scaledResult = next_sqrt_step(scaledResult, scaledArgValue32, hyp );
	maxErr >>= 1;
#endif

	uint32_t res = scaledResult >> bitsToRevShift;
	return res;
}






/*
 // sqrt(x), input value is Q32 output value is Q16.16
extern "C" uint32_t sqrtFixed( uint32_t arg ) {

	constexpr const uint32_t Y0_32bit = 0xb504f333;   // 3037000499
	constexpr const uint32_t dY_32bit = 0x4afb0ccc;   // 1257966796
	constexpr const uint32_t Y0_31bit = 0x80000000;   // 2147483648
	constexpr const uint32_t dY_31bit = 0x3504f333;   // 889516851
	constexpr const uint32_t xMaxMid  = 0x80000000;   //
	constexpr const uint32_t xMaxErr_32 = 0x7504F333; //1963258675
	constexpr const uint32_t yMaxErr_31 = 0x2463002;  //38154242
	constexpr const uint32_t yMaxErr_32 = 0x3375666;  //53958246

	static const struct LinearConstValues {
		uint32_t Y0;
		uint32_t dY;
		uint32_t yMaxErr;
	} constVals[] = {  { Y0_31bit, dY_31bit, yMaxErr_31 }, { Y0_32bit, dY_32bit, yMaxErr_32 } };

	if ( 0 == arg ) return 0;

	// Get leading zeros
	uint32_t zeroBitsCount = clz(arg);

	// Shift value, most bit will be skipped, other bits are scaled to [0..0xFFFFFFFF]
	uint32_t bitsToShift = zeroBitsCount + 1;

	// Shift, xLinear is [0..0xFFFFFFFF]
	uint32_t xLinear = arg << bitsToShift;

	// sqrt(x), can't be directly shifted, because of power of sqrt(2)
	// But it can be split into sqrt(2) * 2 ^ N and 2 ^N, make it.
	uint32_t sel = bitsToShift & 0x01;
	uint32_t Y0 = constVals[sel].Y0;
	uint32_t dY = constVals[sel].dY;

	uint32_t scaledLinearRes = Y0 + umull_32( dY, xLinear );

	uint32_t dxMidErr = ( xLinear < xMaxMid ) ? ( xMaxMid - xLinear ) : ( xLinear - xMaxMid );
	dxMidErr = ( dxMidErr < xMaxMid) ? ( dxMidErr << 1) : 0xFFFFFFFF;

	uint32_t qxMid = umull_32( dxMidErr, dxMidErr );
	uint32_t qxMidInv = 0xFFFFFFFF - qxMid;

	uint32_t xMaxErr = xMaxErr_32;
	uint32_t dxMaxErr = ( xLinear < xMaxErr ) ?
						( xMaxErr - xLinear ) :
						( xLinear - xMaxErr );

	dxMaxErr = ( dxMaxErr < xMaxMid ) ? ( dxMaxErr << 1 ) : 0xFFFFFFFF;

	uint32_t qxMax = umull_32( dxMaxErr, dxMaxErr );
	uint32_t qxMaxInv = 0xFFFFFFFF - qxMax;

	uint32_t par1 = umull_32( qxMaxInv, qxMidInv );
	uint32_t par2 = umull_32( qxMidInv, qxMid );
	//par2 = umlal_32( par2, par2, 85899345 );

	uint32_t scaledPar1 = umull_32(par1, constVals[sel].yMaxErr);
	uint32_t scaledPar2 = umull_32(par2, constVals[sel].yMaxErr);
	uint32_t sum = scaledPar1 +  scaledPar2;

	uint32_t scaledResult = scaledLinearRes + sum;

	// Reverse right shift of y(x) is 2x lower then left shift of (x)
	uint32_t bitsToRevShift = (zeroBitsCount ) >> 1;
	uint32_t res = scaledResult >> bitsToRevShift;

	return res;
}

 */

