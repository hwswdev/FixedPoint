/*
 * AsciiSymbol.cpp
 *
 *  Created on: Feb 24, 2025
 *      Author: Evgeny
 */

#include "FractionalToUint32.h"
#include "AsciiTable.h"

// Function get string of decimal symbols
// Each of decimal symbol is fractional part of value
// In format 012345678, i.e 0,1,2,3..,8 is numbers if fractional value
// i.e. XXXX.012345678
// Function returns total symbol count (0..9) and value as result
size_t fractionalToUint32( const char *str,  uint32_t& result ) {
	static constexpr const size_t   DeciamlDigitCount = 10;
	static constexpr const uint32_t MaxFracSymbolCount = 9;
	// It is the matrix of weight of each number. I mean Ai * 1/(10^i)
	// So the digit is Sum(Ai * 1/(2^i)), i = 0..31
	static const uint32_t convTable[MaxFracSymbolCount][DeciamlDigitCount] = {
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
			};

	if ( ( nullptr == str ) || (!*str) ) return 0;

	size_t index = 0;
	uint32_t sum = 0;

	while(  ( index < MaxFracSymbolCount ) && ( str[index] >= '0' ) && ( str[index] <= '9' )  ) {
		const uint8_t  didg = str[index] - '0';
		sum += convTable[index][didg];
		index++;
	}
	//sum += 0x01;

	result = sum;

	return index;
}


size_t decimalToUint32( const char *str,  uint32_t& result, size_t maxDecSymCount ) {
	static constexpr const size_t   MaxDecimalSymbolCount = 10;
	size_t index = 0;
	uint32_t sum = 0;
	uint8_t digit = 0;

	while(  asciiToDecimal( str[index], digit ) && index < MaxDecimalSymbolCount ) {
		if ( index < maxDecSymCount ) {
			sum = sum * 10;
			sum += digit;
		}
		index++;
	}
	result = sum;
	return index;

}


size_t hexToUint32( const char *str,  uint32_t& result ) {
	static constexpr const size_t   MaxDecimalSymbolCount = 10;
	size_t index = 0;
	uint32_t sum = 0;
	uint8_t digit = 0;

	while(  asciiToNibble( str[index], digit ) && index < MaxDecimalSymbolCount ) {
		sum <<= 4;
		sum |= digit;
		index++;
	}
	result = sum;
	return index;
}

size_t asciiToFpFractional( const char *str,  bool& negative, uint32_t& decimal, uint32_t& fractional ) {
	size_t offset = 0;
	// Skip spaces
	while( ' ' == str[offset] ) { offset++; };
	// Get sign
	const char mayBeSign = str[offset];
	if ( isSignSymbol( mayBeSign ) ) {
		negative = ( '-' == mayBeSign );
		offset++;
	} else {
		negative = false;
	}

	// Get decimal value
	const size_t decimalSize = decimalToUint32( &str[offset], decimal );
	offset += decimalSize;
	if ( 0 == decimalSize ) decimal = 0;

	// Get fractional value
	const char mayBeDot = str[offset];
	if ( isDotSymbol( mayBeDot ) ) {
		offset++;
		const size_t fracSize = fractionalToUint32( &str[offset], fractional );
		offset += fracSize;
		if  ( 0 == fracSize ) fractional = 0;
		const size_t len = (( fracSize ) || (decimalSize)) ? offset : 0;
		return len;
	}

	const size_t len = (decimalSize) ? offset : 0;
    return len;
}


size_t asciiToFpDecimal( const char *str,  bool& negative, uint32_t& decimal, uint32_t& fractional, size_t maxFracSymCount = 9 ) {
	static constexpr const char SymbolMinus = '-';
	size_t offset = 0;
	// Skip spaces
	while( ' ' == str[offset] ) { offset++; };

	// Get sign
	const char mayBeSign = str[offset];
	if ( isSignSymbol( mayBeSign ) ) {
		negative = ( SymbolMinus == mayBeSign );
		offset++;
	} else {
		negative = false;
	}

	// Get decimal value
	const size_t decimalSize = decimalToUint32( &str[offset], decimal );
	offset += decimalSize;
	if ( 0 == decimalSize ) decimal = 0;

	// Get fractional value
	const char mayBeDot = str[offset];
	if ( isDotSymbol( mayBeDot ) ) {
		offset++;
		const size_t fracSize = decimalToUint32( &str[offset], fractional, maxFracSymCount );
		offset += fracSize;
		if  ( 0 == fracSize ) fractional = 0;
		const size_t len = (( fracSize ) || (decimalSize)) ? offset : 0;
		return len;
	}

	const size_t len = (decimalSize) ? offset : 0;
    return len;
}

size_t asciiToFixed( const char *str,  const uint8_t binDotPos, uint32_t& value ) {
	static const size_t BinDotPosMax = (sizeof(uint32_t) * 8);
	uint32_t dec = 0, frac = 0;
	bool neg;
	if ( binDotPos > BinDotPosMax ) return 0;
 	const size_t sz = asciiToFpFractional( str, neg, dec, frac );
	if ( 0 == sz ) return 0;
	const uint32_t decValue = (dec << binDotPos);
	const uint32_t fracValue = frac >> (32 - binDotPos);
	const uint32_t result = decValue | fracValue;
	value = neg ? (-result) : (result);
	return sz;
}

size_t asciiToDecimal( const char *str,  const uint8_t decDotPos, uint32_t& value ) {
	static const uint32_t MaxPow10 = 10;
	static const uint32_t pow10[MaxPow10] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
	uint32_t dec = 0, frac = 0;
	bool neg;
	if ( decDotPos > ( MaxPow10 - 1) ) return 0;
	const size_t sz = asciiToFpDecimal( str, neg, dec, frac,  decDotPos );
	if ( 0 == sz ) return 0;
	const uint32_t decValue  = dec  * pow10[decDotPos];
	const uint32_t fracValue = frac;
	const uint32_t result = decValue + fracValue;
	value = neg ? (-result) : (result);
	return sz;
}
