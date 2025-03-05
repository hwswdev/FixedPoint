/*
 * DataDump.cpp
 *
 *  Created on: Feb 24, 2025
 *      Author: Evgeny
 */

#include <type_traits>
#include "DataDump.h"
#include "AsciiTable.h"


template<typename Type>
static inline size_t dumpHexInternal( char* const str, const size_t maxSize, Type value ) {
	constexpr const size_t SymPerByte = 2;
	constexpr const size_t BitPerSym = 4;
	constexpr const size_t ByteCount = sizeof(Type);
	constexpr const size_t SymCount = ByteCount * SymPerByte;
	constexpr const size_t BitCount = SymCount * BitPerSym;

	static_assert( ( std::is_same<Type, uint8_t>::value  ||
					 std::is_same<Type, uint16_t>::value ||
					 std::is_same<Type, uint32_t>::value ||
					 std::is_same<Type, uint64_t>::value ||
					 std::is_same<Type, int8_t>::value   ||
					 std::is_same<Type, int16_t>::value  ||
					 std::is_same<Type, int32_t>::value  ||
					 std::is_same<Type, int64_t>::value ),  "Something wrong. Please check value type" );

	// Check for maximum length
	if ( maxSize < SymCount ) return 0;

	// Dump value into the buffer
	size_t count = BitCount;
	size_t index = 0;
	do {
		count -= BitPerSym;
		const uint8_t dumpValue = ( value >> count ) & 0x0F;
		nibbleToAscii( dumpValue, str[index]  );
		index++;
	} while( count );

	// Return size
	return SymCount;
}


template<typename Type>
size_t dumpIntegerInternal( char * const str, const size_t maxSize,
							const Type value, const Expand expand,
							const size_t expandSymCount, const Sign sign ) {

	static constexpr const char Space = ' ';
	static constexpr const char Plus  = '+';
	static constexpr const char Minus = '-';

	// Decimal symbol count per different symbol types
	static constexpr const size_t DecSymCountPerInt8  = 3;
	static constexpr const size_t DecSymCountPerInt16 = 5;
	static constexpr const size_t DecSymCountPerInt32 = 10;
	static constexpr const size_t DecSymCountPerInt64 = 21;
	// Size of symbol sign ('-')
	static constexpr const size_t SymCountOfSign = sizeof('-');

	// Buffer size to print integer (will be allocated on stack)
	static constexpr const size_t MaxIntegerSymCountWithoutSign = ( 1 == sizeof(Type) ) ? DecSymCountPerInt8 :
																  ( 2 == sizeof(Type) ) ? DecSymCountPerInt16 :
																  ( 4 == sizeof(Type) ) ? DecSymCountPerInt32 : DecSymCountPerInt64;

	// Total buffer size to print integer and sign (will be allocated on stack)
	static constexpr const size_t MaxSymCount = MaxIntegerSymCountWithoutSign + SymCountOfSign;

	// Check types
	static_assert( 	( std::is_same<Type, uint8_t>::value )  ||
					( std::is_same<Type, uint16_t>::value ) ||
					( std::is_same<Type, uint32_t>::value ) ||
					( std::is_same<Type, uint64_t>::value ) ||
					( std::is_same<Type, int8_t>::value )   ||
					( std::is_same<Type, int16_t>::value )  ||
					( std::is_same<Type, int32_t>::value )  ||
					( std::is_same<Type, int64_t>::value ) , "Other types of integer are not supported" );

	if ( 0 == maxSize ) return 0;

	// Get unsigned value
	const bool negative = ( value < 0 );
	Type positiveValue  = (negative) ? (-value) : (value);

	// Allocate buffer on stack
	char temp[MaxSymCount];

	// Dump numbers into the on stack buffer, reverse sequence
	size_t symIndex = 0;
	for( ; symIndex < MaxIntegerSymCountWithoutSign; ) {
		const uint8_t curSymValue = positiveValue % 10;
		positiveValue /= 10;
		nibbleToAscii( curSymValue, temp[symIndex] );
		symIndex++;
		// Break it
		if ( 0 == positiveValue ) break;
	}

	// Check that value is printed correctly
	if ( 0 != positiveValue ) return 0;

	// Check that buffer is bigger then value
	if ( symIndex > maxSize ) return 0;

	size_t strIndex = 0;
	// Append sign ('-'/'+'/' ')
	if  ( ( Expand::NotUsed == expand ) ||
		  ( Expand::BySpace == expand) ) {
		// Append sign into the temp buffer
		if ( Sign::None != sign ) {
			// Display sign
			if ( negative ) {
				// Display negative sign
				temp[symIndex++] = Minus;
			} else {
				// Display positive sign or space
				if ( Sign::NegativeOrPositive == sign ) {
					temp[symIndex++] = Plus;
				} else if  ( Sign::NegativeOrSpace == sign ) {
					temp[symIndex++] = Space;
				}
			}
		}

		// Append spaces into the main buffer
		if ( Expand::BySpace == expand ) {
			// TO DO: append spaces into the main buffer
			const size_t numTotalSymbolCount = symIndex;
			// Used size is total size of symbols in the both buffers
			const size_t usedSize = symIndex + strIndex;
			// Calculate remain size in the string buffer
			const size_t remainStrSize = ( maxSize > usedSize ) ? ( maxSize - usedSize ) : 0;
			// Calculate zero count to append into the str buffer
			size_t remainSpaceCount = ( expandSymCount > numTotalSymbolCount ) ? (expandSymCount - numTotalSymbolCount) : 0;
			// Check I can print all the symbols
			if ( remainStrSize < remainSpaceCount ) return 0;
			// Append
			while( remainSpaceCount ) {
				str[strIndex] = Space;
				strIndex++;
				remainSpaceCount--;
			}
		}

	} else {
		// Number symbols count
		const size_t numSymbolCount = symIndex;

		// Append sign into the main buffer
		if ( Sign::None != sign ) {
			// Check buffer size
			if ( 0 == maxSize ) return 0;
			// Display sign
			if ( negative ) {
				// Display negative sign
				str[strIndex++] = Minus;
			} else {
				// Display positive sign or space
				if ( Sign::NegativeOrPositive == sign ) {
					str[strIndex++] = Plus;
				} else if  ( Sign::NegativeOrSpace == sign ) {
					str[strIndex++] = Space;
				}
			}
		}

		// Append zeros into the main buffer
		if ( Expand::ByZeros == expand ) {
			// Used size is total size of symbols in the both buffers
			const size_t usedSize = symIndex + strIndex;
			// Calculate remain size in the string buffer
			const size_t remainStrSize = ( maxSize > usedSize ) ? ( maxSize - usedSize ) : 0;
			// Calculate zero count to append into the str buffer
			size_t remainZerosCount = ( expandSymCount > numSymbolCount ) ? (expandSymCount - numSymbolCount) : 0;
			// Check I can print all the symbols
			if ( remainStrSize < remainZerosCount ) return 0;
			// Append
			while( remainZerosCount ) {
				nibbleToAscii( 0, str[strIndex] );
				strIndex++;
				remainZerosCount--;
			}
		}
	}

	// Copy buffer to the general buffer
	while( symIndex > 0 ) {
		if ( strIndex >= maxSize ) return 0;
		str[strIndex++] = temp[--symIndex];
	}

	// Return
	return strIndex;
}

template<typename Type>
size_t dumpFixedPointInternal( char * const str, const size_t maxSize, const Type value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount,
					   const size_t fracSymCount ) {


	// Check types
	static_assert( 	( std::is_same<Type, uint8_t>::value )  ||
					( std::is_same<Type, uint16_t>::value ) ||
					( std::is_same<Type, uint32_t>::value ) ||
					( std::is_same<Type, uint64_t>::value ) ||
					( std::is_same<Type, int8_t>::value )   ||
					( std::is_same<Type, int16_t>::value )  ||
					( std::is_same<Type, int32_t>::value )  ||
					( std::is_same<Type, int64_t>::value ) , "Other types of integer are not supported" );

	static constexpr const char Dot = '.';
	static constexpr const char Minus = '-';
	static constexpr const char Plus = '+';
	static constexpr const char Space = ' ';
	static constexpr const size_t MinSizeOfString = 3;


	const Type Multiplyer = pow( 10, (fracSymCount + 1) );
	const bool negative = (value < 0);
	const Type positiveValue = (negative) ? - value : value;
	const Type decimalPositiveValue = positiveValue >> fracBitCount;
	const Type fracPositiveValue = positiveValue & ( (1 << fracBitCount) - 1 );
	const Type decimalSignedValue = (negative) ? -decimalPositiveValue : decimalPositiveValue;

	if ( maxSize < MinSizeOfString ) return 0;

	// Dump sign
	size_t offset = 0;
	switch(sign){
		case Sign::NegativeOnly: if (negative ) { str[offset++] = Minus; }; break;
		case Sign::NegativeOrPositive: str[offset++] = (negative) ? Minus : Plus; break;
		case Sign::NegativeOrSpace: str[offset++] = (negative) ? Minus : Space; break;
		default: str[offset++] = 'W'; str[offset++] = 'T'; str[offset++] = 'F'; break;
	}

	// Dump decimal value
	const size_t decSymSize = dumpInteger( str + offset, maxSize - offset, decimalSignedValue, expand, expandSymCount, Sign::None );

	const size_t fracAndDotSymCount = fracSymCount + sizeof(Dot);
	if (  (decSymSize + fracAndDotSymCount) > maxSize ) return 0;

	// Dump dot ('.')
	offset += decSymSize;
	str[offset++] = Dot;

	// Dump fractional part
	Type fracPrintValue = 0;

	for( size_t index = 0; index < fracBitCount; index++ ) {
		const Type bitValue = ( fracPositiveValue >> index ) & 0x01;
		const size_t divShiftValue = fracBitCount - index;
		const Type bitWeight = (bitValue) ? ( Multiplyer >> divShiftValue ) : 0;
		fracPrintValue += bitWeight;
	}

	fracPrintValue /= 10;
	const size_t fracSymSize = dumpInteger( str + offset, maxSize - offset, static_cast<int32_t>(fracPrintValue), Expand::ByZeros, fracSymCount, Sign::None );
	if ( 0 == fracSymSize ) return 0;

	offset += fracSymSize;

	return offset;
}

template<typename Type>
size_t scanHex( char* const str, const size_t maxSize, const Type value ) {
	static constexpr const char Space = ' ';
	static constexpr const size_t BitsPerNibble = 4;
	Type res = 0;
	size_t strIndex = 0;
	while( (Space == str[strIndex]) && (strIndex < maxSize) ) { strIndex++; };
	while( strIndex < maxSize ) {
		uint8_t nibble = 0;
		if ( !asciiToNibble( str[strIndex], nibble) ) break;
		res <= BitsPerNibble;
		res |= nibble;
	}
	value = res;
	return strIndex;
}

template<typename Type>
size_t scanIntegerInternal( char* const str, const size_t maxSize, Type& value ) {
	static constexpr const char Space = (' ');
	static constexpr const char Plus = ('+');
	static constexpr const char Minus = ('-');
	static constexpr const Type Multiplyer = 10;

	// Decimal symbol count per different symbol types
	static constexpr const size_t DecSymCountPerInt8  = 3;
	static constexpr const size_t DecSymCountPerInt16 = 5;
	static constexpr const size_t DecSymCountPerInt32 = 10;
	static constexpr const size_t DecSymCountPerInt64 = 21;
	// Size of symbol sign ('-')
	static constexpr const size_t SymCountOfSign = sizeof( Minus );

	// Buffer size to print integer (will be allocated on stack)
	static constexpr const size_t MaxIntegerSymCountWithoutSign = ( 1 == sizeof(Type) ) ? DecSymCountPerInt8 :
																  ( 2 == sizeof(Type) ) ? DecSymCountPerInt16 :
																  ( 4 == sizeof(Type) ) ? DecSymCountPerInt32 : DecSymCountPerInt64;

	size_t strIndex = 0;
	while( ( strIndex < maxSize ) && ( Space == str[strIndex] ) ) { strIndex++; };
	// Skip if symbol '+' is used
	if ( Plus == str[strIndex] ) { strIndex++; };
	// Get if symbol '-' is used
	const bool negative = ( Minus == str[strIndex] );
	if (negative) { strIndex++; };

	value = 0;
	bool isDecimal = false;
	for ( size_t index = 0; index < MaxIntegerSymCountWithoutSign; index++, strIndex++ ) {
		uint8_t decValue = 0;
		const bool decimalSym = asciiToDecimal( str[strIndex],  decValue );
		isDecimal |= decimalSym;
		if ( !decimalSym ) break;
		value *= Multiplyer;
		value += decValue;
	}

	if ( !isDecimal ) return 0;
	value = (negative) ? -value : value;
	return strIndex;
}



template<typename Type>
size_t scanFixedPointInternal( char* const str, const size_t maxSize, Type& value, const size_t fracBitCount ) {

	static constexpr const char Dot = ('.');
	Type dec = 0;
	Type fract = 0;

	const size_t decSymCount = scanIntegerInternal( str, maxSize, dec );
	if ( 0 == decSymCount ) return 0;
	size_t offset = decSymCount;
	if ( Dot == str[offset] ) {
		offset++;
		if (offset >= maxSize) return 0;
		const size_t fracSymCount = scanIntegerInternal( str + offset, maxSize - offset, fract );
		if ( fracSymCount > 0 ) {
			if (dec < 0) fract = -fract;
			fract <<= fracBitCount;
			Type divisor = pow( 10, fracSymCount );
			fract /= divisor;
 		} else {
 			return 0;
 		}
	}

	value = (dec << fracBitCount) + fract;
	return offset;
}

size_t scanFixedPoint( char* const str, const size_t maxSize, uint8_t& value, const size_t fracBitCount ) {
	return scanFixedPointInternal( str, maxSize, value, fracBitCount );
}

size_t scanFixedPoint( char* const str, const size_t maxSize, int8_t& value, const size_t fracBitCount ) {
	return scanFixedPointInternal( str, maxSize, value, fracBitCount );
}

size_t scanFixedPoint( char* const str, const size_t maxSize, uint16_t& value, const size_t fracBitCount ) {
	return scanFixedPointInternal( str, maxSize, value, fracBitCount );
}

size_t scanFixedPoint( char* const str, const size_t maxSize, int16_t& value, const size_t fracBitCount ) {
	return scanFixedPointInternal( str, maxSize, value, fracBitCount );
}

size_t scanFixedPoint( char* const str, const size_t maxSize, uint32_t& value, const size_t fracBitCount ) {
	return scanFixedPointInternal( str, maxSize, value, fracBitCount );
}

size_t scanFixedPoint( char* const str, const size_t maxSize, int32_t& value, const size_t fracBitCount ) {
	return scanFixedPointInternal( str, maxSize, value, fracBitCount );
}

size_t scanFixedPoint( char* const str, const size_t maxSize, uint64_t& value, const size_t fracBitCount ) {
	return scanFixedPointInternal( str, maxSize, value, fracBitCount );
}

size_t scanFixedPoint( char* const str, const size_t maxSize, int64_t& value, const size_t fracBitCount ) {
	return scanFixedPointInternal( str, maxSize, value, fracBitCount );
}

size_t scanInteger(char* const str, const size_t maxSize, int8_t& value) {
	return scanIntegerInternal( str, maxSize, value );
}

size_t scanInteger(char* const str, const size_t maxSize, uint8_t& value) {
	return scanIntegerInternal( str, maxSize, value );
}

size_t scanInteger(char* const str, const size_t maxSize, int16_t& value) {
	return scanIntegerInternal( str, maxSize, value );
}

size_t scanInteger(char* const str, const size_t maxSize, uint16_t& value) {
	return scanIntegerInternal( str, maxSize, value );
}

size_t scanInteger(char* const str, const size_t maxSize, int32_t& value) {
	return scanIntegerInternal( str, maxSize, value );
}

size_t scanInteger(char* const str, const size_t maxSize, uint32_t& value) {
	return scanIntegerInternal( str, maxSize, value );
}

size_t scanInteger(char* const str, const size_t maxSize, int64_t& value) {
	return scanIntegerInternal( str, maxSize, value );
}

size_t scanInteger(char* const str, const size_t maxSize, uint64_t& value) {
	return scanIntegerInternal( str, maxSize, value );
}


// *********************************************************************************
// *  $GPGGA,002153.000,3342.6618,N,11751.3858,W,1,10,1.2,27.0,M,-34.2,M,,0000*5E
// *********************************************************************************


size_t dumpHex( char* const str, const size_t maxSize, const uint8_t value ) {
	return dumpHexInternal(str, maxSize, value);
}

size_t dumpHex( char* const str, const size_t maxSize, const int8_t value ) {
	return dumpHexInternal(str, maxSize, value);
}

size_t dumpHex( char* const str, const size_t maxSize, const uint16_t value ) {
	return dumpHexInternal(str, maxSize, value);
}

size_t dumpHex( char* const str, const size_t maxSize, const int16_t value ) {
	return dumpHexInternal(str, maxSize, value);
}

size_t dumpHex( char* const str, const size_t maxSize, const uint32_t value ) {
	return dumpHexInternal(str, maxSize, value);
}

size_t dumpHex( char* const str, const size_t maxSize, const int32_t value ) {
	return dumpHexInternal(str, maxSize, value);
}

size_t dumpHex( char* const str, const size_t maxSize, const uint64_t value ) {
	return dumpHexInternal(str, maxSize, value);
}

size_t dumpHex( char* const str, const size_t maxSize, const int64_t value ) {
	return dumpHexInternal(str, maxSize, value);
}


size_t dumpInteger( char * const str, const size_t maxSize, const uint8_t value , const Expand expand, const size_t expandSymCount, const Sign sign ) {
	return dumpIntegerInternal(str, maxSize, value, expand, expandSymCount, sign);
}

size_t dumpInteger( char * const str, const size_t maxSize, const int8_t value , const Expand expand, const size_t expandSymCount, const Sign sign ) {
	return dumpIntegerInternal(str, maxSize, value, expand, expandSymCount, sign);
}

size_t dumpInteger( char * const str, const size_t maxSize, const uint16_t value , const Expand expand, const size_t expandSymCount, const Sign sign ) {
	return dumpIntegerInternal(str, maxSize, value, expand, expandSymCount, sign);
}

size_t dumpInteger( char * const str, const size_t maxSize, const int16_t value , const Expand expand, const size_t expandSymCount, const Sign sign ) {
	return dumpIntegerInternal(str, maxSize, value, expand, expandSymCount, sign);
}

size_t dumpInteger( char * const str, const size_t maxSize, const uint32_t value , const Expand expand, const size_t expandSymCount, const Sign sign ) {
	return dumpIntegerInternal(str, maxSize, value, expand, expandSymCount, sign);
}

size_t dumpInteger( char * const str, const size_t maxSize, const int32_t value , const Expand expand, const size_t expandSymCount, const Sign sign ) {
	return dumpIntegerInternal(str, maxSize, value, expand, expandSymCount, sign);
}

size_t dumpInteger( char * const str, const size_t maxSize, const uint64_t value , const Expand expand, const size_t expandSymCount, const Sign sign ) {
	return dumpIntegerInternal(str, maxSize, value, expand, expandSymCount, sign);
}

size_t dumpInteger( char * const str, const size_t maxSize, const int64_t value , const Expand expand, const size_t expandSymCount, const Sign sign ) {
	return dumpIntegerInternal(str, maxSize, value, expand, expandSymCount, sign);
}


size_t dumpFixedPoint( char * const str, const size_t maxSize, uint8_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount ) {
	return dumpFixedPointInternal(str, maxSize, value, expand, expandSymCount, sign, fracBitCount, fracSymCount);
}

size_t dumpFixedPoint( char * const str, const size_t maxSize, uint16_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount ) {
	return dumpFixedPointInternal(str, maxSize, value, expand, expandSymCount, sign, fracBitCount, fracSymCount);
}

size_t dumpFixedPoint( char * const str, const size_t maxSize, uint32_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount ) {
	return dumpFixedPointInternal(str, maxSize, value, expand, expandSymCount, sign, fracBitCount, fracSymCount);
}

size_t dumpFixedPoint( char * const str, const size_t maxSize, uint64_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount ) {
	return dumpFixedPointInternal(str, maxSize, value, expand, expandSymCount, sign, fracBitCount, fracSymCount);
}

size_t dumpFixedPoint( char * const str, const size_t maxSize, int8_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount ) {
	return dumpFixedPointInternal(str, maxSize, value, expand, expandSymCount, sign, fracBitCount, fracSymCount);
}

size_t dumpFixedPoint( char * const str, const size_t maxSize, int16_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount ) {
	return dumpFixedPointInternal(str, maxSize, value, expand, expandSymCount, sign, fracBitCount, fracSymCount);
}

size_t dumpFixedPoint( char * const str, const size_t maxSize, int32_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount ) {
	return dumpFixedPointInternal(str, maxSize, value, expand, expandSymCount, sign, fracBitCount, fracSymCount);
}

size_t dumpFixedPoint( char * const str, const size_t maxSize, int64_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount ) {
	return dumpFixedPointInternal(str, maxSize, value, expand, expandSymCount, sign, fracBitCount, fracSymCount);
}

