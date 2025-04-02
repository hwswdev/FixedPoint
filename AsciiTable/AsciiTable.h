/*
 * AsciiTable.h
 *
 *  Created on: Feb 25, 2025
 *      Author: Evgeny
 */

#pragma once

#include <stdint.h>

bool asciiToNibble( const char hexSymbol, uint8_t& hexValue);
bool nibbleToAscii( const uint8_t hexValue, char& hexSymbol );
bool asciiToDecimal( const char decSymbol, uint8_t& decValue);
