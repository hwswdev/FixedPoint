
/*
  ********************************************************************************************
  * @file      Sqrt.s dedicated to STM32Fxx device
  * @author    Evgeny Sobolev
  * @version   V1.0.0
  * @date      2025-03-04
  *
  * @description  Function unt32_t sqrt( uint32_t x )
  *
  * @description  0  	        =>  0x00000000,
  * @description  65536         =>  0x00010000,
  * @description  2^32-1        =>  0xFFFFFFFF
  * @description  valid  x => [ 0x00000000 .. 0xFFFFFFFF ]
  *
  * @description  Return 32 bit value, where 16bit is decimal & 16bit is fractional
  *
  * @description  Where result, is scaled as
  * @description  0.0           =>  0x00000000,
  * @description  0.5           =>  0x00008000,
  * @description  0.25          =>  0x00004000,
  * @description  1.0           =>  0x00010000,
  * @description  32768.25      =>  0x80004000,
  * @description  65535.9999847 =>  0xFFFFFFFF
  *
  * @description  accuracy is about 0.1
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
	bls sqrt_0_16

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
	adds r2, r2, r0
	mov r0, r2

	// Correcrt sqrt value using itaration
	//mov r0, r8
	//sub r1, r9, #1
	//bl sqrt_iter_correction_calc

	pop {r1-r9, lr}
	bx lr

sqrt_0_16:
	ldr r2, =#sqrt_0_16_table
	ldr r0, [r2, r0, lsl #2 ]
	pop {r1-r9, lr}
	bx lr


.global quad_correction_calc
.type quad_correction_calc, %function
.align 4
quad_correction_calc:
	push {r4-r7, lr}

	// Parable
	rsbs r4, r1, #30		// Shift value to maximum
	itte cc
	negcc r4, r4
	lsrcc r5, r0, r4
	lslcs r5, r0, r4
	mov r7, r5				 	// Shifted to maxumum value
	rsbs r4, r5, #0x60000000 	// Get difference between value and center
	it cc
	negcc r4, r4
	lsl r4, r4, #2
	umull r4, r5, r4, r4
	rsbs r5, r5, #0x40000000
	it cc
	negcc r5, r5
	cmp r5, #0x40000000
	ite eq
	moveq r5, #0xFFFFFFFF
	lslne r5, r5, #2
	// ( Xmid - XmaxVal ) * Parable
	ldr r4, =#0xAFB0CCC0 		// 184224972.024 * 2^4 - i.e maximum x shift on 32-bit value
	rsb r6, r1, #32
	lsr r4, r4, r6				// Error is 184224972 / (2 ^ BitCount-32)
	umull r4, r5, r4, r5		// Multiply maxumum error shifted by 16bit
	adds r4, r4, #0x80000000
	adc r5, r5, #0				// Compensate multiply error
	lsr r6, r5, #4				// (0xAFB0CCC << 4, i.e *2^4), have to shift back to there original value


	// Quad corrected err approxymation parable
	rsbs r4, r1, #30			// Shift value to correction
	itte cc
	negcc r4, r4				// R4 <= abs(R4)
	lsrcc r5, r6, r4		 	// R5 <= (R6) >> abs(R4), R4 = 30 - R1
	lslcs r5, r6, r4		 	// R5 <= (R6) << abs(R4), R4 = 30 - R1
	add r5, r5, r7			 	// R4 <= Shifted to maxumum value + correction
	rsbs r4, r5, #0x60000000 	// Get difference between corrected X value and center
	it cc
	negcc r4, r4
	lsl r4, r4, #2
	umull r4, r5, r4, r4
	rsbs r5, r5, #0x40000000
	it cc
	negcc r5, r5
	cmp r5, #0x40000000
	ite eq
	moveq r5, #0xFFFFFFFF
	lslne r5, r5, #2

	tst r1, #1
	ite ne
	ldrne r7, =#0x66EACCA0		// Maximum difference beetween linear approx of sqrt, and real value  sqrt(2) << N or 2 << N
	ldreq r7, =#0x48C60010		// Maximum difference beetween linear approx of sqrt, and real valuee sqrt(2) << N or 2 << N
	lsr r6, r1, #1
	rsb r6, r6, #16
	lsr r7, r7, r6
	umull r6, r7, r5, r7
	lsr r0, r7, #4

	pop {r4-r7, lr}
	bx lr



.align 4
sqrt_0_16_table:
.word 0x00000000 // sqrt(00) = 0.000000, err = 0.000000
.word 0x00010000 // sqrt(01) = 1.000000, err = 0.000000
.word 0x00016A09 // sqrt(02) = 1.414200, err = -0.000014
.word 0x0001BB67 // sqrt(03) = 1.732040, err = -0.000010
.word 0x00020000 // sqrt(04) = 2.000000, err = 0.000000
.word 0x00023C6E // sqrt(05) = 2.236053, err = -0.000015
.word 0x00027311 // sqrt(06) = 2.449478, err = -0.000012
.word 0x0002A54F // sqrt(07) = 2.645737, err = -0.000015
.word 0x0002D413 // sqrt(08) = 2.828415, err = -0.000012
.word 0x00030000 // sqrt(09) = 3.000000, err = 0.000000
.word 0x0003298B // sqrt(10) = 3.162277, err = -0.000000
.word 0x0003510E // sqrt(11) = 3.316620, err = -0.000005
.word 0x000376CF // sqrt(12) = 3.464096, err = -0.000006
.word 0x00039B05 // sqrt(13) = 3.605545, err = -0.000006
.word 0x0003BDDD // sqrt(14) = 3.741653, err = -0.000004
.word 0x0003DF7B // sqrt(15) = 3.872971, err = -0.000013
.word 0x00040000 // sqrt(16) = 4.000000, err = 0.000000
