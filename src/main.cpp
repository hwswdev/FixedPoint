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


typedef MixedPoint<int32_t, int32_t, 2, 4> MixedT1;
typedef MixedPoint<int32_t, int32_t, 2, 8> MixedT2;
typedef MixedPoint<int32_t, int32_t, 2, 2> MixedT3;
typedef MixedPoint<int32_t, int32_t, 0, 4> MixedT4;
typedef MixedPoint<int32_t, int32_t, 0, 2> MixedT5;
typedef MixedPoint<int32_t, int32_t, 3, 6> MixedT6;
typedef MixedPoint<int32_t, int32_t, 0, 6> MixedT7;


static constexpr const size_t BufferSize = 1024;
char buffer[BufferSize];

void test() {

	MixedT1 a;
	MixedT2 b;
	MixedT3 c;
	MixedT4 d;
	MixedT5 e;
	MixedT6 f;
	MixedT7 g;

	a = 10.625;
	b = a;
	c = a;
	d = a;
	e = a;
	f = a;
	g = a;


	size_t offset = 0;
	offset += sprintf( buffer + offset, "a = %f, b = %f, c = %f, d = %f, e = %f, f= %f, g = %f\n ", a.toFloat(), b.toFloat(), c.toFloat(), d.toFloat(), e.toFloat(), f.toFloat(), g.toFloat() );
	a = e;
	f = e;
	offset += sprintf( buffer + offset, "a = %f, b = %f, c = %f, d = %f, e = %f, f= %f, g = %f\n ", a.toFloat(), b.toFloat(), c.toFloat(), d.toFloat(), e.toFloat(), f.toFloat(), g.toFloat() );
	asm("bkpt");

#if 0
	const Fixed   mmPerCircleXfix   = 5.00;
	const Decimal mmPerCircleXdec   = 5.00;

	const Fixed   stepsPerCircleFix = 200;
	const Decimal stepsPerCircleDec = 200;

	Fixed   stepSizeMmFix = mmPerCircleXfix / stepsPerCircleFix;
	Decimal stepSizeMmDec = mmPerCircleXdec / stepsPerCircleDec;

	asm("bkpt");
	float stepMmFix = stepSizeMmFix.toFloat();
	float stepMmDec = stepSizeMmDec.toFloat();

	asm("bkpt");

	sprintf( buffer, "value = %f\n", stepMmFix );
	sprintf( buffer, "value = %f\n", stepMmDec );
	asm("bkpt");
#endif
}


extern "C" int main(void){

	test();

	return 0;
}
