/*
 * DecimalPoint.h
 *
 *  Created on: Mar 6, 2025
 *      Author: Evgeny
 */

#pragma once

template<typename BaseType, size_t DecimalFracSymCount >
struct DecimalPoint {
	typedef BaseType Type;
	static constexpr const BaseType Pow10 = pow( 10, DecimalFracSymCount );

	constexpr inline DecimalPoint() : _value(0) {};

	constexpr inline DecimalPoint( const int value ) : _value( value * Pow10 ) {};
	constexpr inline DecimalPoint( const float  value ) : _value( static_cast<BaseType>(value * Pow10) ) {};
	constexpr inline DecimalPoint( const double value ) : _value( static_cast<BaseType>(value * Pow10) ) {};

	constexpr inline float toFloat() const {
		float value = _value;
		value /= Pow10;
		return value;
	};

	constexpr inline double toDouble() const {
		double value = _value;
		value /= Pow10;
		return value;
	};

	constexpr inline int toInteger() const {
		return _value;
	};

	/**********/
	constexpr inline DecimalPoint operator+ ( const double value ) const {
		DecimalPoint res;
		res._value = _value + DecimalPoint(value);
		return res;
	}

	constexpr inline DecimalPoint operator- ( const double value ) const {
		DecimalPoint res;
		res._value = _value - DecimalPoint(value);
		return res;
	}

	constexpr inline DecimalPoint operator* ( const double value ) const {
		DecimalPoint res;
		res._value = _value * DecimalPoint(value) / Pow10;
		return res;
	}

	constexpr inline DecimalPoint operator/ ( const double value ) const {
		DecimalPoint res;
		res._value = _value * Pow10 / DecimalPoint(value);
		return res;
	}

	/**********/
	constexpr inline DecimalPoint operator+ ( const float value ) const {
		DecimalPoint res;
		res._value = _value + DecimalPoint(value);
		return res;
	}

	constexpr inline DecimalPoint operator- ( const float value ) const {
		DecimalPoint res;
		res._value = _value - DecimalPoint(value);
		return res;
	}

	constexpr inline DecimalPoint operator* ( const float value ) const {
		DecimalPoint res;
		res._value = _value * DecimalPoint(value) / Pow10;
		return res;
	}

	constexpr inline DecimalPoint operator/ ( const float value ) const {
		DecimalPoint res;
		res._value = _value * Pow10 / DecimalPoint(value);
		return res;
	}

	/**********/
	constexpr inline DecimalPoint operator+ ( const int value ) const {
		DecimalPoint res;
		res._value = _value + value;
		return res;
	}

	constexpr inline DecimalPoint operator- ( const int value ) const {
		DecimalPoint res;
		res._value = _value - value;
		return res;
	}

	constexpr inline DecimalPoint operator* ( const int value ) const {
		DecimalPoint res;
		res._value = _value * value;
		return res;
	}

	constexpr inline DecimalPoint operator/ ( const int value ) const {
		DecimalPoint res;
		res._value = _value / value;
		return res;
	}

	/**********/
	constexpr inline DecimalPoint operator+ ( const DecimalPoint& other ) const {
		DecimalPoint res;
		res._value = _value + other._value;
		return res;
	}

	constexpr inline DecimalPoint operator- ( const DecimalPoint& other ) const {
		DecimalPoint res;
		res._value = _value - other._value;
		return res;
	}

	constexpr inline DecimalPoint operator* ( const DecimalPoint& other ) const {
		DecimalPoint res;
		res._value = _value * other._value / Pow10;
		return res;
	}

	constexpr inline DecimalPoint operator/ ( const DecimalPoint& other ) const {
		DecimalPoint res;
		res._value = _value * Pow10 / other._value;
		return res;
	}

	/**********/
	constexpr inline DecimalPoint operator+= ( const DecimalPoint& other ) {
		_value = _value + other._value;
		return *this;
	}

	constexpr inline DecimalPoint operator-= ( const DecimalPoint& other ) {
		_value = _value - other._value;
		return *this;
	}

	constexpr inline DecimalPoint operator*= ( const DecimalPoint& other ) {
		_value = _value * other._value / Pow10;
		return *this;
	}

	constexpr inline const DecimalPoint& operator/= ( const DecimalPoint& other ) {
		_value = _value * Pow10 / other._value;
		return *this;
	}

	/**********/
	constexpr inline const bool operator== ( const DecimalPoint& other ) const {
		return (_value == other._value);
	}

	constexpr inline const bool operator> ( const DecimalPoint& other ) const {
		return (_value > other._value);
	}

	constexpr inline const bool operator< ( const DecimalPoint& other ) const {
		return (_value < other._value);
	}

	constexpr inline const bool operator>= ( const DecimalPoint& other ) const {
		return (_value >= other._value);
	}

	constexpr inline const bool operator<= ( const DecimalPoint& other ) const {
		return (_value <= other._value);
	}

private:
	BaseType _value;
};
