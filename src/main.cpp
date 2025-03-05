/*
 * main.cpp
 *
 *  Created on: Feb 22, 2025
 *      Author: Evgeny
 */

#include <stdint.h>
#include <stdio.h>

#include "FixedPoint.h"
#include "DataDump.h"
#include "Axis.h"


typedef int32_t FixedPointBaseType;
static constexpr const size_t FixedPointFracBitCount = 10;
typedef FixedPoint<FixedPointBaseType, FixedPointFracBitCount> Fixed;

static constexpr const size_t BufferSize = 1024;
char buffer[BufferSize];

void test() {


	const size_t sz = sizeof( Fixed );

	asm("bkpt");
	const Fixed mmPerCircleX = 4.990;
	const Fixed stepsPerCircle = 200;
	Fixed stepSizeMm = mmPerCircleX / stepsPerCircle;
	asm("bkpt");
	stepSizeMm = stepSizeMm;
	asm("bkpt");
	float stepMm = stepSizeMm.toFloat(6);
	asm("bkpt");
	sprintf( buffer, "value = %f, sz = %d", stepMm, sz );
	asm("bkpt");

}


extern "C" int main(void){

	test();

	return 0;
}
