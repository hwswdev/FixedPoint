
/*
  ********************************************************************************************
  * @file      sincos_pi4.s dedicated to STM32Fxx device
  * @author    Evgeny Sobolev
  * @version   V1.0.0
  * @date      2025-03-10
  *
  * @description  SIN & COS fixed point math in <= [  0 .. 1 ],  out <= [-1..1]
  * @description  SIN & COS fixed point math in <= [ -1 .. 1 ],  out <= [-1..1]
  *
  * @description  Function int32_t sinFixed( uint32_t x )
  * @description  Function int32_t cosFixed( uint32_t x )
  *
  * @description  0.0          =>  0x00000000,
  * @description  0.25         =>  0x40000000,
  * @description  0.5          =>  0x80000000,
  * @description  0.75         =>  0xC0000000,
  * @description  0.999999999  =>  0xFFFFFFFF,
  * @description  valid  x => [ 0x00000000 .. 0xFFFFFFFF ]
  *
  * @description  Return: sin( x * 2 * pi / (2^32) ) * (1 / 2^31)
  * @description  Result scaled as
  * @description -0.999999999  =>  0xFFFFFFFF,
  * @description -0.25         =>  0xE0000000,
  * @description -0.5          =>  0xC0000000,
  * @description -0.75         =>  0x40000000
  * @description  0.0          =>  0x00000000,
  * @description  0.25         =>  0x20000000,
  * @description  0.5          =>  0x40000000,
  * @description  0.75         =>  0x60000000,
  * @description  0.999999999  =>  0x7FFFFFFF,
  *
  *******************************************************************************************
 */

.syntax unified
.cpu cortex-m3


  .thumb
//  .global sinFixed_0ToPi4
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
  //  R4 <= ( (P/4) => 00xb504f335 )
  sub r4, r4, r5

  mov r0, r4
  pop { r4 - r9 }
  bx lr

.align 4
sin_k3:
.word  0x2AAAAAAA // ( 1 / 6 )
.word  0x02222222 // ( 1 / 120 )
.word  0x000D00D0 // ( 1 / 5040 )
.word  0x00002E3B // ( 1 / 362880 )
.word  0x0000006B // ( 1 / 39916800 )


//  .global cosFixed_0ToPi4
  .section .text
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

  mov r4, #0xFFFFFFFF

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

  mov r0, r4
  pop { r4 - r9 }
  bx lr

.align 4
cos_k2:
.word 0x80000000 // ( 1 / 2 )
.word 0x0AAAAAAA // ( 1 / 24 )
.word 0x005B05B0 // ( 1 / 720 )
.word 0x0001A01A // ( 1 / 40320 )
.word 0x0000049F // ( 1 / 3628800 )


   .thumb
  .section .text
  .type toTwoPiScale, %function
  .align 4
toTwoPiScale:
  push {r1}
  ldr r1, =#0xc90fdaa2
  umull r1, r0, r1, r0
  lsl r0, r0, #3
  orr r0, r0, r1, lsr #(32-3)
  pop {r1}
  bx lr


// Calculate sinus value [0..1/8] of circle
   .thumb
  .section .text
  .type sin_000_125, %function
  .align 4
sin_000_125:
 	push {lr}
	bl toTwoPiScale
  	bl sinFixed_0ToPi4
  	lsr r0, r0, #1
  	pop {lr}
	bx lr


// Calculate sinus value [1/8..2/8] of circle
   .thumb
  .section .text
  .type sin_125_250, %function
  .align 4
sin_125_250:
 	push {r1, lr}
 	mov r1, #0x40000000 // (PI/2 - X)
 	sub r0, r1, r0
 	bl toTwoPiScale
  	bl cosFixed_0ToPi4
  	lsr r0, r0, #1
 	pop  {r1, lr}
	bx lr

// Calculate sinus value [2/8..3/8] of circle
  .thumb
  .section .text
  .type sin_250_375, %function
  .align 4
sin_250_375:
 	push {r1, lr}
 	mov r1, #0x40000000 // ( X - PI/2)
 	sub r0, r0, r1
 	bl toTwoPiScale
  	bl cosFixed_0ToPi4
  	lsr r0, r0, #1
 	pop  {r1, lr}
	bx lr

// Calculate sinus value [3/8..4/8] of circle
  .thumb
  .global sin_375_500
  .section .text
  .type sin_375_500, %function
  .align 4
sin_375_500:
 	push {r1, lr}
 	mov r1, #0x80000000 // (PI/2 - X)
 	sub r0, r1, r0
 	bl toTwoPiScale
  	bl sinFixed_0ToPi4
  	lsr r0, r0, #1
 	pop  {r1, lr}
	bx lr


// Calculate sinus value [4/8..5/8] of circle
   .thumb
  .global sin_500_625
  .section .text
  .type sin_500_625, %function
  .align 4
sin_500_625:
 	push {r1, lr}
 	bl toTwoPiScale
  	bl sinFixed_0ToPi4
  	mvn r0, r0, lsr #1
  	add r0, r0, #1
 	pop  {r1, lr}
	bx lr

// Calculate sinus value [5/8..6/8] of circle
   .thumb
  .section .text
  .type sin_625_750, %function
  .align 4
sin_625_750:
 	push {r1, lr}
 	mov r1, #0x40000000 // ( 3PI/2 - X)
 	sub r0, r1, r0
 	bl toTwoPiScale
  	bl cosFixed_0ToPi4
  	mvn r0, r0, lsr #1
  	add r0, r0, #1
 	pop  {r1, lr}
	bx lr


// Calculate sinus value [6/8..7/8] of circle
  .thumb
  .section .text
  .type sin_750_875, %function
  .align 4
sin_750_875:
 	push {r1, lr}
 	mov r1, #0x40000000 // ( X - 3PI/2)
 	sub r0, r0, r1
 	bl toTwoPiScale
  	bl cosFixed_0ToPi4
  	mvn r0, r0, lsr #1
  	add r0, r0, #1
 	pop  {r1, lr}
	bx lr


// Calculate sinus value [7/8..1] of circle
  .thumb
  .section .text
  .type sin_875_000, %function
  .align 4
sin_875_000:
 	push {r1, lr}
 	mov r1, #0x80000000
 	sub r0, r1, r0
 	bl toTwoPiScale
  	bl sinFixed_0ToPi4
  	mvn r0, r0, lsr #1
  	add r0, r0, #1
 	pop  {r1, lr}
	bx lr


  .thumb
  .section .text
  .global sinFixed
  .type sinFixed, %function
  .align 4
// Calculate cos value [ 0 .. 1 - (1/1^24) ] of circle
sinFixed:
   push { r1, r2, lr }
   // Load return address
   ldr r1, =ret
   // Thumb mode reutrn
   orr lr, r1, #1
   // Get function pointer
   ldr r1, =#sin_func_ptr
   // Calculate offset
   lsr r2, r0, #(32 - 3)
   add r1, r1, r2, lsl #2
   // Load function pointer
   ldr r1, [r1]
   and r0, r0, #0x7FFFFFFF
   bx r1
ret:
   pop  { r1, r2, lr }
   bx lr

.align 4
sin_func_ptr:
.word  sin_000_125
.word  sin_125_250
.word  sin_250_375
.word  sin_375_500
.word  sin_500_625
.word  sin_625_750
.word  sin_750_875
.word  sin_875_000

// Calculate cos value [ 0 .. 1 - (1/1^24) ] of circle
  .thumb
  .global cosFixed
  .section .text
  .type cosFixed, %function
  .align 4
cosFixed:
  add r0, r0, #0x40000000
  b sinFixed
  bx lr




