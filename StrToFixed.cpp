
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
	// Approximation
	// Lets's look at the typical approximation
	// One side     Number = SumOf( Ai * 1/(10^i ), i=1..N, N=9
	// Other side   Number = SumOf( Bj * 1/(2^j ),  i=1..N, N=32
	// Lets's look at 1/(10 ^ i). In fact it is scale.
	// If I get 1/(10^i), as SumOf( Bj * 1/(2^j ), I will get Bj
	// In fact it is scale between two triangles with fixed angles (right triangle)
	static const uint32_t conv10Arr[ConversionArraySize + 1] = {
			0x19999999,   //  1/10
			0x028f5c28,   //  1/100
			0x00418937,   //  1/1000
			0x00068db8,   //  1/10000
			0x0000a7c5,   //  1/100000
			0x000010c6,   //  1/1000000
			0x000001ad,   //  1/10000000
			0x0000002a,   //  1/100000000
			0x00000004,   //  1/1000000000
			0x00000039	  //  1 - ( 1/10 + 1/100 + .... + 1/1000000000 )
	};

	// So I believe, It is better way to get 2 points of approximation ( center each of Point(i) near 5 * 1/(10^i) ).
	// But I use one point and Zero. Because of Math. I will get precision

	// Check string
	if ( ( nullptr == str ) || (!*str) ) return 0;

	// Calculate it
	size_t index = 0;
	uint32_t addErrSingle =  conv10Arr[ConversionArraySize] / ConversionArraySize;
	uint32_t addErrOther  =  conv10Arr[ConversionArraySize] - addErrSingle * ConversionArraySize;

	uint32_t sum = addErrOther;

	while( ( index < ConversionArraySize ) && ( str[index] >= '0' ) && ( str[index] <= '9' ) ) {
		const uint8_t didg = str[index] - '0';
		// So, I tried to split error bits between digits without thinking about it
		sum += didg * conv10Arr[index] + addErrSingle;
		index++;
	}

	// I add half of next digit, because of precision
	//if ( index < ConversionArraySize ) {
	//	sum += 5 * conv10Arr[index];
	//}

	return sum;
}


uint32_t convertFractional2( const char *str ) {

	static constexpr const uint32_t ConversionArraySize = 9;
	// It is the matrix of weight of each number. I mean Ai * 1/(10^i)
	// So the digit is Sum(Ai * 1/(10^i)), i = 0..9
	static const uint32_t convTable[ConversionArraySize + 1][10] = {
			{ 0x00000000,   0x1999999A,   0x33333333,   0x4CCCCCCD,   0x66666667,
			  0x80000000,   0x9999999A,   0xB3333333,   0xCCCCCCCD,   0xE6666667 },
			{ 0x00000000,   0x028F5C29,   0x051EB852,   0x07AE147B,   0x0A3D70A4,
			  0x0CCCCCCD,   0x0F5C28F6,   0x11EB851F,   0x147AE148,   0x170A3D71 },
			{ 0x00000000,   0x00418937,   0x0083126F,   0x00C49BA6,   0x010624DD,
			  0x0147AE14,   0x0189374C,   0x01CAC083,   0x020C49BA,   0x024DD2F2 },
			{ 0x00000000,   0x00068DB9,   0x000D1B71,   0x0013A92A,   0x001A36E3,
			  0x0020C49C,   0x00275254,   0x002DE00D,   0x00346DC6,   0x003AFB7F },
			{ 0x00000000,   0x0000A7C6,   0x00014F8B,   0x0001F751,   0x00029F17,
			  0x000346DC,   0x0003EEA2,   0x00049668,   0x00053E2D,   0x0005E5F3 },
			{ 0x00000000,   0x000010C7,   0x0000218E,   0x00003255,   0x0000431C,
			  0x000053E3,   0x000064AA,   0x00007571,   0x00008638,   0x000096FF },
			{ 0x00000000,   0x000001AD,   0x0000035B,   0x00000508,   0x000006B6,
			  0x00000863,   0x00000A11,   0x00000BBE,   0x00000D6C,   0x00000F19 },
			{ 0x00000000,   0x0000002B,   0x00000056,   0x00000081,   0x000000AC,
			  0x000000D7,   0x00000102,   0x0000012D,   0x00000158,   0x00000183 },
			{ 0x00000000,   0x00000004,   0x00000009,   0x0000000D,   0x00000011,
			  0x00000015,   0x0000001A,   0x0000001E,   0x00000022,   0x00000027 },
			{ 0xCCCCCCCC,   0x0093FE14,   0x00111EC8,   0x00000001,   0x00D93CA8,
			  0x00D92160,   0xE407A559,   0x00111122,   0x00111122,   0x00663000 }
			};

	if ( ( nullptr == str ) || (!*str) ) return 0;

	size_t index = 0;
	uint32_t sum = 0;

	while( ( index < ConversionArraySize ) && ( str[index] >= '0' ) && ( str[index] <= '9' ) ) {
		const uint8_t  didg = str[index] - '0';
		sum += convTable[index][didg];
		index++;
	}
	sum += 0x01;

	return sum;
}

