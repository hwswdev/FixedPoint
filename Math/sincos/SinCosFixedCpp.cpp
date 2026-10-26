

#include <stdint.h>
#include <stddef.h>
#include "multiply.h"

typedef uint32_t TSinPI4_Result;
typedef uint32_t TAngleFixedArg;


inline uint32_t sinFixed_0ToPI4( uint32_t arg ) {

	constexpr const uint32_t K3 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2*3);
	constexpr const uint32_t K5 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2*3*4*5);
	constexpr const uint32_t K7 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2*3*4*5*6*7);
	constexpr const uint32_t K9 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2*3*4*5*6*7*8*9);
	constexpr const uint32_t K11 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2*3*4*5*6*7*8*9*10*11);
	//constexpr const uint32_t K13 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2*3*4*5*6*7*8*9*10*11*12*13);
	constexpr const uint32_t PI4_Scale = 0xC90FDAA2;

	// Make scaled angle value
	//uint32_t xn = umull_64( arg, PI4_Scale ) >> (32-3);
	uint64_t raw_xn = umull_64( arg, PI4_Scale );
	uint32_t xn = static_cast<uint32_t>((raw_xn + (1ULL << 28)) >> 29);


	// Calc sin(x)
	uint64_t sum = ( static_cast<uint64_t>(xn) ) << 32;
	uint32_t x_quad = umull_32( xn, xn );

	xn = umull_32( xn, x_quad );
	sum = umslal_64( sum, xn, K3 );

	xn = umull_32( xn, x_quad );
	sum = umlal_64( sum, xn, K5 );

	xn = umull_32( xn, x_quad );
	sum = umslal_64( sum, xn, K7 );

	xn = umull_32( xn, x_quad );
	sum = umlal_64( sum, xn, K9 );

	xn = umull_32( xn, x_quad );
	sum = umslal_64( sum, xn, K11 );

	uint32_t sum32 = ( (sum + 0) >> 32 );

	return sum32;

}


inline uint32_t cosFixed_0ToPI4( uint32_t arg ) {

	constexpr const uint32_t K2 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2);
	constexpr const uint32_t K4 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2*3*4);
	constexpr const uint32_t K6 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2*3*4*5*6);
	constexpr const uint32_t K8 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2*3*4*5*6*7*8);
	constexpr const uint32_t K10 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2*3*4*5*6*7*8*9*10);
	constexpr const uint32_t K12 = ( static_cast<uint64_t>(1) << 32 ) / static_cast<uint64_t>(2*3*4*5*6*7*8*9*10*11*12);

	constexpr const uint32_t PI4_Scale = 0xC90FDAA2;

	// Make scaled angle value
	//uint32_t xn = umull_64( arg, PI4_Scale ) >> (32-3);

	uint64_t raw_xn = umull_64( arg, PI4_Scale );
	uint32_t xn = static_cast<uint32_t>((raw_xn + (1ULL << 28)) >> 29);


	// Calc cos(x)
	uint64_t sum = 0xFFFFFFFFFFFFFFFEULL;
	uint32_t x_quad = umull_32( xn, xn );

	sum = umslal_64( sum, x_quad, K2 );

	xn = umull_32( x_quad, x_quad );
	sum = umlal_64( sum, xn, K4 );

	xn = umull_32( xn, x_quad );
	sum = umslal_64( sum, xn, K6 );

	xn = umull_32( xn, x_quad );
	sum = umlal_64( sum, xn, K8 );

	xn = umull_32( xn, x_quad );
	sum = umslal_64( sum, xn, K10 );

	xn = umull_32( xn, x_quad );
	sum = umslal_64( sum, xn, K12 );

	uint32_t sum32 = static_cast<uint32_t>( (sum + 0x80000000) >> 32 );
	sum32 = ( sum32 == 0 ) ? (sum32 - 1) : sum32;

	return sum32;

}



extern "C" int32_t sinFixed( uint32_t arg ) {

	enum class AngleSel : uint32_t {
		O_0_TO_PI4,
		O_PI4_TO_PI2,
		O_PI2_TO_3PI4,
		O_3PI4_TO_PI,
		O_PI_TO_5PI4,
		O_5PI4_TO_3PI2,
		O_3PI2_TO_7PI4,
		O_7PI4_TO_2PI
	} sel = static_cast<AngleSel>( arg >> (32-3) );

	switch( sel ){
		case AngleSel::O_0_TO_PI4:		return ( sinFixed_0ToPI4( arg ) >> 1 ) ;
		case AngleSel::O_PI4_TO_PI2:	return ( cosFixed_0ToPI4( 0x40000000 - arg ) >> 1 );
		case AngleSel::O_PI2_TO_3PI4: 	return ( cosFixed_0ToPI4( arg - 0x40000000 ) >> 1 );
		case AngleSel::O_3PI4_TO_PI:  	return ( sinFixed_0ToPI4( 0x80000000 - arg ) >> 1 );
		case AngleSel::O_PI_TO_5PI4:	return -( sinFixed_0ToPI4( arg - 0x80000000 ) >> 1 );
		case AngleSel::O_5PI4_TO_3PI2:	return -( cosFixed_0ToPI4( 0xC0000000 - arg ) >> 1 );
		case AngleSel::O_3PI2_TO_7PI4:	return -( cosFixed_0ToPI4( arg - 0xC0000000 ) >> 1 );
		case AngleSel::O_7PI4_TO_2PI:	return -( sinFixed_0ToPI4( 0x00000000 - arg ) >> 1 );
		default: return 0;
	}

	return 0;
}

extern "C" int32_t cosFixed( uint32_t arg ) {

	enum class AngleSel : uint32_t {
		O_0_TO_PI4,
		O_PI4_TO_PI2,
		O_PI2_TO_3PI4,
		O_3PI4_TO_PI,
		O_PI_TO_5PI4,
		O_5PI4_TO_3PI2,
		O_3PI2_TO_7PI4,
		O_7PI4_TO_2PI
	} sel = static_cast<AngleSel>( arg >> (32-3) );

	switch( sel ){
		case AngleSel::O_0_TO_PI4: 		return (cosFixed_0ToPI4( arg )  >> 1 );
		case AngleSel::O_PI4_TO_PI2:  	return (sinFixed_0ToPI4( 0x40000000 - arg ) >> 1 );
		case AngleSel::O_PI2_TO_3PI4:	return -(sinFixed_0ToPI4( arg - 0x40000000 ) >> 1 );
		case AngleSel::O_3PI4_TO_PI:	return -(cosFixed_0ToPI4( 0x80000000 - arg ) >> 1 );
		case AngleSel::O_PI_TO_5PI4:	return -(cosFixed_0ToPI4( arg - 0x80000000 ) >> 1 );
		case AngleSel::O_5PI4_TO_3PI2:	return -(sinFixed_0ToPI4( 0xC0000000 - arg ) >> 1 );
		case AngleSel::O_3PI2_TO_7PI4:	return (sinFixed_0ToPI4( arg - 0xC0000000 ) >> 1 );
		case AngleSel::O_7PI4_TO_2PI:	return (cosFixed_0ToPI4( 0x00000000 - arg ) >> 1 );
		default: return 0;
	}

	return 0;
}

extern "C" int32_t rSinFixed( uint32_t arg, uint32_t radius ) {

	enum class AngleSel : uint32_t {
		O_0_TO_PI4,
		O_PI4_TO_PI2,
		O_PI2_TO_3PI4,
		O_3PI4_TO_PI,
		O_PI_TO_5PI4,
		O_5PI4_TO_3PI2,
		O_3PI2_TO_7PI4,
		O_7PI4_TO_2PI
	} sel = static_cast<AngleSel>( arg >> (32-3) );

	switch( sel ){
		case AngleSel::O_0_TO_PI4:		return ( umull_32( sinFixed_0ToPI4( arg ), radius ) >> 2 );
		case AngleSel::O_PI4_TO_PI2:	return ( umull_32( cosFixed_0ToPI4( 0x40000000 - arg ), radius ) >> 2 );
		case AngleSel::O_PI2_TO_3PI4: 	return ( umull_32( cosFixed_0ToPI4( arg - 0x40000000 ), radius ) >> 2 );
		case AngleSel::O_3PI4_TO_PI:  	return ( umull_32( sinFixed_0ToPI4( 0x80000000 - arg ), radius ) >> 2 );
		case AngleSel::O_PI_TO_5PI4:	return -( umull_32( sinFixed_0ToPI4( arg - 0x80000000 ), radius ) >> 2 );
		case AngleSel::O_5PI4_TO_3PI2:	return -( umull_32( cosFixed_0ToPI4( 0xC0000000 - arg ), radius ) >> 2 );
		case AngleSel::O_3PI2_TO_7PI4:	return -( umull_32( cosFixed_0ToPI4( arg - 0xC0000000 ), radius ) >> 2 );
		case AngleSel::O_7PI4_TO_2PI:	return -( umull_32( sinFixed_0ToPI4( 0x00000000 - arg ), radius ) >> 2 );
		default: return 0;
	}

	return 0;
}

extern "C" int32_t rCosFixed( uint32_t arg, uint32_t radius ) {
	enum class AngleSel : uint32_t {
		O_0_TO_PI4,
		O_PI4_TO_PI2,
		O_PI2_TO_3PI4,
		O_3PI4_TO_PI,
		O_PI_TO_5PI4,
		O_5PI4_TO_3PI2,
		O_3PI2_TO_7PI4,
		O_7PI4_TO_2PI
	} sel = static_cast<AngleSel>( arg >> (32-3) );

	switch( sel ){
		case AngleSel::O_0_TO_PI4: 		return ( umull_32( cosFixed_0ToPI4( arg ), radius ) >> 2 );
		case AngleSel::O_PI4_TO_PI2:  	return ( umull_32( sinFixed_0ToPI4( 0x40000000 - arg ),radius ) >> 2 );
		case AngleSel::O_PI2_TO_3PI4:	return -( umull_32( sinFixed_0ToPI4( arg - 0x40000000 ), radius ) >> 2 );
		case AngleSel::O_3PI4_TO_PI:	return -( umull_32( cosFixed_0ToPI4( 0x80000000 - arg ), radius ) >> 2 );
		case AngleSel::O_PI_TO_5PI4:	return -( umull_32( cosFixed_0ToPI4( arg - 0x80000000 ), radius ) >> 2 );
		case AngleSel::O_5PI4_TO_3PI2:	return -( umull_32( sinFixed_0ToPI4( 0xC0000000 - arg ), radius ) >> 2 );
		case AngleSel::O_3PI2_TO_7PI4:	return ( umull_32( sinFixed_0ToPI4( arg - 0xC0000000 ), radius ) >> 2 );
		case AngleSel::O_7PI4_TO_2PI:	return ( umull_32( cosFixed_0ToPI4( 0x00000000 - arg ), radius ) >> 2 );
		default: return 0;
	}
}


