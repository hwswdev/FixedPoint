
#include <math.h>
#include <stdio.h>

typedef unsigned char      uint8_t;
typedef signed char        int8_t;
typedef unsigned short     uint16_t;
typedef signed short       int16_t;
typedef unsigned int       uint32_t;
typedef signed int         int32_t;
typedef unsigned long long uint64_t;
typedef signed long long   int64_t;


uint8_t getMsb( const uint32_t val ) {
	register uint32_t x = val;
	register uint8_t bitCount = 32;
	while( bitCount ) {
		if ( x & 0x80000000 ) break;
		x <<= 1;
		bitCount--;
	}
	return bitCount;
}

double sqrtLinearApprox( const uint32_t val ) {
	const uint8_t msb = getMsb(val) - 1;
	const uint32_t X0 = static_cast<uint32_t>(1) << msb;
	const uint32_t dX = X0;
	const double Y0 = sqrt( static_cast<double>(X0) );
	const double dY = sqrt( static_cast<double>(X0 + dX) ) - Y0;
	const double res = (val - X0) * dY / dX + Y0;
	return res;
}

double sqrtErrorApprox( const uint32_t val ) {
	static const double LinearErr[32] = {
		0.025126265847083,
		0.035533905932737,
		0.050252531694166,
		0.071067811865475,
		0.100505063388334,
		0.142135623730949,
		0.201010126776668,
		0.284271247461899,
		0.402020253553336,
		0.568542494923798,
		0.804040507106672,
		1.1370849898476,
		1.60808101421334,
		2.27416997969519,
		3.21616202842669,
		4.54833995939038,
		6.43232405685337
	};
	static const double div = ( sqrt(2.0) - 1 ) * ( sqrt(2.0) - 1 );
	const uint8_t msb = getMsb( val ) - 1;

	const double xMid = (static_cast<uint64_t>(3) << (msb - 1));
	const double xMax = static_cast<double>( static_cast<uint64_t>(1) << (msb - 2) ) / div;
	const double deltaX = xMid - xMax;
	const double x = val;
	const double a = x - xMid;
	const double b = 1 << (msb - 1);
	const double c = a / b;
	const double d = c * c;
	const double e = 1.0 - d;

	const double xDiff = deltaX * e;
	const double corrX = a + xDiff;
	const double f = corrX / ( static_cast<uint64_t>(1) << (msb - 1) );
	const double g =  f * f;
	const double h = 1.0 - g;
	const double quadErr = LinearErr[msb] * h;

	return quadErr;
}

double sqrtX( const uint32_t val ) {
	const double linear = sqrtLinearApprox( val );
	const double quad = sqrtErrorApprox( val );
	const double res = linear + quad;
	return res;
}


int main (int argc, char argv[]){

	double maxPositiveError = 0.0;
	double maxNegativeError = 0.0;

	for(uint32_t x = 4; x < 65536; x++ ) {
		const double valCalc = sqrtX(x);
		const double valOrig = sqrt(static_cast<double>(x));
		const double err = valCalc - valOrig;

		if ( maxPositiveError < err ) {
			maxPositiveError = err;
		}

		if ( maxNegativeError > err ) {
			maxNegativeError = err;
		}

		printf( "%d, %3.8f, %3.8f, %1.8f\n", x, valCalc, valOrig, err );
	}
	
	printf("Max: %1.8f\n", maxPositiveError );
	printf("Min: %1.8f\n", maxNegativeError );

	return 0;
}
