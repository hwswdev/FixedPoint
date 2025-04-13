
/*
  ********************************************************************************************
  * @file      Sqrt.s dedicated to STM32Fxx device
  * @author    Evgeny Sobolev
  * @version   V1.0.0
  * @date      2025-03-04
  *
  * @description  Function unt32_t sqrt( uint32_t x )
  *
  * @description  0.0           =>  0x00000000,
  * @description  0.25          =>  0x40000000,
  * @description  0.5           =>  0x80000000,
  * @description  0.75          =>  0xC0000000,
  * @description  0.999999999xx =>  0xFFFFFFFF
  * @description  valid  x => [ 0x00000000 .. 0xFFFFFFFF ]
  *
  * @description  Return 32 bit value, where 16bit is decimal & 16bit is fractional
  *
  * @description  Where result, is scaled as
  * @description  0.0           =>  0x00000000,
  * @description  0.5           =>  0x40008000,
  * @description  0.25          =>  0x80004000,
  * @description  1.0           =>  0x00010000,
  * @description  65535.9999847 =>  0xFFFFFFFF
  *
  *******************************************************************************************
 */

.syntax unified
.cpu cortex-m3
.thumb

.global sqrtFixed
.global getMsb
.section .text

.type getMsb, %function
.align 4
getMsb:
	push {r1}
	mov r1, r0
	mov r0, #32
getMsbLoop:
    lsls r1, #1
    bcs getMsbLoopEnd
    sub r0, r0, #1
    cmp r0, #0
	bne getMsbLoop
getMsbLoopEnd:
	pop {r1}
	bx lr

.type sqrtFixed, %function
.align 4
sqrtFixed:
	push {r1-r9, lr}

	// Check if square root if value 0..256
	// It is faster, but can be removed
	cmp r0, #16
	ble sqrt_0_16

	// Generic calculations
	mov r8, r0
	bl getMsb
	mov r9, r0

	mov r1, #1
	lsl r1, r1, r9				// Xmin

	sub r5, r9, #2
	mov r2, #0x80000000
	lsr r2, r5					// 1/(Xmax-Xmin)

	tst r9, #1
	ite ne
	ldrne r4, =#0x6A09E668		// (Ymax - Ymin), it depends on sqrt(2) >> MSB, or 2 >> MSB
	ldreq r4, =#0x4AFB0CCC		// (Ymax - Ymin), it depends on sqrt(2) >> MSB, or 2 >> MSB
	lsr r5, r9, #1
	rsb r5, r5, #16
	lsr r4, r4, r5

	tst r9, #1
	itte ne
	ldrne r3, =#0x80000000		// Ymin
	subne r5, r5, #1			// Ymin, correct shift value, cause can't load 0x100000000
	ldreq r3, =#0xB504F334		// Ymin
	lsr r3, r3, r5

	sub r5, r8, r1        		// R5 <= X - Xmin
	mul r5, r5, r2  	  		// R5 <= (X - Xmin) / (Xmax - Xmin)
	umull r6, r7, r5, r4  		// R7 <= (X - Xmin) / (Xmax - Xmin) * (Ymax - Ymin)
	add r2, r7, r3		  		// R2 <= Ymin + (X - Xmin) / (Xmax - Xmin) * (Ymax - Ymin)
	mov r0, r2

	// Correct sqrt value, using another approx
	mov r0, r8
	sub r1, r9, #1
	bl quad_correction_calc
	adds r0, r2, r0
	it cs
	ldrcs r0, =#0xFFFB0000

	pop {r1-r9, lr}
	bx lr

sqrt_0_16:
	ldr r2, =#sqrt_0_16_table
	ldr r0, [r2, r0, lsl #2 ]
	pop {r1-r9, lr}
	bx lr

.align 4
sqrt_0_16_table:
.word 0x00000000  // sqrt(0) = 0.00000
.word 0x00010000  // sqrt(1) = 1.00000
.word 0x00016A09  // sqrt(2) = 1.41421
.word 0x0001BB67  // sqrt(3) = 1.73205
.word 0x00020000  // sqrt(4) = 2.00000
.word 0x00023C6E  // sqrt(5) = 2.23607
.word 0x00027311  // sqrt(6) = 2.44949
.word 0x0002A54F  // sqrt(7) = 2.64575
.word 0x0002D413  // sqrt(8) = 2.82843
.word 0x00030000  // sqrt(9) = 3.00000
.word 0x0003298B  // sqrt(10) = 3.16228
.word 0x0003510E  // sqrt(11) = 3.31662
.word 0x000376CF  // sqrt(12) = 3.46410
.word 0x00039B05  // sqrt(13) = 3.60555
.word 0x0003BDDD  // sqrt(14) = 3.74166
.word 0x0003DF7B  // sqrt(15) = 3.87298
.word 0x00040000  // sqrt(16) = 4.00000


.global quad_dx_calc
.type quad_dx_calc, %function
.align 4
quad_dx_calc:
	push {r4-r6, lr}
	lsr r0, r0, #3		// R0 <= X >> 3
	ldr r4, =0x2F504F33	// 0.1848191738
	rsb r5, r1, #32
	lsr r4, r4, r5		// R4 <= R4 >> (32 - MSB), i.e HI(R4 << MSB)
	subs r4, r0, r4		// R4 <= (X >> 3) - (0.1848191738 << MSB)
	it cc
	negscc r4, r4		// R4 <= abs(R4)
	mov r6, #1			// R6 <= 1
	subs r5, r1, #4
	ite cc
	movcc r6, #0			// R6 <= 0
	lslcs r6, r6, r5	// R6 <= 1 << (MSB-4)
	sub r0, r6, r4		// R0 <= ( 1 << (MSB-4)) - abs( (X >> 3) - (0.1848191738 << MSB) )
	pop {r4-r6, lr}
	bx lr

.global quad_half_calc
.type quad_half_calc, %function
.align 4
quad_half_calc:
	push {r4-r5, lr}
	mov r4, #3
	subs r5, r1, #1
	ite cc
	movscc r0, #1
	lslcs r0, r4, r5
	pop {r4-r5, lr}
	bx lr

.global quad_correction_calc
.type quad_correction_calc, %function
.align 4
quad_correction_calc:
	push {r4-r9, lr}
	mov r4, r0				// R4 <= X
	bl quad_half_calc
	sub r5, r4, r0			// R5 <= X - (Xmax + Xmin)/2,
	mov r0, r4				// R0 <= X
	bl quad_dx_calc			// R0 <= dXCorr, i.e shifted delta X
	adds r5, r5, r0			// R5 <= (X - (Xmax + Xmin)/2) + dXCorr
	it le
	negsle r5, r5			// R5 <= abs(R5), i.e. abs( (X - (Xmax + Xmin)/2) + dXCorr )
	rsbs r8, r1, #17
	itte cc
	negcc r8, r8
	lsrcc  r5, r5, r8
	lslcs  r5, r5, r8

	cmp r5, #0x10000
	it ge
	ldrge r5, =#0xFFFF

	umull r6, r7, r5, r5	// [R6,R7] <= ( Corrected(X) ^ 2 )
	rsb r0, r6, #0xFFFFFFFF

	tst r1, #1
	ite ne
	ldrne r7, =#0x066EACCA	// Maximum difference beetween linear approx of sqrt, and real value  sqrt(2) << N or 2 << N
	ldreq r7, =#0x048C6001	// Maximum difference beetween linear approx of sqrt, and real valuee sqrt(2) << N or 2 << N
	lsr r6, r1, #1
	rsb r6, r6, #16
	lsr r7, r7, r6

	umull r6, r7, r0, r7
	mov r0, r7
	pop {r4-r9, lr}
	bx lr



/*
.type sqrt_linear_err_calc, %function
.align 4
sqrt_linear_err_calc:
	push {r4-r8, lr}
	mov r4, r0  		// R4 <= (X - X0)/(Xm - X0)
	lsr r8, r1, #1
	rsb r8, r8, #16

	lsl r5, r1, #(3+2)
	and r5, r5, #0x20
	ldr r6, =sqrt_err_approx_table
	add r5, r6, r5

	mov r6, #0

	lsr r4, r4, #1
	mov r7, r4

	// R1
	mov r0, r7
	ldmia r5!, {r1}
	lsr r1, r1, r8
	bl rSinFixed
	add r6, r6, r0

	// R2
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	lsr r1, r1, r8
	bl rSinFixed
	add r6, r6, r0

	// R3
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	lsr r1, r1, r8
	bl rSinFixed
	add r6, r6, r0

	// R4
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	lsr r1, r1, r8
	bl rSinFixed
	add r6, r6, r0

	// R5
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	lsr r1, r1, r8
	bl rSinFixed
	add r6, r6, r0

	// R6
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	lsr r1, r1, r8
	bl rSinFixed
	add r6, r6, r0

	// R7
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	lsr r1, r1, r8
	bl rSinFixed
	add r6, r6, r0

	// R8
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	lsr r1, r1, r8
	bl rSinFixed
	add r6, r6, r0

	// Error as 16bits (not 20bits)
	asr r0, r6, #4

	pop {r4-r8, lr}
	bx lr

.align 4
sqrt_err_approx_table:
// X0 = 0x100000000 (2^32)
.word 0x34FBE4DD  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   888923357
.word 0x036544C2  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   56968386
.word 0x024598FA  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   38115578
.word 0x007515A7  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   7673255
.word 0x007FF8E1  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 8386785
.word 0x00234631  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     2311729
.word 0x002EE5BF  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 3073471
.word 0x000EF91D  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     981277
// X0 = 0x200000000 (2^33)
.word 0x4AEE3E2C  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   1257127468
.word 0x04CD54D8  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   80565464
.word 0x033680D0  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   53903568
.word 0x00A59525  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   10851621
.word 0x00B4FAE1  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 11860705
.word 0x0031E29E  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     3269278
.word 0x004252B1  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 4346545
.word 0x00152CD7  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     1387735
*/

