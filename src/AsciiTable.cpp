/*
 * AsciiSymbol.cpp
 *
 *  Created on: Feb 24, 2025
 *      Author: Evgeny
 */

#include <stddef.h>
#include <stdint.h>
#include "AsciiTable.h"

static constexpr const size_t AsciiTableSymbolCount = 256;
static constexpr const size_t HexSymbolsCount = 16;


enum class Hex : bool {
	False,
	True
};

enum class Decimal : bool {
	False,
	True
};

enum class Dot : bool {
	False,
	True
};

enum class Sign : bool {
	False,
	True
};

enum class Text : bool {
	False,
	True
};


enum class NmeaSymbol : uint8_t {
	Invalid,
	Regular,
	Special
};

struct SymbolFlags {

	constexpr SymbolFlags(
			const char 			asciiCode,
			const char 			description[32],
			const Hex 			hex,
			const Decimal 		decimal,
			const Sign 			sign,
			const Text 			text,
			const Dot			dot,
			const NmeaSymbol 	nmea,
			const uint8_t hexValueOfNibble ) :
					_hex(hex),
					_decimal(decimal),
					_sign(sign),
					_text(text),
					_dot(dot),
					_nmeaSymbol(nmea),
					_hexVauleOfNibble(hexValueOfNibble) {};

	inline bool getHex( uint8_t& hexValue ) {
		hexValue = _hexVauleOfNibble;
		return ( Hex::True == _hex );;
	}

	inline bool getDecimal( uint8_t& decValue ) {
		decValue = _hexVauleOfNibble;
		return ( Decimal::True == _decimal );
	}

	inline bool isNmeaValidSymbol() {
		return ( NmeaSymbol::Invalid != _nmeaSymbol );
	}

	inline bool isNmeaSpecialSymbol() {
		return ( NmeaSymbol::Special == _nmeaSymbol );
	}

private:
	Hex 			_hex 			: 1;
	Decimal			_decimal 		: 1;
	Sign			_sign 			: 1;
	Text			_text 			: 1;
	Dot				_dot 			: 1;
	NmeaSymbol		_nmeaSymbol 	: 2;
	uint8_t			_hexVauleOfNibble;
};



// Hex to symbol fast transformation table
static const char HexToAsiiTable[HexSymbolsCount] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };


// So, I tried to make self-described code, and I got this
// There are to cause I used symbol flags CONSTEXPR constructor
// 1. I had files to describe code, but I don't want to get space into flash memory or sram
// 2. I want to describe each flag, and made minimum count of mistakes
// 3. I used table because it is fastest way to get flags and value and conveyor
static SymbolFlags Symbols[AsciiTableSymbolCount] = {

	SymbolFlags( 0x00, "NULL (null)",                   Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x01, "SOH  (start of heading)",       Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x02, "STX  (start of text)",          Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x03, "ETX  ( end of text)",           Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x04, "EOT  ( end of transmission)",   Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x05, "ENQ  ( enquiry )",              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x06, "ACK  ( acknowledge )",          Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x07, "BEL  ( bell )",                 Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x08, "BS   ( backspace )",            Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x09, "TAB  ( horizontal tab )",       Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x0A, "LF   ( new line )",             Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Special, 0 ),
	SymbolFlags( 0x0B, "VT   ( vertical tab )",         Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x0C, "FF   ( new page )",             Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x0D, "CR   ( carriage return )",      Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Special, 0 ),
	SymbolFlags( 0x0E, "SO   ( shift out )",            Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x0F, "SI   ( shift in )",             Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0x10, "DLE  ( data link escape )",     Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x11, "DC1  ( device control 1 )",     Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x12, "DC2  ( device control 2 )",     Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x13, "DC3  ( device control 3 )",     Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x14, "DC4  ( device control 4 )",     Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x15, "NAK  ( negative acknowledge )", Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x16, "SYN  ( synchronous idle )",     Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x17, "ETB  ( end of trans. block )",  Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x18, "CAN  ( cancel )",               Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x19, "EM   ( end of medium )",        Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x1A, "SUB  ( substitute )",           Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x1B, "ESC  ( escape )",               Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x1C, "FS   ( file separator )",       Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x1D, "GS   ( group separator )",      Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x1E, "RS   ( record separator )",     Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x1E, "US   ( unit separator )",       Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0x20, "' '  ( space )",                Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x21, "'!'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Special, 0 ),
	SymbolFlags( 0x22, "'\"'",                          Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x23, "'#'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x24, "'$'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Special, 0 ),
	SymbolFlags( 0x25, "'%'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x26, "'&'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x27, "'\''",                          Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x28, "'('",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x29, "')'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x2A, "'*'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Special, 0 ),
	SymbolFlags( 0x2B, "'+'",                           Hex::False, Decimal::False,  Sign::True,  Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x2C, "','",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Special, 0 ),
	SymbolFlags( 0x2D, "'-'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x2E, "'.'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::True,  NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x2F, "'/'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0x30, "'0'",                           Hex::True,  Decimal::True,   Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x31, "'1'",                           Hex::True,  Decimal::True,   Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 1 ),
	SymbolFlags( 0x32, "'2'",                           Hex::True,  Decimal::True,   Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 2 ),
	SymbolFlags( 0x33, "'3'",                           Hex::True,  Decimal::True,   Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 3 ),
	SymbolFlags( 0x34, "'4'",                           Hex::True,  Decimal::True,   Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 4 ),
	SymbolFlags( 0x35, "'5'",                           Hex::True,  Decimal::True,   Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 5 ),
	SymbolFlags( 0x36, "'6'",                           Hex::True,  Decimal::True,   Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 6 ),
	SymbolFlags( 0x37, "'7'",                           Hex::True,  Decimal::True,   Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 7 ),
	SymbolFlags( 0x38, "'8'",                           Hex::True,  Decimal::True,   Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 8 ),
	SymbolFlags( 0x39, "'9'",                           Hex::True,  Decimal::True,   Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 9 ),
	SymbolFlags( 0x3A, "':'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x3B, "';'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x3C, "'<'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x3D, "'='",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x3E, "'>'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x3F, "'?'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0x40, "'@'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x41, "'A'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0A ),
	SymbolFlags( 0x42, "'B'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0B ),
	SymbolFlags( 0x43, "'C'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0C ),
	SymbolFlags( 0x44, "'D'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0D ),
	SymbolFlags( 0x45, "'E'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0E ),
	SymbolFlags( 0x46, "'F'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0F ),
	SymbolFlags( 0x47, "'G'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x48, "'H'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x49, "'I'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x4A, "'J'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x4B, "'K'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x4C, "'L'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x4D, "'M'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x4E, "'N'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x4F, "'O'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),

	SymbolFlags( 0x50, "'P'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x51, "'Q'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x52, "'R'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x53, "'S'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x54, "'T'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x55, "'U'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x56, "'V'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x57, "'W'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x58, "'X'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x59, "'Y'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x5A, "'Z'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x5B, "'['",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x5C, "'\\'",                          Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x5D, "']'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x5E, "'^'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x5F, "'_'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0x60, "",    	                        Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x61, "'a'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0A ),
	SymbolFlags( 0x62, "'b'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0B ),
	SymbolFlags( 0x63, "'c'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0C ),
	SymbolFlags( 0x64, "'d'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0D ),
	SymbolFlags( 0x65, "'e'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0E ),
	SymbolFlags( 0x66, "'f'",                           Hex::True,  Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0x0F ),
	SymbolFlags( 0x67, "'g'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x68, "'h'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x69, "'i'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x6A, "'j'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x6B, "'k'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x6C, "'l'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x6D, "'m'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x6E, "'n'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x6F, "'o'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),

	SymbolFlags( 0x70, "'p'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x71, "'q'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x72, "'r'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x73, "'s'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x74, "'t'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x75, "'u'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x76, "'v'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x77, "'w'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x78, "'x'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x79, "'y'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x7A, "'z'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Regular, 0 ),
	SymbolFlags( 0x7B, "'{'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x7C, "'|'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x7D, "'}'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x7E, "'~'",                           Hex::False, Decimal::False,  Sign::False, Text::True,  Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x7F, "DEL (delete)",                  Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0x80, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x81, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x82, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x83, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x84, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x85, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x86, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x87, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x88, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x89, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x8A, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x8B, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x8C, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x8D, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x8E, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x8F, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0x90, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x91, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x92, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x93, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x94, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x95, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x96, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x97, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x98, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x99, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x9A, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x9B, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x9C, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x9D, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x9E, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0x9F, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0xA0, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xA1, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xA2, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xA3, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xA4, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xA5, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xA6, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xA7, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xA8, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xA9, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xAA, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xAB, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xAC, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xAD, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xAE, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xAF, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0xB0, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xB1, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xB2, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xB3, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xB4, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xB5, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xB6, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xB7, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xB8, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xB9, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xBA, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xBB, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xBC, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xBD, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xBE, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xBF, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0xC0, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xC1, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xC2, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xC3, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xC4, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xC5, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xC6, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xC7, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xC8, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xC9, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xCA, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xCB, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xCC, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xCD, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xCE, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xCF, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0xD0, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xD1, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xD2, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xD3, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xD4, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xD5, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xD6, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xD7, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xD8, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xD9, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xDA, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xDB, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xDC, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xDD, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xDE, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xDF, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0xE0, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xE1, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xE2, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xE3, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xE4, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xE5, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xE6, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xE7, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xE8, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xE9, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xEA, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xEB, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xEC, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xED, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xEE, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xEF, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),

	SymbolFlags( 0xF0, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xF1, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xF2, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xF3, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xF4, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xF5, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xF6, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xF7, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xF8, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xF9, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xFA, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xFB, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xFC, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xFD, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xFE, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),
	SymbolFlags( 0xFF, "",                              Hex::False, Decimal::False,  Sign::False, Text::False, Dot::False, NmeaSymbol::Invalid, 0 ),

};


bool asciiToNibble( const char hexSymbol, uint8_t& hexValue) {
	return Symbols[ static_cast<uint8_t>( hexSymbol ) ].getHex( hexValue );
}

bool nibbleToAscii( const uint8_t hexValue, char& hexSymbol ) {
	const bool isHexSymbol =  !( hexValue & 0xF0 );
	hexSymbol = (isHexSymbol) ? (HexToAsiiTable[ hexValue ]) : '\0';
	return isHexSymbol;
}

bool asciiToDecimal( const char decSymbol, uint8_t& decValue) {
	return Symbols[ static_cast<uint8_t>( decSymbol ) ].getDecimal( decValue );
}


bool isNmeaSymbol(const char symbol ) {
	return Symbols[ static_cast<uint8_t>(symbol) ].isNmeaValidSymbol();
}

bool isNmeaSpecialSymbol( const char symbol ) {
	return Symbols[ static_cast<uint8_t>(symbol) ].isNmeaSpecialSymbol();
}

