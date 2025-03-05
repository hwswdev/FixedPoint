//**********************************************************************************************
// Serialization data convertor
// Copyright: Evgeny Sobolev 09.02.1984 y.b. VRN/RUS
// Created: 01.03.2025
//**********************************************************************************************

#include <stddef.h>
#include <stdint.h>
#include <type_traits>
#include "AsciiTable.h"
#include <math.h>

#pragma once


enum class Expand : uint8_t {
	NotUsed,		// Don't expand integer value by zeros or spaces
	ByZeros,		// Expand integer value by zeros
	BySpace			// Expand integer value by spaces
};

enum class Sign : uint8_t {
	NegativeOnly,			// Display sign of the digit, if and only if it is negative
	NegativeOrSpace,		// Display sign as '-' if it is negative, and space if it is positive
	NegativeOrPositive,		// Display sing as '-' if it is negative, and '+' if it is positive
	None					// Don't display sign
};

size_t dumpHex( char* const str, const size_t maxSize, const uint8_t value );
size_t dumpHex( char* const str, const size_t maxSize, const uint16_t value );
size_t dumpHex( char* const str, const size_t maxSize, const uint32_t value );
size_t dumpHex( char* const str, const size_t maxSize, const uint64_t value );

size_t dumpInteger( char * const str, const size_t maxSize, const uint8_t value , const Expand expand, const size_t expandSymCount, const Sign sign );
size_t dumpInteger( char * const str, const size_t maxSize, const int8_t value , const Expand expand, const size_t expandSymCount, const Sign sign );
size_t dumpInteger( char * const str, const size_t maxSize, const uint16_t value , const Expand expand, const size_t expandSymCount, const Sign sign );
size_t dumpInteger( char * const str, const size_t maxSize, const int16_t value , const Expand expand, const size_t expandSymCount, const Sign sign );
size_t dumpInteger( char * const str, const size_t maxSize, const uint32_t value , const Expand expand, const size_t expandSymCount, const Sign sign );
size_t dumpInteger( char * const str, const size_t maxSize, const int32_t value , const Expand expand, const size_t expandSymCount, const Sign sign );
size_t dumpInteger( char * const str, const size_t maxSize, const uint64_t value , const Expand expand, const size_t expandSymCount, const Sign sign );
size_t dumpInteger( char * const str, const size_t maxSize, const int64_t value , const Expand expand, const size_t expandSymCount, const Sign sign );

size_t scanInteger(char* const str, const size_t maxSize, int8_t& value);
size_t scanInteger(char* const str, const size_t maxSize, uint8_t& value);
size_t scanInteger(char* const str, const size_t maxSize, int16_t& value);
size_t scanInteger(char* const str, const size_t maxSize, uint16_t& value);
size_t scanInteger(char* const str, const size_t maxSize, int32_t& value);
size_t scanInteger(char* const str, const size_t maxSize, uint32_t& value);
size_t scanInteger(char* const str, const size_t maxSize, int64_t& value);
size_t scanInteger(char* const str, const size_t maxSize, uint64_t& value);

size_t dumpFixedPoint( char * const str, const size_t maxSize, int8_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount );

size_t dumpFixedPoint( char * const str, const size_t maxSize, int16_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount );

size_t dumpFixedPoint( char * const str, const size_t maxSize, int32_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount );

size_t dumpFixedPoint( char * const str, const size_t maxSize, int64_t value,
					   const Expand expand, const size_t expandSymCount,
					   const Sign sign,
					   const size_t fracBitCount, const size_t fracSymCount );


size_t scanFixedPoint( char* const str, const size_t maxSize, uint8_t& value, const size_t fracBitCount );
size_t scanFixedPoint( char* const str, const size_t maxSize, int8_t& value, const size_t fracBitCount );
size_t scanFixedPoint( char* const str, const size_t maxSize, uint16_t& value, const size_t fracBitCount );
size_t scanFixedPoint( char* const str, const size_t maxSize, int16_t& value, const size_t fracBitCount );
size_t scanFixedPoint( char* const str, const size_t maxSize, uint32_t& value, const size_t fracBitCount );
size_t scanFixedPoint( char* const str, const size_t maxSize, int32_t& value, const size_t fracBitCount );
size_t scanFixedPoint( char* const str, const size_t maxSize, uint64_t& value, const size_t fracBitCount );
size_t scanFixedPoint( char* const str, const size_t maxSize, int64_t& value, const size_t fracBitCount );


namespace Format {

	enum class DataClass {
		FixedPoint,
		DecimalPoint,
		FloatingPoint,
		Integer
	};

	enum class FloatingPointValueFormat {
		Float,
		Double
	};

	template<size_t FractionalPartBitCount>
	struct FixedPoint {
		static constexpr const DataClass Class = DataClass::FixedPoint;
		static constexpr const size_t FracBitCount = FractionalPartBitCount;
	};

	template<size_t FractionalZeroCount>
	struct DecimalPoint {
		static constexpr const DataClass Class = DataClass::DecimalPoint;
	};

	template<FloatingPointValueFormat ValueFormat>
	struct FloatingPoint {
		static constexpr const DataClass Class = DataClass::FloatingPoint;
		static constexpr const FloatingPointValueFormat Format = ValueFormat;
	};

	struct Integer {
		static constexpr const DataClass Class = DataClass::Integer;
	};

}

template<typename ValueType, typename DataFormat>
struct Data {
	typedef ValueType 		Type;
	typedef DataFormat 		Format;
};

template< typename Data, typename Data::Type ValueMin, typename Data::Type ValueMax >
struct Limit {
	typedef typename Data::Type Type;
	static constexpr const Type Min = ValueMin;
	static constexpr const Type Max = ValueMax;

	constexpr static inline bool check( Type value ) {
		const bool valueInRange = ( ( Min <= value) && ( value <= Max ) );
		return valueInRange;
	}

};

template< typename Limit, typename... NextLimits >
struct Limits {
	typedef typename Limit::Type Type;

	template<typename ValueType>
	constexpr static inline bool check( ValueType value ) {
		static_assert( std::is_same<ValueType,Type>::value, "Please check value type" );
		const bool res = Limit::check( value );
		if constexpr ( 0 != sizeof...(NextLimits) ) {
			return ( (res) || ( Limits<NextLimits...>::check( value ) ) );
		}
		return res;
	}
};

template<typename Data>
struct Dump {
	typedef typename Data::Type Type;

	static inline size_t dumpHex(  char * const str, const size_t maxSize, Type value ) {
		if ( sizeof(Type) > 4 ) {
			return ::dumpHex( str, maxSize, static_cast<uint64_t>(value) );
		} else {
			return ::dumpHex( str, maxSize, static_cast<uint32_t>(value) );
		}
	}

	static inline size_t dump( char* const str, const size_t maxSize, Type value ) {
		if constexpr ( Format::DataClass::Integer ==  Data::Format::Class ) {
			return dumpInteger( str, maxSize, value, Expand::NotUsed, 0,  Sign::NegativeOnly );
		} else if constexpr ( Format::DataClass::FixedPoint ==  Data::Format::Class ) {
			const size_t fracBitCount = Data::Format::FracBitCount;
			return dumpFixedPoint( str, maxSize, value, Expand::ByZeros, 0, Sign::NegativeOnly, fracBitCount, 4);
		}

		return 0;
	}
};



