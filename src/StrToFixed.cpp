
/*
  ********************************************************************************************
  * @file      StrToFixed.cpp
  * @author    Evgeny Sobolev
  * @version   V1.0.0
  * @date      2025-03-13
  * @description  Convert string to uint32_t value
  * *******************************************************************************************
*/

#include <stddef.h>
#include <stdint.h>

uint32_t convertFractional( const char *str ) {

	static constexpr const uint32_t ConversionArraySize = 9;

	static const uint32_t conv10Arr[ConversionArraySize + 1] = {
			0x19999999,
			0x028f5c28,
			0x00418937,
			0x00068db8,
			0x0000a7c5,
			0x000010c6,
			0x000001ad,
			0x0000002a,
			0x00000004,
			0x00000039
	};

	if ( ( nullptr == str ) || (!*str) ) return 0;

	size_t index = 0;
	uint32_t addErrSingle =  conv10Arr[ConversionArraySize] / ConversionArraySize;
	uint32_t addErrOther  =  conv10Arr[ConversionArraySize] - addErrSingle * ConversionArraySize;

	uint32_t sum = addErrOther;

	while( ( index < ConversionArraySize ) && ( str[index] >= '0' ) && ( str[index] <= '9' ) ) {
		const uint8_t didg = str[index] - '0';
		sum += didg * conv10Arr[index] + addErrSingle;
		index++;
	}
	if ( index < ConversionArraySize ) {
		sum += 5 * conv10Arr[index];
	}

	return sum;
}

