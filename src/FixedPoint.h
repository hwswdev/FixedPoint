/*
 *  Created on: Mar 5, 2025
 *      Author: Evgeny Sobolev
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <math.h>

template<typename BaseType, size_t BinaryFracBitCount, size_t BinaryFracSymCount = 3>
struct FixedPoint {
	typedef BaseType Type;
	static constexpr const size_t BitCount = BinaryFracBitCount;
	static constexpr const BaseType Pow10 = pow(10, BinaryFracSymCount);

	constexpr inline FixedPoint() : _value(0) {}

	constexpr inline FixedPoint( const FixedPoint& value ) {
		_value = value._value;
	}

	constexpr inline FixedPoint( const int value ) {
		_value = value <<  BinaryFracBitCount;
	}

	constexpr inline FixedPoint( const float value ) {
		const Type decimal = value;
		const bool negative = ( decimal < 0 );
		Type frac = ( (negative) ? (decimal - value) : ( value - decimal )) * Pow10;
		frac <<= BinaryFracBitCount;
		frac /= Pow10;
		_value = (decimal << BinaryFracBitCount) + ( (negative) ? (- frac) : (frac) );
	}

	constexpr inline FixedPoint( const double value ) {
		const Type decimal = value;
		const bool negative = ( decimal < 0 );
		Type frac = ( (negative) ? (decimal - value) : ( value - decimal )) * Pow10;
		frac <<= BinaryFracBitCount;
		frac /= Pow10;
		_value = (decimal << BinaryFracBitCount) + ( (negative) ? (- frac) : (frac) );
	}

	constexpr inline FixedPoint operator= ( const float other ) const {
		FixedPoint res;
		_value = FixedPoint(other).value();
		res._value = _value;
		return res;
	}

	constexpr inline FixedPoint operator+ ( const float other ) const {
		FixedPoint res;
		res._value = _value + FixedPoint(other).value();
		return res;
	}

	constexpr inline FixedPoint operator- ( const float other ) const {
		FixedPoint res;
		res._value = _value - FixedPoint(other).value();
		return res;
	}

	constexpr inline FixedPoint operator* ( const float other ) const {
		FixedPoint res;
		Type value = _value * FixedPoint(other).value();
		value >>= BinaryFracBitCount;
		res._value = value;
		return res;
	}

	constexpr inline FixedPoint operator/ ( const float other ) const {
		FixedPoint res;
		res._value = (_value << BinaryFracBitCount) / FixedPoint(other).value();
		return res;
	}

	constexpr inline FixedPoint operator+ ( const double other ) const {
		FixedPoint res;
		res._value = _value + FixedPoint(other).value();
		return res;
	}

	constexpr inline FixedPoint operator- ( const double other ) const {
		FixedPoint res;
		res._value = _value - FixedPoint(other).value();
		return res;
	}

	constexpr inline FixedPoint operator* ( const double other ) const {
		FixedPoint res;
		Type value = _value * FixedPoint(other).value();
		value >>= BinaryFracBitCount;
		res._value = value;
		return res;
	}

	constexpr inline FixedPoint operator/ ( const double other ) const {
		FixedPoint res;
		const Type value = (_value << BinaryFracBitCount) / FixedPoint(other).value();
		res._value = value;
		return res;
	}

	constexpr inline FixedPoint operator+ ( const int other ) const {
		FixedPoint res;
		res._value = _value + (other << BinaryFracBitCount);
		return res;
	}

	constexpr inline FixedPoint operator- ( const int other ) const {
		FixedPoint res;
		res._value = _value - (other << BinaryFracBitCount);
		return res;
	}

	constexpr inline FixedPoint operator* ( const int other ) const {
		FixedPoint res;
		res._value = _value * other;
		return res;
	}

	constexpr inline FixedPoint operator/ ( const int other ) const {
		FixedPoint res;
		res._value = _value / other;
		return res;
	}

	constexpr inline const FixedPoint& operator+= ( const FixedPoint& other ) {
		static_assert( ( BitCount == other.BitCount ), "Please check value types" );
		_value += other._value;
		return *this;
	}

	constexpr inline const FixedPoint& operator-= ( const FixedPoint& other ) {
		static_assert( ( BitCount == other.BitCount ), "Please check value types" );
		_value -= other._value;
		return *this;
	}

	constexpr inline FixedPoint operator+ ( const FixedPoint& other ) const {
		static_assert( ( BitCount == other.BitCount ), "Please check value types" );
		Type value = _value + other._value;
		FixedPoint res;
		res._value = value;
		return res;
	}

	constexpr inline FixedPoint operator- ( const FixedPoint& other ) const {
		static_assert( ( BitCount == other.BitCount ), "Please check value types" );
		const Type value = _value - other._value;
		FixedPoint res;
		res._value = value;
		return res;
	}

	constexpr inline FixedPoint operator* ( const FixedPoint& other ) const {
		static_assert( ( BitCount == other.BitCount ), "Please check value types" );
		Type value = _value * other._value;
		value >>= BinaryFracBitCount;
		FixedPoint res;
		res._value = value;
		return res;
	}

	constexpr inline FixedPoint operator/ ( const FixedPoint& other ) const {
		static_assert( ( BitCount == other.BitCount ), "Please check value types" );
		Type value = (_value << BinaryFracBitCount) / other._value;
		FixedPoint res;
		res._value = value;
		return res;
	}

	constexpr inline bool operator== ( FixedPoint& other ) const {
		return (_value == other._value);
	}

	constexpr inline bool operator> ( FixedPoint& other ) const {
		return (_value > other._value);
	}

	constexpr inline bool operator< ( FixedPoint& other ) const {
		return (_value < other._value);
	}

	constexpr inline bool operator>= ( FixedPoint& other ) const {
		return (_value >= other._value);
	}

	constexpr inline bool operator<= ( FixedPoint& other ) const {
		return (_value <= other._value);
	}

	constexpr inline float toFloat() const {
		const Type BitMask = ((1 << (BinaryFracBitCount - 1)) - 1) | (1 << (BinaryFracBitCount - 1));
		const bool negative = ( _value < 0 );
		const Type positiveValue = (negative) ? -_value : _value;
		const Type decimal = positiveValue >> BinaryFracBitCount;
		const Type Multiplyer = Pow10;

		// Dump fractional part
#if 1
		const Type fracPositiveValue = (positiveValue & BitMask);
		const Type fracDecValue = (fracPositiveValue * Multiplyer) >> BinaryFracBitCount;
#else
		Type fracPositiveValue = (positiveValue & BitMask);
		Type fracDecValue = 0;
		for( size_t index = 0; index < BinaryFracBitCount; index++ ) {
			const bool bitValue = fracPositiveValue & 0x01;
			const size_t divShiftValue = BinaryFracBitCount - index;
			const Type bitWeight = (bitValue) ? ( Multiplyer >> divShiftValue ) : 0;
			fracDecValue += bitWeight;
			fracPositiveValue >>= 1;
		}
#endif

		const float fracValuePowN = fracDecValue;
		const float absValue = fracValuePowN / Multiplyer + decimal;
		const float result = (negative) ? -absValue : absValue;

		return result;
	}

	constexpr inline float toDouble() const {
		const Type BitMask = ((1 << (BinaryFracBitCount - 1)) - 1) | (1 << (BinaryFracBitCount - 1));
		const bool negative = ( _value < 0 );
		const Type positiveValue = (negative) ? -_value : _value;
		const Type decimal = positiveValue >> BinaryFracBitCount;
		const Type Multiplyer = Pow10;
		const Type fracPositiveValue = (positiveValue & BitMask);

		// Dump fractional part
#if 1
		const Type fracDecValue = (fracPositiveValue * Multiplyer) >> BinaryFracBitCount;
#else
		Type fracDecValue = 0;
		for( size_t index = 0; index < BinaryFracBitCount; index++ ) {
			const Type bitValue = ( fracPositiveValue >> index ) & 0x01;
			const size_t divShiftValue = BinaryFracBitCount - index;
			const Type bitWeight = (bitValue) ? ( Multiplyer >> divShiftValue ) : 0;
			fracDecValue += bitWeight;
		}
#endif

		const double fracValuePowN = fracDecValue;
		const double absValue = fracValuePowN / Multiplyer + decimal;
		const double result = (negative) ? -absValue : absValue;
		return result;
	}

	constexpr inline Type value() const {
		return _value;
	}

private:
	Type _value;
};


