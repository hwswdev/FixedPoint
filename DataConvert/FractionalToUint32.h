
#pragma once

#include <stdint.h>
#include <stddef.h>

size_t fractionalToUint32( const char *str,  uint32_t& result );
size_t decimalToUint32( const char *str,  uint32_t& result, size_t maxDecSymCount = 10, size_t minDecSymCount = 0);
size_t hexToUint32( const char *str,  uint32_t& result );
size_t asciiToFpFractional( const char *str,  bool& negative, uint32_t& decimal, uint32_t& fractional );
size_t asciiToFpDecimal( const char *str,  bool& negative, uint32_t& decimal, uint32_t& fractional );
size_t asciiToFixed( const char *str,  const uint8_t binDotPos, uint32_t& value );
size_t asciiToDecimal( const char *str,  const uint8_t decDotPos, uint32_t& value );
