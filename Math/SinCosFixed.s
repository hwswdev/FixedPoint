
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

#define COS_APPROX_MODE    1

.syntax unified
.cpu cortex-m3
.thumb

.global sinFixed_0ToPi4
.global cosFixed_0ToPi4
.global toTwoPiScale
.global cosFixed
.global sinFixed
.global cosSignedFixed
.global sinSignedFixed
.global mulAB

.section .text

.type sinFixed_0ToPi4, %function
.align 4
sinFixed_0ToPi4:

  // R0 - { x, x^3, x^5, x^7, x^9, x^11 }
  // R4 - SUM ({ K3, K5, K7, K9, K11} * R0 ^ {3,5,7,9,11} )
  // R5 - ({ K3, K5, K7, K9, K11} * R0 ^ {3,5,7,9,11} )
  // R6 - K3, K5, K7, K9, K11
  // R7 - ptr({K3..K11})
  // R8 - X^2 value
  push { r4 - r9 }

  // R8 <= X ^ 2, R7, - doesn't metters
  umull r7, r8, r0, r0

  // {R5, R4} <= X, ( PI/4 => 0xc90fdaa2 )
  mov r4, r0

  // R7 <= ptr{K3}
  ldr r7, =sin_k3

  // K3
  // R0 <= X ^ 3,  ( (PI/4)^3 => 0x7c066d64 )
  umull r9 ,r0, r0, r8
  ldmia r7!, { r6 }
  umull r9, r5, r0, r6
  sub r4, r4, r5

  // K5
  // R0 <= X ^ 5,  ( (PI/4)^5 => 0x4c814282 )
  umull r9 ,r0, r0, r8
  ldmia r7!, { r6 }
  umull r9, r5, r0, r6
  add r4, r4, r5

  // K7
  // R0 <= X ^ 7,  ( (PI/4)^7 => 0x2f312c42 )
  umull r9 ,r0, r0, r8
  ldmia r7!, { r6 }
  umull r9, r5, r0, r6
  sub r4, r4, r5

  // K9
  // R0 <= X ^ 9,  ( (PI/4)^9 => 0x1d1c465a )
  umull r9 ,r0, r0, r8
  ldmia r7!, { r6 }
  umull r9, r5, r0, r6
  add r4, r4, r5

  // K11
  // R0 <= X ^ 11  ( (PI/4)^11 => 0x11f4f011 )
  umull r9 ,r0, r0, r8
  ldmia r7!, { r6 }
  umull r9, r5, r0, r6
  //  R4 <= ( (P/4) => 0xb504f335 )
  sub r4, r4, r5

  mov r0, r4
  pop { r4 - r9 }
  bx lr

.align 4
sin_k3: // Teylor coefficents
.word  0x2AAAAAAA // ( 1 / 6 )
.word  0x02222222 // ( 1 / 120 )
.word  0x000D00D0 // ( 1 / 5040 )
.word  0x00002E3B // ( 1 / 362880 )
.word  0x0000006B // ( 1 / 39916800 )


.type cosFixed_0ToPi4, %function
.align 4
cosFixed_0ToPi4:

  // R0 - { x, x^2, x^4, x^6, x^8, x^10 }
  // R4 - SUM ( { K2, K4, K6, K8, K10} * R0 ^ {2,4,6,8,10} )
  // R5 - ( { K2, K4, K6, K8, K10} * R0 ^ {2,4,6,8,10} )
  // R6 - K2, K4, K6, K8, K10
  // R7 - ptr({K2..K10})
  // R8 - X^2 value
  push { r4 - r9 }

#if ( COS_APPROX_MODE )
  mov r4, #0x00000000
#else
  mov r4, #0xFFFFFFFF
#endif

  // X1(PI/4)  = 0xC90FDAA2
  // R8 <= X ^ 2,
  umull r7, r8, r0, r0
  mov r0, r8

  // R7 <= ptr{K3}
  ldr r7, =cos_k2

  // K2
  // R0 <= X2(PI/4)  = 0x9DE9E64D
  ldmia r7!, { r6 }
  umull r9, r5, r0, r6
  sub r4, r4, r5

  // K4
  // R0 <= X4(PI/4)  = 0x6168BA2F
  umull r9 ,r0, r0, r8
  ldmia r7!, { r6 }
  umull r9, r5, r0, r6
  add r4, r4, r5

  // K6
  // R0 <= X6(PI/4)  = 0x3C163A21
  umull r9 ,r0, r0, r8
  ldmia r7!, { r6 }
  umull r9, r5, r0, r6
  sub r4, r4, r5

  // K8
  // R0 <= X8(PI/4)  = 0x251087EF
  umull r9 ,r0, r0, r8
  ldmia r7!, { r6 }
  umull r9, r5, r0, r6
  add r4, r4, r5

  // K10
  // R0 <= X10(PI/4) = 0x16DD00C1
  umull r9 ,r0, r0, r8
  ldmia r7!, { r6 }
  umull r9, r5, r0, r6
  sub r4, r4, r5

  // In the case, R4 is initilaize by 0x00000000
  // It is equals to 0x100000000 ( But, it is 0x00000000 )
  // FixUp it to get 0xFFFFFFFF
#if ( COS_APPROX_MODE )
  subs r5, r4, #1
  sbc  r0, r4, #0
#else
  mov r0, r4
#endif

  pop { r4 - r9 }
  bx lr

.align 4
cos_k2: // Teylor coefficents
.word 0x80000000 // ( 1 / 2 )
.word 0x0AAAAAAA // ( 1 / 24 )
.word 0x005B05B0 // ( 1 / 720 )
.word 0x0001A01A // ( 1 / 40320 )
.word 0x0000049F // ( 1 / 3628800 )


.type toTwoPiScale, %function
.align 4
// Scale interval [0..1/8] => [0..PI/4]
toTwoPiScale:
  push {r1}
  ldr r1, =#0xc90fdaa2
  umull r1, r0, r1, r0
  lsl r0, r0, #3
  orr r0, r0, r1, lsr #(32-3)
  pop {r1}
  bx lr



.type sinSignedFixed, %function
.type cosSignedFixed, %function
.type cosFixed, %function
.type sinFixed, %function

.align 4
// Calculate cos value [ 0 .. 1 - (1/1^31) ] of circle
cosSignedFixed:
cosFixed:
  add r0, r0, #0x40000000
  nop
// Calculate sin value [ 0 .. 1 - (1/1^31) ] of circle
sinSignedFixed:
sinFixed:
  push { r1, r2, lr }
  // Get function pointer
  ldr r1, =#sin_case_select
  // Calculate offset
  lsr r2, r0, #(32 - 3)
  ldr r1, [ r1 , r2, lsl #2 ]
  // Update value to 0..PI - 1LSB
  and r0, r0, #0x7FFFFFFF
  bx r1

.align 4
case_sin_000_125:
  bl toTwoPiScale
  bl sinFixed_0ToPi4
  lsr r0, r0, #1
  b end_sin_case

.align 4
case_sin_125_250:
  mov r1, #0x40000000 // (PI/2 - X)
  sub r0, r1, r0
  bl toTwoPiScale
  bl cosFixed_0ToPi4
  lsr r0, r0, #1
  b end_sin_case

.align 4
case_sin_250_375:
  mov r1, #0x40000000 // ( X - PI/2)
  sub r0, r0, r1
  bl toTwoPiScale
  bl cosFixed_0ToPi4
  lsr r0, r0, #1
  b end_sin_case

.align 4
case_sin_375_500:
  mov r1, #0x80000000 // ( PI - X )
  sub r0, r1, r0
  bl toTwoPiScale
  bl sinFixed_0ToPi4
  lsr r0, r0, #1
  b end_sin_case

.align 4
case_sin_500_625:
  bl toTwoPiScale
  bl sinFixed_0ToPi4
  mvn r0, r0, lsr #1
  add r0, r0, #1
  b end_sin_case

.align 4
case_sin_625_750:
  mov r1, #0x40000000 // ( PI/2 - X)
  sub r0, r1, r0
  bl toTwoPiScale
  bl cosFixed_0ToPi4
  mvn r0, r0, lsr #1
  add r0, r0, #1
  b end_sin_case

.align 4
case_sin_750_875:
  mov r1, #0x40000000 // ( X - PI/2 )
  sub r0, r0, r1
  bl toTwoPiScale
  bl cosFixed_0ToPi4
  mvn r0, r0, lsr #1
  add r0, r0, #1
  b end_sin_case

.align 4
case_sin_875_000:
  mov r1, #0x80000000 // ( PI - X )
  sub r0, r1, r0
  bl toTwoPiScale
  bl sinFixed_0ToPi4
  mvn r0, r0, lsr #1
  add r0, r0, #1
  b end_sin_case

.align 4
end_sin_case:
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
sin_case_select:
   .word (case_sin_000_125)
   .word (case_sin_125_250)
   .word (case_sin_250_375)
   .word (case_sin_375_500)
   .word (case_sin_500_625)
   .word (case_sin_625_750)
   .word (case_sin_750_875)
   .word (case_sin_875_000)


.type mulAB, %function
.align 4
mulAB:
	push {r1}
	umull r1, r0, r0, r0
	pop {r1}
	bx lr
