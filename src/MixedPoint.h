/*
 *  Created on: Mar 5, 2025
 *      Author: Evgeny Sobolev
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <math.h>


template< typename BaseType, typename HelperType = BaseType, size_t DecimalSymAfterDot = 0, size_t FractionalBitCount = 0 >
struct MixedPoint {
	constexpr static const size_t FractionalPartDecimalSymbolCount = DecimalSymAfterDot;
	constexpr static const size_t FractionalPartBinaryBitCount = FractionalBitCount;
	constexpr static const BaseType DecimalMyltiplyer = pow( 10, FractionalPartDecimalSymbolCount );
	constexpr static const BaseType BinaryMultiplyer  = pow( 2, FractionalPartBinaryBitCount );
	constexpr static const BaseType ConversionMultiplyer = DecimalMyltiplyer * BinaryMultiplyer;

	constexpr MixedPoint() : _value( 0 ) {};
	constexpr MixedPoint( int value ) : _value( value * ConversionMultiplyer ) {};
	constexpr MixedPoint( float value )  : _value( value * ConversionMultiplyer ) {};
	constexpr MixedPoint( double value ) : _value( value * ConversionMultiplyer ) {};


	constexpr inline float toFloat() const {
		const float value = static_cast<float>( _value ) / ConversionMultiplyer;
		return value;
	}

	constexpr inline double toDouble() const {
		const double value = static_cast<double>( _value ) / ConversionMultiplyer;
		return value;
	}

	template< typename OtherMixedPoint >
	constexpr MixedPoint( const OtherMixedPoint& other ) {
		_value = convert(other);
	}

	template< typename OtherMixedPoint >
	constexpr inline MixedPoint operator+( const OtherMixedPoint& other ) const {
		MixedPoint result;
		result._value = _value + convert( other );
		return result;
	}

	template< typename OtherMixedPoint >
	constexpr inline MixedPoint operator-( const OtherMixedPoint& other ) const {
		MixedPoint result;
		result._value = _value - convert( other );
		return result;
	}

	template< typename OtherMixedPoint >
	constexpr inline MixedPoint operator*( const OtherMixedPoint& other ) const {
		const HelperType temp = _value;
		MixedPoint result;
		result._value = temp * other.nativeValue() / OtherMixedPoint::ConversionMultiplyer;
		return result;
	}

	template< typename OtherMixedPoint >
	constexpr inline MixedPoint operator/( const OtherMixedPoint& other ) const {
		const HelperType temp = _value;
		MixedPoint result;
		result._value = ( temp * OtherMixedPoint::ConversionMultiplyer ) / other;
		return result;
	}

	template< typename OtherMixedPoint >
	constexpr inline bool operator==( const OtherMixedPoint& other ) const {
		return ( _value == convert( other ).nativeValue() );
	}

	template< typename OtherMixedPoint >
	constexpr inline bool operator>( const OtherMixedPoint& other ) const {
		return ( _value > convert( other ).nativeValue() );
	}

	template< typename OtherMixedPoint >
	constexpr inline bool operator<( const OtherMixedPoint& other ) const {
		return ( _value < convert( other ).nativeValue() );
	}

	template< typename OtherMixedPoint >
	constexpr inline bool operator>=( const OtherMixedPoint& other ) const {
		return ( _value >= convert( other ).nativeValue() );
	}

	template< typename OtherMixedPoint >
	constexpr inline bool operator<=( const OtherMixedPoint& other ) const {
		return ( _value <= convert( other ).nativeValue() );
	}

	constexpr inline MixedPoint operator+( const int value ) const {
		// So, I think compile will shift it if it's power of 2
		const BaseType temp = value * ConversionMultiplyer;
		MixedPoint result;
		result._value = _value + temp;
		return result;
	}

	constexpr inline MixedPoint operator-( const int value ) const {
		// So, I think compile will shift it if it's power of 2
		const BaseType temp = value * ConversionMultiplyer;
		MixedPoint result;
		result._value = _value - temp;
		return result;
	}

	constexpr inline MixedPoint operator*( const int value ) const {
		MixedPoint result;
		result._value = _value * value;
		return result;
	}

	constexpr inline MixedPoint operator/( const int value ) const {
		MixedPoint result;
		result._value = _value / value;
		return result;
	}


	constexpr inline MixedPoint operator+( const double value ) const {
		// So, I think compile will shift it if it's power of 2
		const BaseType temp = value * ConversionMultiplyer;
		MixedPoint result;
		result._value = _value + temp;
		return result;
	}

	constexpr inline MixedPoint operator-( const double value ) const {
		// So, I think compile will shift it if it's power of 2
		const BaseType temp = value * ConversionMultiplyer;
		MixedPoint result;
		result._value = _value - temp;
		return result;
	}

	constexpr inline MixedPoint operator*(  const double value  ) const {
		HelperType temp = _value;
		MixedPoint result;
		temp = temp * ( value * ConversionMultiplyer );
		result._value = temp / ConversionMultiplyer;
		return result;
	}

	constexpr inline MixedPoint operator/(  const double value  ) const {
		const HelperType temp = _value;
		MixedPoint result;
		result._value = ( temp * ConversionMultiplyer) / ( value * ConversionMultiplyer );
		return result;
	}

	constexpr inline BaseType nativeValue() const {
		return _value;
	}

private:

	template< typename OtherMixedPoint >
	constexpr inline BaseType convert( const OtherMixedPoint& other ) const {
		constexpr const size_t OtherDec = OtherMixedPoint::FractionalPartDecimalSymbolCount;
		constexpr const size_t OtherBin = OtherMixedPoint::FractionalPartBinaryBitCount;
		constexpr const size_t ThisDec = FractionalPartDecimalSymbolCount;
		constexpr const size_t ThisBin = FractionalPartBinaryBitCount;
		constexpr const bool TheSameDecScale = ( ThisDec == OtherDec );
		constexpr const bool TheSameBinScale = ( ThisBin == OtherBin );
		constexpr const bool TheSameScale = ( TheSameDecScale && TheSameBinScale );

		if ( TheSameScale ) {
			return  other.nativeValue();
		}

		if ( TheSameDecScale ) {
			if ( ThisBin > OtherBin ) {
				constexpr const size_t ShiftSize = ThisBin - OtherBin;
				const BaseType result =  ( other.nativeValue() ) << ShiftSize;
				return result;
			} else {
				constexpr const size_t ShiftSize = OtherBin - ThisBin;
				// So I wanted to use shift (>>), but what if negative value
				// I think compiler is not so stupid, and will make shift if possible
				const BaseType DivValue = pow( 2, ShiftSize );
				BaseType result = ( other.nativeValue() ) / DivValue;
				return result;
			}
		} else {
			const BaseType ThisMultiplyer  = pow( 10, ThisDec )  * pow(2, ThisBin );
			const BaseType OtherMultiplyer = pow( 10, OtherDec ) * pow(2, OtherBin );
			if ( ThisMultiplyer >= OtherMultiplyer ) {
				const BaseType MulValue = ThisMultiplyer / OtherMultiplyer;
				const BaseType result = other.nativeValue() * MulValue;
				return result;
			} else {
				const BaseType DivValue = OtherMultiplyer / ThisMultiplyer;
				const BaseType result = other.nativeValue() / DivValue;
				return result;
			}
		}

		return 0;
	}

private:
	BaseType _value {};
};


