/*
  ********************************************************************************************
  * @file      SinCosFixed.s dedicated to STM32Fxx device
  * @author    Evgeny Sobolev
  * @version   V1.0.0
  * @date      2025-03-10
  *
  * @description  SIN & COS fixed point math in <= [  0 .. 1 ],  out <= [ -1 .. 1 ]
  * @description  SIN & COS fixed point math in <= [ -1 .. 1 ],  out <= [ -1 .. 1 ]
  *
  * @description  Function int32_t sinFixed( uint32_t x )
  * @description  Function int32_t cosFixed( uint32_t x )
  *
  * @description  0.0           =>  0x00000000,
  * @description  0.25          =>  0x40000000,
  * @description  0.5           =>  0x80000000,
  * @description  0.75          =>  0xC0000000,
  * @description  0.999999999xx =>  0xFFFFFFFF
  * @description  valid  x => [ 0x00000000 .. 0xFFFFFFFF ]
  *
  * @description  Return: int32_t sin( x * 2 * pi / (2^32) ) * ( 1 / 2^31 )
  * @description  Return: int32_t cos( x * 2 * pi / (2^32) ) * ( 1 / 2^31 )
  * @description  Result scaled as
  *
  * @description -1.0           =>  0x80000000,
  * @description -0.75          =>  0xA0000000,
  * @description -0.5           =>  0xC0000000,
  * @description -0.25          =>  0xE0000000,
  * @description -0.000000000xx =>  0xFFFFFFFF,
  * @description  0.0           =>  0x00000000,
  * @description  0.25          =>  0x20000000,
  * @description  0.5           =>  0x40000000,
  * @description  0.75          =>  0x60000000,
  * @description  0.999999999xx =>  0x7FFFFFFF
  *
  *******************************************************************************************
 */


// So. Because of Taylor approximation, vaule of sin(x) is always
// less then real value of sin(x). It is about one bit error
// But, cos value can't be greater then 0.9999999(9)
// So, in fact I used (0xFFFFFFFF - SomeApproxValue), or (0x100000000 - SomeApproxValue) on FixUp
// In this case cos value is always less then 1.0, i.e. max 0.999999(9)

.syntax unified
.cpu cortex-m3
.thumb

.global sinFixed_0ToPi4
.global cosFixed_0ToPi4
.global cosFixed
.global sinFixed
.global cosSignedFixed
.global sinSignedFixed

.section .text

/*

0x2AAAAAAA // (1/6)
0x02222222 // (1/120)
0x000D00D0 // (1/5040)
0x00002E3B // (1/362880)
0x0000006B // (1/39916800)
*/


.type sinFixed_0ToPi4, %function
.type sinFixed_0ToPi4_Scaled, %function

.align 4
sinFixed_0ToPi4_Scaled:
  	ldr r1, =#0xc90fdaa2 	// PI/4 value
  	umull r1, r0, r1, r0
  	lsl r0, r0, #3
  	orr r0, r0, r1, lsr #(32-3)
sinFixed_0ToPi4:
	// R0 <= X
	push {r1-r4}

	// 0xC90FDAA2 <= (PI/4) ^ 1

	umull r1, r3, r0, r0   // R3 <= R0 * R0, i.e X^2
	umull r1, r4, r3, r0   // R4 <= X^2 * X, i.e X^3

	// 0x9DE9E64D <= (PI/4) ^ 2
	// 0x7C066D64 <= (PI/4) ^ 3
	ldr r1, =#0x2AAAAAAA   // R1 <= 1/6
	umull r1, r2, r1, r4   // R2 <= R1 * R4, i.e 1/6 * X^3
	sub r0, r0, r2		   // R0 <= X - 1/6 * X^3

	// 0x4C814282 <= (PI/4) ^ 5
	umull r1, r4, r4, r3   // R4 <= X^5
	ldr r1, =#0x02222222   // R1 <= 1/120
	umull r1, r2, r1, r4   // R2 <= 1/120 * X^5
	add r0, r0, r2         // R0 <= X - 1/6 * X^3 + 1/120 * X^5

	// 0x2F312C42 <= (PI/4) ^ 7
	umull r1, r4, r4, r3   // R4 <= X^7
	ldr r1, =#0x000D00D0   // R1 <= 1/5040
	umull r1, r2, r1, r4   // R2 <= 1/5040 * X^7
	sub r0, r0, r2         // R0 <= X - 1/6 * X^3  + 1/120 * X^5 - 1/5040 * X^7

	// 0x1D1C465A <= (PI/4) ^ 9
	umull r1, r4, r4, r3   // R4 <= X^9
	ldr r1, =#0x00002E3B   // R1 <= 1/362880
	umull r1, r2, r1, r4   // R2 <= 1/362880 * X^9
	add r0, r0, r2         // R0 <= X - 1/6 * X^3  + 1/120 * X^5 - 1/5040 * X^7 + 1/362880 * X^9

	// 0x11F4F011 <= (PI/4) ^ 11
	umull r1, r4, r4, r3   // R4 <= X^11
	ldr r1, =#0x0000006B   // R1 <= 1/39916800
	umull r1, r2, r1, r4   // R2 <= 1/39916800 * X^11
	sub r0, r0, r2         // R0 <=X - 1/6 * X^3  + 1/120 * X^5 - 1/5040 * X^7 + 1/362880 * X^9 - 1/39916800 * X^11

	// 0.70710678142495453 <= 0xb504f335 <= SIN(PI/4)
	pop {r1-r4}
	bx lr



/*
0x80000000 // (1/2)
0x0AAAAAAA // (1/24)
0x000D00D0 // (1/720)
0x0001A01A // (1/40320)
0x0000049F // (1/3628800)
*/

.type cosFixed_0ToPi4_Scaled, %function
.type cosFixed_0ToPi4, %function

.align 4
cosFixed_0ToPi4_Scaled:
  	ldr r1, =#0xc90fdaa2 	// PI/4 value
  	umull r1, r0, r1, r0
  	lsl r0, r0, #3
  	orr r0, r0, r1, lsr #(32-3)
cosFixed_0ToPi4:
	push {r1-r4}

	// 0xC90FDAA2 <= (PI/4) ^ 1
	umull r1, r3, r0, r0   // R3 <= R0 * R0, i.e X^2
	mov r4, r3             // R4 <= X^2

	//mov r0, #0xFFFFFFFF    // R0 <= '0.999(9)'
	mov r0, #0x00000000    // R0 <= '1.0'

	// 0x9DE9E64D <= (PI/4) ^ 2
	lsr r2, r4, #1		   // R2 <= 1/2 * X^2
	sub r0, r0, r2         // R0 <=  1 - 1/2 * X^2

	// 0x6168BA2F <= (PI/4) ^ 4
	umull r1, r4, r4, r3   // R4 <= X^4
	ldr r1, =#0x0AAAAAAA   // ( 1 / 24 )
	umull r1, r2, r1, r4   // R2 <= 1/24 * X^4
	add r0, r0, r2         // R0 <=  1 - 1/2 * X^2 + 1/24*X^4

	// 0x3C163A21 <= (PI/4) ^ 6
	umull r1, r4, r4, r3   // R4 <= X^6
	ldr r1, =#0x005B05B0   // ( 1 / 720 )
	umull r1, r2, r1, r4   // R2 <= 1/720 * X^6
	sub r0, r0, r2         // R0 <= 1 - 1/2 * X^2 + 1/24*X^4 - 1/720 * X^6

	// 0x251087EF <= (PI/4) ^ 8
	umull r1, r4, r4, r3   // R4 <= X^8
	ldr r1, =#0x0001A01A   // ( 1 / 40320 )
	umull r1, r2, r1, r4   // R2 <= 1/40320 * X^8
	add r0, r0, r2         // R0 <= 1 - 1/2 * X^2 + 1/24*X^4 - 1/720 * X^6 + 1/40320 * X^8

	// 0x16DD00C1 <= (PI/4) ^ 10
	umull r1, r4, r4, r3   // R4 <= X^10
	ldr r1, =#0x0000049F   // ( 1 / 3628800 )
	umull r1, r2, r1, r4   // R2 <= 1/3628800 * X^10
	sub r0, r0, r2         // R0 <= 1 - 1/2 * X^2 + 1/24*X^4 - 1/720 * X^6 + 1/40320 * X^8 - 1/39916800 * X^10

	// 0x0E1A6D2D <= (PI/4) ^ 12
	umull r1, r4, r4, r3   // R4 <= X^12
	ldr r1, =#0x00000008   // ( 1 / 479001600 )
	umull r1, r2, r1, r4   // R2 <= 1/479001600 * X^12
	add r0, r0, r2         // R0 <= 1 - 1/2 * X^2 + 1/24*X^4 - 1/720 * X^6 + 1/40320 * X^8 - 1/39916800 * X^10 + 1/479001600 * X^12

	// COS value correction, because of 0xFFFFFFFF is maximum
	subs r1, r0, #1
	sbc  r0, r0, #0

	// 0.70710678119212389 <= 0xb504f334 <= COS(PI/4)
	pop {r1-r4}
	bx lr

.global toTwoPiScale
.type toTwoPiScale, %function
.align 4
// Scale interval [0..1/8] => [0..PI/4]
// 0x20000000 => 0xc90fdaa2
toTwoPiScale:
  push {r1}
  ldr r1, =#0x6487ed51
  umull r1, r0, r1, r0
  lsl r0, r0, #4
  orr r0, r0, r1, lsr #(32-4)
  pop {r1}
  bx lr


.type sinSignedFixed, %function
.type cosSignedFixed, %function
.type cosFixed, %function
.type sinFixed, %function

.align 4

cosSignedFixed:
cosFixed:
  add r0, r0, #0x40000000
  nop
// Calculate sin value [ 0 .. 1 - (1/1^31) ] of circle
sinSignedFixed:
sinFixed:
  push { r1, r2, lr }
  lsr r2, r0, #(32 - 3)
  ldr r1, =#sinFixedTBBTable
  tbb [ r1, r2 ]

case_sin_000_125:
  bl sinFixed_0ToPi4_Scaled
  lsr r0, r0, #1
  pop  { r1, r2, lr }
  bx lr

case_sin_125_250:
  mov r1, #0x40000000 // (PI/2 - X)
  sub r0, r1, r0
  bl cosFixed_0ToPi4_Scaled
  lsr r0, r0, #1
  pop  { r1, r2, lr }
  bx lr

case_sin_250_375:
  mov r1, #0x40000000 // ( X - PI/2)
  sub r0, r0, r1
  bl cosFixed_0ToPi4_Scaled
  lsr r0, r0, #1
  pop  { r1, r2, lr }
  bx lr

case_sin_375_500:
  mov r1, #0x80000000 // ( PI - X )
  sub r0, r1, r0
  bl sinFixed_0ToPi4_Scaled
  lsr r0, r0, #1
  pop  { r1, r2, lr }
  bx lr

case_sin_500_625:
  mov r1, #0x80000000 // ( PI - X )
  sub r0, r0, r1
  bl sinFixed_0ToPi4_Scaled
  lsr r0, r0, #1
  negs r0, r0
  pop  { r1, r2, lr }
  bx lr

case_sin_625_750:
  mov r1, #0xC0000000 // ( 3*PI/4 - X)
  sub r0, r1, r0
  bl cosFixed_0ToPi4_Scaled
  lsr r0, r0, #1
  negs r0, r0
  pop  { r1, r2, lr }
  bx lr

case_sin_750_875:
  mov r1, #0xC0000000 // ( X - 3*PI/4 )
  sub r0, r0, r1
  bl cosFixed_0ToPi4_Scaled
  lsr r0, r0, #1
  negs r0, r0
  pop  { r1, r2, lr }
  bx lr

case_sin_875_000:
  mov r1, #0x00000000 // ( 2*PI - X )
  sub r0, r1, r0
  bl sinFixed_0ToPi4_Scaled
  lsr r0, r0, #1
  negs r0, r0
  pop  { r1, r2, lr }
  bx lr

.type case_sin_000_125, %function
.type case_sin_125_250, %function
.type case_sin_250_375, %function
.type case_sin_375_500, %function
.type case_sin_500_625, %function
.type case_sin_625_750, %function
.type case_sin_750_875, %function
.type case_sin_875_000, %function

.align 4
sinFixedTBBTable:
.byte ((case_sin_000_125 - case_sin_000_125 ) / 2)
.byte ((case_sin_125_250 - case_sin_000_125 ) / 2)
.byte ((case_sin_250_375 - case_sin_000_125 ) / 2)
.byte ((case_sin_375_500 - case_sin_000_125 ) / 2)
.byte ((case_sin_500_625 - case_sin_000_125 ) / 2)
.byte ((case_sin_625_750 - case_sin_000_125 ) / 2)
.byte ((case_sin_750_875 - case_sin_000_125 ) / 2)
.byte ((case_sin_875_000 - case_sin_000_125 ) / 2)


