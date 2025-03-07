/*
 * main.cpp
 *
 *  Created on: Feb 22, 2025
 *      Author: Evgeny
 */

#include <stdint.h>
#include <stdio.h>

#include "MixedPoint.h"
#include "DecimalPoint.h"
#include "DataDump.h"
#include "Axis.h"

static constexpr const size_t BufferSize = 1024;
char buffer[BufferSize];


template<typename Type>
constexpr Type sinZeroToPi4( const Type x ) {
	typedef MixedPoint<uint32_t, uint32_t, 0, 15> PowOfArgType;
	typedef MixedPoint<int32_t, int64_t, 0, 30> SinKoeffType;

	constexpr const SinKoeffType k3  = static_cast<SinKoeffType>( -1 ) / 6;
	constexpr const SinKoeffType k5  = static_cast<SinKoeffType>(  1 ) / 120;
	constexpr const SinKoeffType k7  = static_cast<SinKoeffType>( -1 ) / 5040;
	constexpr const SinKoeffType k9  = static_cast<SinKoeffType>(  1 ) / 362880;
	constexpr const SinKoeffType k11 = static_cast<SinKoeffType>( -1 ) / 39916800;

	const PowOfArgType x1  = x;
	const PowOfArgType x2  = x1 * x1;
	const PowOfArgType x3  = x2 * x1;
	const PowOfArgType x5  = x3 * x2;
	const PowOfArgType x7  = x5 * x2;
	const PowOfArgType x9  = x7 * x2;
	const PowOfArgType x11  = x9 * x2;

	const SinKoeffType sin1 =  ( x1 );
	const SinKoeffType sin3 =  ( k3  * x3  );
	const SinKoeffType sin5 =  ( k5  * x5  );
	const SinKoeffType sin7 =  ( k7  * x7  );
	const SinKoeffType sin9 =  ( k9  * x9  );
	const SinKoeffType sin11 = ( k11  * x11  );

	const Type result  = sin1 + sin3 + sin5 + sin7 + sin9 + sin11;

	return result;
}


template<typename Type>
constexpr Type cosZeroToPi4( const Type x ) {
	typedef MixedPoint<uint32_t, uint32_t, 0, 15> PowOfArgType;
	typedef MixedPoint<int32_t, int64_t, 0, 30> SinKoeffType;

	constexpr const SinKoeffType k0  = static_cast<SinKoeffType>(  1 );
	constexpr const SinKoeffType k2  = static_cast<SinKoeffType>( -1 ) / 2;
	constexpr const SinKoeffType k4  = static_cast<SinKoeffType>(  1 ) / 24;
	constexpr const SinKoeffType k6  = static_cast<SinKoeffType>( -1 ) / 720;
	constexpr const SinKoeffType k8  = static_cast<SinKoeffType>(  1 ) / 40320;
	constexpr const SinKoeffType k10 = static_cast<SinKoeffType>( -1 ) / 3628800;
	constexpr const SinKoeffType k12 = static_cast<SinKoeffType>(  1 ) / 479001600;

	const PowOfArgType x1   = x;
	const PowOfArgType x2   = x1 * x1;
	const PowOfArgType x4   = x2 * x2;
	const PowOfArgType x6   = x4 * x2;
	const PowOfArgType x8   = x6 * x2;
	const PowOfArgType x10  = x8 * x2;
	const PowOfArgType x12  = x10 * x2;

	const SinKoeffType cos0 =  ( 1 );
	const SinKoeffType cos2 =  ( k2   * x2  );
	const SinKoeffType cos4 =  ( k4   * x4  );
	const SinKoeffType cos6 =  ( k6   * x6  );
	const SinKoeffType cos8 =  ( k8   * x8  );
	const SinKoeffType cos10 = ( k10  * x10  );
	const SinKoeffType cos12 = ( k12  * x12  );

	const Type result  =  cos0 + cos2 + cos4 + cos6 + cos8 + cos10 + cos12;

	return result;
}



void test() {

	typedef MixedPoint<int32_t, int64_t, 0, 26> SinCosType;

	const SinCosType Pi = 3.1415926;
	const SinCosType Pi8 = Pi / 8;
	const SinCosType Pi4 = Pi / 4;
	const SinCosType Pi2 = Pi / 2;

	auto sin0   = sinZeroToPi4< SinCosType >(0);
	auto sinPi8 = sinZeroToPi4< SinCosType >(Pi8);
	auto sinPi4 = sinZeroToPi4< SinCosType >(Pi4);
	auto sinPi2 = sinZeroToPi4< SinCosType >(Pi2);

	auto cos0   = cosZeroToPi4< SinCosType >(0);
	auto cosPi8 = cosZeroToPi4< SinCosType >(Pi8);
	auto cosPi4 = cosZeroToPi4< SinCosType >(Pi4);
	auto cosPi2 = cosZeroToPi4< SinCosType >(Pi2);


	size_t offset = 0;
	offset += sprintf( buffer + offset, "sin(0) = %f, sin(pi/8) = %f,  sin(pi/4) = %f,  sin(pi/2) = %f\n ", sin0.toFloat(),  sinPi8.toFloat(),  sinPi4.toFloat(), sinPi2.toFloat() );
	offset += sprintf( buffer + offset, "cos(0) = %f, cos(pi/8) = %f,  cos(pi/4) = %f,  cos(pi/2) = %f\n ", cos0.toFloat(),  cosPi8.toFloat(),  cosPi4.toFloat(), cosPi2.toFloat() );
	asm("bkpt");
}


extern "C" int main(void){

	test();

	return 0;
}
