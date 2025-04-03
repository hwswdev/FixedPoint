
#pragma once

#include <stdint.h>
#include <stddef.h>

size_t hexToUint32( const char *str, size_t strLength,  uint32_t& result );
size_t asciiToFixed( const char *str, const size_t strLength, const uint8_t   binDotPos, uint32_t& value );
size_t asciiToDecimal( const char *str, const size_t strLength,  const uint8_t decDotPos, uint32_t& value );

