
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

.type getMsbY, %function
.align 4
getMsbY:
	push {lr}
	bl getMsb
	add r0, r0, #33
	lsr r0, r0, #1
	pop {lr}
	bx lr


.type sqrtFixed, %function
.align 4
sqrtFixed:
	push {r1-r9, lr}

	// Check if square root if value 0..256
	cmp r0, #256
	ble sqrt_0_256

	mov r8, r0
	bl getMsb
	mov r9, r0					// R9 <= MSB i.e table_index
	lsl r0, r0, #(2+2)		 	// R0 <= MSB * 4 (element) * 4 (sizeof uint32_t)
	ldr r1, =sqrt_approx_table	// R1 <= sqrt_approx_table
	add r0, r0, r1				// R0 <= sqrt_approx_table[MSB]

	ldmia r0, {r1-r4}			// R1..R4, values
	sub r5, r8, r1        		// R5 <= X - X0
	mul r5, r5, r2  	  		// R5 <= (X - X0) / dX
	umull r6, r7, r5, r4  		// R7 <= (X - X0) / dX * dY
	add r2, r7, r3		  		// R2 <= Y0 + (X - X0) / dX * dY

	// Linear error correction calc
	mov r0, r5
	mov r1, r9
	bl sqrt_linear_err_calc
	add r0, r2, r0				// R0 <= sqrt(x) * 65536

	// TO DO: calculate accuracy value

	pop {r1-r9, lr}
	bx lr
sqrt_0_256:
	ldr r2, =#sqrt_0_31_table
	ldr r0, [r2, r0, lsl #2 ]
	pop {r1-r9, lr}
	bx lr


.align 4
sqrt_approx_table:
.word 0x00000000  // X0   = 0x00000000 (0)
.word 0xFFFFFFFF  // 1/dX = 1/0x00000001
.word 0x00000000  // Y0   = sqrt(0x00000000)
.word 0x00010000  // dY   = 0x00010000
.word 0x00000001  // X0   = 0x00000001 (1)
.word 0xFFFFFFFF  // 1/dX = 1/0x00000001
.word 0x00010000  // Y0   = sqrt(0x00000001)
.word 0x00006A09  // dY   = 0x00006A09
.word 0x00000002  // X0   = 0x00000002 (2)
.word 0x80000000  // 1/dX = 1/0x00000002
.word 0x00016A09  // Y0   = sqrt(0x00000002)
.word 0x000095F7  // dY   = 0x000095F7
.word 0x00000004  // X0   = 0x00000004 (4)
.word 0x40000000  // 1/dX = 1/0x00000004
.word 0x00020000  // Y0   = sqrt(0x00000004)
.word 0x0000D413  // dY   = 0x0000D413
.word 0x00000008  // X0   = 0x00000008 (8)
.word 0x20000000  // 1/dX = 1/0x00000008
.word 0x0002D413  // Y0   = sqrt(0x00000008)
.word 0x00012BED  // dY   = 0x00012BED
.word 0x00000010  // X0   = 0x00000010 (16)
.word 0x10000000  // 1/dX = 1/0x00000010
.word 0x00040000  // Y0   = sqrt(0x00000010)
.word 0x0001A827  // dY   = 0x0001A827
.word 0x00000020  // X0   = 0x00000020 (32)
.word 0x08000000  // 1/dX = 1/0x00000020
.word 0x0005A827  // Y0   = sqrt(0x00000020)
.word 0x000257D9  // dY   = 0x000257D9
.word 0x00000040  // X0   = 0x00000040 (64)
.word 0x04000000  // 1/dX = 1/0x00000040
.word 0x00080000  // Y0   = sqrt(0x00000040)
.word 0x0003504F  // dY   = 0x0003504F
.word 0x00000080  // X0   = 0x00000080 (128)
.word 0x02000000  // 1/dX = 1/0x00000080
.word 0x000B504F  // Y0   = sqrt(0x00000080)
.word 0x0004AFB1  // dY   = 0x0004AFB1
.word 0x00000100  // X0   = 0x00000100 (256)
.word 0x01000000  // 1/dX = 1/0x00000100
.word 0x00100000  // Y0   = sqrt(0x00000100)
.word 0x0006A09E  // dY   = 0x0006A09E
.word 0x00000200  // X0   = 0x00000200 (512)
.word 0x00800000  // 1/dX = 1/0x00000200
.word 0x0016A09E  // Y0   = sqrt(0x00000200)
.word 0x00095F62  // dY   = 0x00095F62
.word 0x00000400  // X0   = 0x00000400 (1024)
.word 0x00400000  // 1/dX = 1/0x00000400
.word 0x00200000  // Y0   = sqrt(0x00000400)
.word 0x000D413C  // dY   = 0x000D413C
.word 0x00000800  // X0   = 0x00000800 (2048)
.word 0x00200000  // 1/dX = 1/0x00000800
.word 0x002D413C  // Y0   = sqrt(0x00000800)
.word 0x0012BEC4  // dY   = 0x0012BEC4
.word 0x00001000  // X0   = 0x00001000 (4096)
.word 0x00100000  // 1/dX = 1/0x00001000
.word 0x00400000  // Y0   = sqrt(0x00001000)
.word 0x001A8279  // dY   = 0x001A8279
.word 0x00002000  // X0   = 0x00002000 (8192)
.word 0x00080000  // 1/dX = 1/0x00002000
.word 0x005A8279  // Y0   = sqrt(0x00002000)
.word 0x00257D87  // dY   = 0x00257D87
.word 0x00004000  // X0   = 0x00004000 (16384)
.word 0x00040000  // 1/dX = 1/0x00004000
.word 0x00800000  // Y0   = sqrt(0x00004000)
.word 0x003504F3  // dY   = 0x003504F3
.word 0x00008000  // X0   = 0x00008000 (32768)
.word 0x00020000  // 1/dX = 1/0x00008000
.word 0x00B504F3  // Y0   = sqrt(0x00008000)
.word 0x004AFB0D  // dY   = 0x004AFB0D
.word 0x00010000  // X0   = 0x00010000 (65536)
.word 0x00010000  // 1/dX = 1/0x00010000
.word 0x01000000  // Y0   = sqrt(0x00010000)
.word 0x006A09E6  // dY   = 0x006A09E6
.word 0x00020000  // X0   = 0x00020000 (131072)
.word 0x00008000  // 1/dX = 1/0x00020000
.word 0x016A09E6  // Y0   = sqrt(0x00020000)
.word 0x0095F61A  // dY   = 0x0095F61A
.word 0x00040000  // X0   = 0x00040000 (262144)
.word 0x00004000  // 1/dX = 1/0x00040000
.word 0x02000000  // Y0   = sqrt(0x00040000)
.word 0x00D413CC  // dY   = 0x00D413CC
.word 0x00080000  // X0   = 0x00080000 (524288)
.word 0x00002000  // 1/dX = 1/0x00080000
.word 0x02D413CC  // Y0   = sqrt(0x00080000)
.word 0x012BEC34  // dY   = 0x012BEC34
.word 0x00100000  // X0   = 0x00100000 (1048576)
.word 0x00001000  // 1/dX = 1/0x00100000
.word 0x04000000  // Y0   = sqrt(0x00100000)
.word 0x01A82799  // dY   = 0x01A82799
.word 0x00200000  // X0   = 0x00200000 (2097152)
.word 0x00000800  // 1/dX = 1/0x00200000
.word 0x05A82799  // Y0   = sqrt(0x00200000)
.word 0x0257D867  // dY   = 0x0257D867
.word 0x00400000  // X0   = 0x00400000 (4194304)
.word 0x00000400  // 1/dX = 1/0x00400000
.word 0x08000000  // Y0   = sqrt(0x00400000)
.word 0x03504F33  // dY   = 0x03504F33
.word 0x00800000  // X0   = 0x00800000 (8388608)
.word 0x00000200  // 1/dX = 1/0x00800000
.word 0x0B504F33  // Y0   = sqrt(0x00800000)
.word 0x04AFB0CD  // dY   = 0x04AFB0CD
.word 0x01000000  // X0   = 0x01000000 (16777216)
.word 0x00000100  // 1/dX = 1/0x01000000
.word 0x10000000  // Y0   = sqrt(0x01000000)
.word 0x06A09E66  // dY   = 0x06A09E66
.word 0x02000000  // X0   = 0x02000000 (33554432)
.word 0x00000080  // 1/dX = 1/0x02000000
.word 0x16A09E66  // Y0   = sqrt(0x02000000)
.word 0x095F619A  // dY   = 0x095F619A
.word 0x04000000  // X0   = 0x04000000 (67108864)
.word 0x00000040  // 1/dX = 1/0x04000000
.word 0x20000000  // Y0   = sqrt(0x04000000)
.word 0x0D413CCC  // dY   = 0x0D413CCC
.word 0x08000000  // X0   = 0x08000000 (134217728)
.word 0x00000020  // 1/dX = 1/0x08000000
.word 0x2D413CCC  // Y0   = sqrt(0x08000000)
.word 0x12BEC334  // dY   = 0x12BEC334
.word 0x10000000  // X0   = 0x10000000 (268435456)
.word 0x00000010  // 1/dX = 1/0x10000000
.word 0x40000000  // Y0   = sqrt(0x10000000)
.word 0x1A827999  // dY   = 0x1A827999
.word 0x20000000  // X0   = 0x20000000 (536870912)
.word 0x00000008  // 1/dX = 1/0x20000000
.word 0x5A827999  // Y0   = sqrt(0x20000000)
.word 0x257D8667  // dY   = 0x257D8667
.word 0x40000000  // X0   = 0x40000000 (1073741824)
.word 0x00000004  // 1/dX = 1/0x40000000
.word 0x80000000  // Y0   = sqrt(0x40000000)
.word 0x3504F333  // dY   = 0x3504F333
.word 0x80000000  // X0   = 0x80000000 (-2147483648)
.word 0x00000002  // 1/dX = 1/0x80000000
.word 0xB504F333  // Y0   = sqrt(0x80000000)
.word 0x4AFB0CCD  // dY   = 0x4AFB0CCD


.type sqrt_linear_err_calc, %function
.align 4
sqrt_linear_err_calc:
	push {r4-r7, lr}
	mov r4, r0  		// R4 <= (X - X0)/(Xm - X0)

	lsl r5, r1, #(3+2)
	ldr r6, =sqrt_err_approx_table
	add r5, r6, r5

	mov r6, #0

	lsr r4, r4, #1
	mov r7, r4

	// 1
	mov r0, r7
	ldmia r5!, {r1}
	bl rSinFixed
	add r6, r6, r0

	// 2
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	bl rSinFixed
	add r6, r6, r0

	// 3
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	bl rSinFixed
	add r6, r6, r0

	// 4
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	bl rSinFixed
	add r6, r6, r0

	// 5
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	bl rSinFixed
	add r6, r6, r0

	// 6
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	bl rSinFixed
	add r6, r6, r0

	// 7
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	bl rSinFixed
	add r6, r6, r0

	// 8
	add r7, r7, r4
	mov r0, r7
	ldmia r5!, {r1}
	bl rSinFixed
	add r6, r6, r0

	// Error as 16bits (not 20bits)
	asr r0, r6, #4

	pop {r4-r7, lr}
	bx lr

.align 4
.align 4
sqrt_err_approx_table:
.word 0x00000000  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   0
.word 0x00000000  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   0
.word 0x00000000  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   0
.word 0x00000000  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   0
.word 0x00000000  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 0
.word 0x00000000  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     0
.word 0x00000000  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 0
.word 0x00000000  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     0
.word 0x00004AEE  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   19182
.word 0x000004CD  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   1229
.word 0x00000337  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   823
.word 0x000000A6  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   166
.word 0x000000B5  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 181
.word 0x00000032  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     50
.word 0x00000042  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 66
.word 0x00000015  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     21
.word 0x000069F8  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   27128
.word 0x000006CB  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   1739
.word 0x0000048B  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   1163
.word 0x000000EA  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   234
.word 0x00000100  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 256
.word 0x00000047  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     71
.word 0x0000005E  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 94
.word 0x0000001E  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     30
.word 0x000095DC  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   38364
.word 0x0000099B  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   2459
.word 0x0000066D  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   1645
.word 0x0000014B  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   331
.word 0x0000016A  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 362
.word 0x00000064  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     100
.word 0x00000085  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 133
.word 0x0000002A  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     42
.word 0x0000D3F0  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   54256
.word 0x00000D95  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   3477
.word 0x00000916  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   2326
.word 0x000001D4  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   468
.word 0x00000200  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 512
.word 0x0000008D  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     141
.word 0x000000BC  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 188
.word 0x0000003C  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     60
.word 0x00012BB9  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   76729
.word 0x00001335  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   4917
.word 0x00000CDA  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   3290
.word 0x00000296  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   662
.word 0x000002D4  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 724
.word 0x000000C8  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     200
.word 0x00000109  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 265
.word 0x00000055  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     85
.word 0x0001A7DF  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   108511
.word 0x00001B2A  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   6954
.word 0x0000122D  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   4653
.word 0x000003A9  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   937
.word 0x00000400  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 1024
.word 0x0000011A  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     282
.word 0x00000177  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 375
.word 0x00000078  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     120
.word 0x00025772  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   153458
.word 0x0000266B  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   9835
.word 0x000019B4  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   6580
.word 0x0000052D  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   1325
.word 0x000005A8  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 1448
.word 0x0000018F  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     399
.word 0x00000213  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 531
.word 0x000000A9  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     169
.word 0x00034FBE  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   217022
.word 0x00003654  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   13908
.word 0x0000245A  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   9306
.word 0x00000751  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   1873
.word 0x00000800  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 2048
.word 0x00000234  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     564
.word 0x000002EE  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 750
.word 0x000000F0  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     240
.word 0x0004AEE4  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   306916
.word 0x00004CD5  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   19669
.word 0x00003368  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   13160
.word 0x00000A59  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   2649
.word 0x00000B50  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 2896
.word 0x0000031E  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     798
.word 0x00000425  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 1061
.word 0x00000153  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     339
.word 0x00069F7D  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   434045
.word 0x00006CA9  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   27817
.word 0x000048B3  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   18611
.word 0x00000EA3  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   3747
.word 0x00000FFF  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 4095
.word 0x00000469  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     1129
.word 0x000005DD  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 1501
.word 0x000001DF  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     479
.word 0x00095DC8  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   613832
.word 0x000099AB  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   39339
.word 0x000066D0  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   26320
.word 0x000014B3  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   5299
.word 0x0000169F  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 5791
.word 0x0000063C  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     1596
.word 0x0000084A  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 2122
.word 0x000002A6  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     678
.word 0x000D3EF9  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   868089
.word 0x0000D951  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   55633
.word 0x00009166  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   37222
.word 0x00001D45  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   7493
.word 0x00001FFE  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 8190
.word 0x000008D2  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     2258
.word 0x00000BB9  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 3001
.word 0x000003BE  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     958
.word 0x0012BB90  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   1227664
.word 0x00013355  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   78677
.word 0x0000CDA0  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   52640
.word 0x00002965  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   10597
.word 0x00002D3F  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 11583
.word 0x00000C79  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     3193
.word 0x00001095  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 4245
.word 0x0000054B  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     1355
.word 0x001A7DF2  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   1736178
.word 0x0001B2A2  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   111266
.word 0x000122CC  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   74444
.word 0x00003A8B  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   14987
.word 0x00003FFC  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 16380
.word 0x000011A3  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     4515
.word 0x00001773  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 6003
.word 0x0000077D  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     1917
.word 0x0025771F  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   2455327
.word 0x000266AA  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   157354
.word 0x00019B40  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   105280
.word 0x000052CB  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   21195
.word 0x00005A7D  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 23165
.word 0x000018F1  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     6385
.word 0x00002129  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 8489
.word 0x00000A96  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     2710
.word 0x0034FBE5  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   3472357
.word 0x00036545  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   222533
.word 0x00024599  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   148889
.word 0x00007516  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   29974
.word 0x00007FF9  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 32761
.word 0x00002346  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     9030
.word 0x00002EE6  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 12006
.word 0x00000EF9  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     3833
.word 0x004AEE3E  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   4910654
.word 0x0004CD55  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   314709
.word 0x00033681  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   210561
.word 0x0000A595  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   42389
.word 0x0000B4FB  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 46331
.word 0x000031E3  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     12771
.word 0x00004253  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 16979
.word 0x0000152D  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     5421
.word 0x0069F7CA  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   6944714
.word 0x0006CA8A  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   445066
.word 0x00048B32  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   297778
.word 0x0000EA2B  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   59947
.word 0x0000FFF2  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 65522
.word 0x0000468C  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     18060
.word 0x00005DCB  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 24011
.word 0x00001DF2  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     7666
.word 0x0095DC7C  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   9821308
.word 0x00099AAA  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   629418
.word 0x00066D02  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   421122
.word 0x00014B2A  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   84778
.word 0x000169F6  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 92662
.word 0x000063C5  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     25541
.word 0x000084A5  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 33957
.word 0x00002A5A  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     10842
.word 0x00D3EF93  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   13889427
.word 0x000D9513  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   890131
.word 0x00091664  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   595556
.word 0x0001D457  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   119895
.word 0x0001FFE4  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 131044
.word 0x00008D19  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     36121
.word 0x0000BB97  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 48023
.word 0x00003BE4  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     15332
.word 0x012BB8F9  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   19642617
.word 0x00133553  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   1258835
.word 0x000CDA03  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   842243
.word 0x00029655  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   169557
.word 0x0002D3EC  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 185324
.word 0x0000C78A  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     51082
.word 0x0001094B  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 67915
.word 0x000054B3  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     21683
.word 0x01A7DF27  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   27778855
.word 0x001B2A26  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   1780262
.word 0x00122CC8  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   1191112
.word 0x0003A8AD  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   239789
.word 0x0003FFC7  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 262087
.word 0x00011A32  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     72242
.word 0x0001772E  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 96046
.word 0x000077C9  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     30665
.word 0x025771F1  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   39285233
.word 0x00266AA7  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   2517671
.word 0x0019B407  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   1684487
.word 0x00052CA9  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   339113
.word 0x0005A7D7  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 370647
.word 0x00018F15  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     102165
.word 0x00021296  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 135830
.word 0x0000A967  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     43367
.word 0x034FBE4E  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   55557710
.word 0x0036544C  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   3560524
.word 0x00245990  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   2382224
.word 0x0007515A  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   479578
.word 0x0007FF8E  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 524174
.word 0x00023463  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     144483
.word 0x0002EE5C  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 192092
.word 0x0000EF92  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     61330
.word 0x04AEE3E3  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   78570467
.word 0x004CD54E  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   5035342
.word 0x0033680D  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   3368973
.word 0x000A5952  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   678226
.word 0x000B4FAE  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 741294
.word 0x00031E2A  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     204330
.word 0x0004252B  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 271659
.word 0x000152CD  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     86733
.word 0x069F7C9C  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   111115420
.word 0x006CA898  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   7121048
.word 0x0048B31F  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   4764447
.word 0x000EA2B5  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   959157
.word 0x000FFF1C  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 1048348
.word 0x000468C6  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     288966
.word 0x0005DCB8  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 384184
.word 0x0001DF24  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     122660
.word 0x095DC7C6  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   157140934
.word 0x0099AA9B  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   10070683
.word 0x0066D01A  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   6737946
.word 0x0014B2A5  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   1356453
.word 0x00169F5C  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 1482588
.word 0x00063C54  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     408660
.word 0x00084A56  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 543318
.word 0x0002A59B  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     173467
.word 0x0D3EF937  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   222230839
.word 0x00D95131  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   14242097
.word 0x0091663F  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   9528895
.word 0x001D456A  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   1918314
.word 0x001FFE38  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 2096696
.word 0x0008D18C  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     577932
.word 0x000BB970  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 768368
.word 0x0003BE47  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     245319
.word 0x12BB8F8B  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   314281867
.word 0x01335536  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   20141366
.word 0x00CDA034  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   13475892
.word 0x00296549  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   2712905
.word 0x002D3EB8  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 2965176
.word 0x000C78A8  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     817320
.word 0x001094AC  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 1086636
.word 0x00054B36  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     346934
.word 0x1A7DF26F  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   444461679
.word 0x01B2A261  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   28484193
.word 0x0122CC7D  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   19057789
.word 0x003A8AD3  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   3836627
.word 0x003FFC71  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 4193393
.word 0x0011A318  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     1155864
.word 0x001772E0  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 1536736
.word 0x00077C8E  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     490638
.word 0x25771F16  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   628563734
.word 0x0266AA6C  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   40282732
.word 0x019B4068  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   26951784
.word 0x0052CA92  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   5425810
.word 0x005A7D71  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 5930353
.word 0x0018F14F  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     1634639
.word 0x00212958  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 2173272
.word 0x000A966B  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     693867
.word 0x34FBE4DD  // R1, i.e. R1 * rSinFixed( ( (X - X0)/dX ) / 2   ),   888923357
.word 0x036544C2  // R2, i.e. R2 * rSinFixed( ( (X - X0)/dX ) * 1   ),   56968386
.word 0x024598FA  // R3, i.e. R3 * rSinFixed( ( (X - X0)/dX ) * 3/2 ),   38115578
.word 0x007515A7  // R4, i.e. R4 * rSinFixed( ( (X - X0)/dX ) * 2   ),   7673255
.word 0x007FF8E1  // R5, i.e. R5 * rSinFixed( ( (X - X0)/dX ) * 5 / 2 ), 8386785
.word 0x00234631  // R6, i.e. R6 * rSinFixed( ( (X - X0)/dX ) * 3 ),     2311729
.word 0x002EE5BF  // R7, i.e. R7 * rSinFixed( ( (X - X0)/dX ) * 7 / 2 ), 3073471
.word 0x000EF91D  // R8, i.e. R8 * rSinFixed( ( (X - X0)/dX ) * 4 ),     981277



.align 4
sqrt_0_31_table:
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
.word 0x00041F83  // sqrt(17) = 4.12311
.word 0x00043E1D  // sqrt(18) = 4.24264
.word 0x00045BE0  // sqrt(19) = 4.35890
.word 0x000478DD  // sqrt(20) = 4.47214
.word 0x00049523  // sqrt(21) = 4.58258
.word 0x0004B0BF  // sqrt(22) = 4.69042
.word 0x0004CBBB  // sqrt(23) = 4.79583
.word 0x0004E623  // sqrt(24) = 4.89898
.word 0x00050000  // sqrt(25) = 5.00000
.word 0x00051959  // sqrt(26) = 5.09902
.word 0x00053237  // sqrt(27) = 5.19615
.word 0x00054A9F  // sqrt(28) = 5.29150
.word 0x0005629A  // sqrt(29) = 5.38516
.word 0x00057A2B  // sqrt(30) = 5.47723
.word 0x00059159  // sqrt(31) = 5.56776
.word 0x0005A827  // sqrt(32) = 5.65685
.word 0x0005BE9B  // sqrt(33) = 5.74456
.word 0x0005D4B9  // sqrt(34) = 5.83095
.word 0x0005EA84  // sqrt(35) = 5.91608
.word 0x00060000  // sqrt(36) = 6.00000
.word 0x0006152F  // sqrt(37) = 6.08276
.word 0x00062A17  // sqrt(38) = 6.16441
.word 0x00063EB8  // sqrt(39) = 6.24500
.word 0x00065316  // sqrt(40) = 6.32456
.word 0x00066733  // sqrt(41) = 6.40312
.word 0x00067B11  // sqrt(42) = 6.48074
.word 0x00068EB4  // sqrt(43) = 6.55744
.word 0x0006A21C  // sqrt(44) = 6.63325
.word 0x0006B54C  // sqrt(45) = 6.70820
.word 0x0006C846  // sqrt(46) = 6.78233
.word 0x0006DB0C  // sqrt(47) = 6.85565
.word 0x0006ED9E  // sqrt(48) = 6.92820
.word 0x00070000  // sqrt(49) = 7.00000
.word 0x00071231  // sqrt(50) = 7.07107
.word 0x00072434  // sqrt(51) = 7.14143
.word 0x0007360A  // sqrt(52) = 7.21110
.word 0x000747B5  // sqrt(53) = 7.28011
.word 0x00075935  // sqrt(54) = 7.34847
.word 0x00076A8B  // sqrt(55) = 7.41620
.word 0x00077BBA  // sqrt(56) = 7.48331
.word 0x00078CC1  // sqrt(57) = 7.54983
.word 0x00079DA3  // sqrt(58) = 7.61577
.word 0x0007AE5F  // sqrt(59) = 7.68115
.word 0x0007BEF7  // sqrt(60) = 7.74597
.word 0x0007CF6C  // sqrt(61) = 7.81025
.word 0x0007DFBE  // sqrt(62) = 7.87401
.word 0x0007EFEF  // sqrt(63) = 7.93725
.word 0x00080000  // sqrt(64) = 8.00000
.word 0x00080FF0  // sqrt(65) = 8.06226
.word 0x00081FC0  // sqrt(66) = 8.12404
.word 0x00082F73  // sqrt(67) = 8.18535
.word 0x00083F07  // sqrt(68) = 8.24621
.word 0x00084E7E  // sqrt(69) = 8.30662
.word 0x00085DD9  // sqrt(70) = 8.36660
.word 0x00086D18  // sqrt(71) = 8.42615
.word 0x00087C3B  // sqrt(72) = 8.48528
.word 0x00088B43  // sqrt(73) = 8.54400
.word 0x00089A31  // sqrt(74) = 8.60233
.word 0x0008A906  // sqrt(75) = 8.66025
.word 0x0008B7C1  // sqrt(76) = 8.71780
.word 0x0008C664  // sqrt(77) = 8.77496
.word 0x0008D4EE  // sqrt(78) = 8.83176
.word 0x0008E360  // sqrt(79) = 8.88819
.word 0x0008F1BB  // sqrt(80) = 8.94427
.word 0x00090000  // sqrt(81) = 9.00000
.word 0x00090E2D  // sqrt(82) = 9.05539
.word 0x00091C45  // sqrt(83) = 9.11043
.word 0x00092A47  // sqrt(84) = 9.16515
.word 0x00093834  // sqrt(85) = 9.21954
.word 0x0009460B  // sqrt(86) = 9.27362
.word 0x000953CF  // sqrt(87) = 9.32738
.word 0x0009617E  // sqrt(88) = 9.38083
.word 0x00096F19  // sqrt(89) = 9.43398
.word 0x00097CA1  // sqrt(90) = 9.48683
.word 0x00098A15  // sqrt(91) = 9.53939
.word 0x00099777  // sqrt(92) = 9.59166
.word 0x0009A4C6  // sqrt(93) = 9.64365
.word 0x0009B203  // sqrt(94) = 9.69536
.word 0x0009BF2D  // sqrt(95) = 9.74679
.word 0x0009CC47  // sqrt(96) = 9.79796
.word 0x0009D94E  // sqrt(97) = 9.84886
.word 0x0009E645  // sqrt(98) = 9.89949
.word 0x0009F32A  // sqrt(99) = 9.94987
.word 0x000A0000  // sqrt(100) = 10.00000
.word 0x000A0CC4  // sqrt(101) = 10.04988
.word 0x000A1979  // sqrt(102) = 10.09950
.word 0x000A261D  // sqrt(103) = 10.14889
.word 0x000A32B2  // sqrt(104) = 10.19804
.word 0x000A3F38  // sqrt(105) = 10.24695
.word 0x000A4BAE  // sqrt(106) = 10.29563
.word 0x000A5815  // sqrt(107) = 10.34408
.word 0x000A646E  // sqrt(108) = 10.39230
.word 0x000A70B7  // sqrt(109) = 10.44031
.word 0x000A7CF3  // sqrt(110) = 10.48809
.word 0x000A8920  // sqrt(111) = 10.53565
.word 0x000A953F  // sqrt(112) = 10.58301
.word 0x000AA151  // sqrt(113) = 10.63015
.word 0x000AAD55  // sqrt(114) = 10.67708
.word 0x000AB94B  // sqrt(115) = 10.72381
.word 0x000AC534  // sqrt(116) = 10.77033
.word 0x000AD110  // sqrt(117) = 10.81665
.word 0x000ADCDF  // sqrt(118) = 10.86278
.word 0x000AE8A1  // sqrt(119) = 10.90871
.word 0x000AF456  // sqrt(120) = 10.95445
.word 0x000B0000  // sqrt(121) = 11.00000
.word 0x000B0B9C  // sqrt(122) = 11.04536
.word 0x000B172D  // sqrt(123) = 11.09054
.word 0x000B22B2  // sqrt(124) = 11.13553
.word 0x000B2E2A  // sqrt(125) = 11.18034
.word 0x000B3997  // sqrt(126) = 11.22497
.word 0x000B44F9  // sqrt(127) = 11.26943
.word 0x000B504F  // sqrt(128) = 11.31371
.word 0x000B5B99  // sqrt(129) = 11.35782
.word 0x000B66D9  // sqrt(130) = 11.40175
.word 0x000B720D  // sqrt(131) = 11.44552
.word 0x000B7D37  // sqrt(132) = 11.48913
.word 0x000B8856  // sqrt(133) = 11.53256
.word 0x000B936A  // sqrt(134) = 11.57584
.word 0x000B9E73  // sqrt(135) = 11.61895
.word 0x000BA972  // sqrt(136) = 11.66190
.word 0x000BB467  // sqrt(137) = 11.70470
.word 0x000BBF51  // sqrt(138) = 11.74734
.word 0x000BCA32  // sqrt(139) = 11.78983
.word 0x000BD508  // sqrt(140) = 11.83216
.word 0x000BDFD4  // sqrt(141) = 11.87434
.word 0x000BEA97  // sqrt(142) = 11.91638
.word 0x000BF550  // sqrt(143) = 11.95826
.word 0x000C0000  // sqrt(144) = 12.00000
.word 0x000C0AA5  // sqrt(145) = 12.04159
.word 0x000C1542  // sqrt(146) = 12.08305
.word 0x000C1FD5  // sqrt(147) = 12.12436
.word 0x000C2A5F  // sqrt(148) = 12.16553
.word 0x000C34E0  // sqrt(149) = 12.20656
.word 0x000C3F58  // sqrt(150) = 12.24745
.word 0x000C49C7  // sqrt(151) = 12.28821
.word 0x000C542E  // sqrt(152) = 12.32883
.word 0x000C5E8B  // sqrt(153) = 12.36932
.word 0x000C68E0  // sqrt(154) = 12.40967
.word 0x000C732C  // sqrt(155) = 12.44990
.word 0x000C7D70  // sqrt(156) = 12.49000
.word 0x000C87AB  // sqrt(157) = 12.52996
.word 0x000C91DE  // sqrt(158) = 12.56981
.word 0x000C9C09  // sqrt(159) = 12.60952
.word 0x000CA62C  // sqrt(160) = 12.64911
.word 0x000CB046  // sqrt(161) = 12.68858
.word 0x000CBA59  // sqrt(162) = 12.72792
.word 0x000CC463  // sqrt(163) = 12.76715
.word 0x000CCE66  // sqrt(164) = 12.80625
.word 0x000CD861  // sqrt(165) = 12.84523
.word 0x000CE254  // sqrt(166) = 12.88410
.word 0x000CEC3F  // sqrt(167) = 12.92285
.word 0x000CF623  // sqrt(168) = 12.96148
.word 0x000D0000  // sqrt(169) = 13.00000
.word 0x000D09D4  // sqrt(170) = 13.03840
.word 0x000D13A2  // sqrt(171) = 13.07670
.word 0x000D1D68  // sqrt(172) = 13.11488
.word 0x000D2727  // sqrt(173) = 13.15295
.word 0x000D30DF  // sqrt(174) = 13.19091
.word 0x000D3A8F  // sqrt(175) = 13.22876
.word 0x000D4439  // sqrt(176) = 13.26650
.word 0x000D4DDB  // sqrt(177) = 13.30413
.word 0x000D5777  // sqrt(178) = 13.34166
.word 0x000D610B  // sqrt(179) = 13.37909
.word 0x000D6A99  // sqrt(180) = 13.41641
.word 0x000D7420  // sqrt(181) = 13.45362
.word 0x000D7DA0  // sqrt(182) = 13.49074
.word 0x000D871A  // sqrt(183) = 13.52775
.word 0x000D908D  // sqrt(184) = 13.56466
.word 0x000D99F9  // sqrt(185) = 13.60147
.word 0x000DA35F  // sqrt(186) = 13.63818
.word 0x000DACBF  // sqrt(187) = 13.67479
.word 0x000DB618  // sqrt(188) = 13.71131
.word 0x000DBF6B  // sqrt(189) = 13.74773
.word 0x000DC8B7  // sqrt(190) = 13.78405
.word 0x000DD1FD  // sqrt(191) = 13.82027
.word 0x000DDB3D  // sqrt(192) = 13.85641
.word 0x000DE477  // sqrt(193) = 13.89244
.word 0x000DEDAA  // sqrt(194) = 13.92839
.word 0x000DF6D8  // sqrt(195) = 13.96424
.word 0x000E0000  // sqrt(196) = 14.00000
.word 0x000E0921  // sqrt(197) = 14.03567
.word 0x000E123D  // sqrt(198) = 14.07125
.word 0x000E1B53  // sqrt(199) = 14.10674
.word 0x000E2463  // sqrt(200) = 14.14214
.word 0x000E2D6D  // sqrt(201) = 14.17745
.word 0x000E3671  // sqrt(202) = 14.21267
.word 0x000E3F70  // sqrt(203) = 14.24781
.word 0x000E4869  // sqrt(204) = 14.28286
.word 0x000E515C  // sqrt(205) = 14.31782
.word 0x000E5A4A  // sqrt(206) = 14.35270
.word 0x000E6332  // sqrt(207) = 14.38749
.word 0x000E6C15  // sqrt(208) = 14.42221
.word 0x000E74F2  // sqrt(209) = 14.45683
.word 0x000E7DCA  // sqrt(210) = 14.49138
.word 0x000E869D  // sqrt(211) = 14.52584
.word 0x000E8F6A  // sqrt(212) = 14.56022
.word 0x000E9832  // sqrt(213) = 14.59452
.word 0x000EA0F5  // sqrt(214) = 14.62874
.word 0x000EA9B2  // sqrt(215) = 14.66288
.word 0x000EB26A  // sqrt(216) = 14.69694
.word 0x000EBB1D  // sqrt(217) = 14.73092
.word 0x000EC3CB  // sqrt(218) = 14.76482
.word 0x000ECC74  // sqrt(219) = 14.79865
.word 0x000ED517  // sqrt(220) = 14.83240
.word 0x000EDDB6  // sqrt(221) = 14.86607
.word 0x000EE650  // sqrt(222) = 14.89966
.word 0x000EEEE5  // sqrt(223) = 14.93318
.word 0x000EF775  // sqrt(224) = 14.96663
.word 0x000F0000  // sqrt(225) = 15.00000
.word 0x000F0886  // sqrt(226) = 15.03330
.word 0x000F1107  // sqrt(227) = 15.06652
.word 0x000F1983  // sqrt(228) = 15.09967
.word 0x000F21FB  // sqrt(229) = 15.13275
.word 0x000F2A6E  // sqrt(230) = 15.16575
.word 0x000F32DC  // sqrt(231) = 15.19868
.word 0x000F3B46  // sqrt(232) = 15.23155
.word 0x000F43AB  // sqrt(233) = 15.26434
.word 0x000F4C0C  // sqrt(234) = 15.29706
.word 0x000F5467  // sqrt(235) = 15.32971
.word 0x000F5CBF  // sqrt(236) = 15.36229
.word 0x000F6511  // sqrt(237) = 15.39480
.word 0x000F6D60  // sqrt(238) = 15.42725
.word 0x000F75A9  // sqrt(239) = 15.45962
.word 0x000F7DEF  // sqrt(240) = 15.49193
.word 0x000F8630  // sqrt(241) = 15.52417
.word 0x000F8E6C  // sqrt(242) = 15.55635
.word 0x000F96A5  // sqrt(243) = 15.58846
.word 0x000F9ED9  // sqrt(244) = 15.62050
.word 0x000FA708  // sqrt(245) = 15.65248
.word 0x000FAF33  // sqrt(246) = 15.68439
.word 0x000FB75B  // sqrt(247) = 15.71623
.word 0x000FBF7D  // sqrt(248) = 15.74802
.word 0x000FC79C  // sqrt(249) = 15.77973
.word 0x000FCFB7  // sqrt(250) = 15.81139
.word 0x000FD7CD  // sqrt(251) = 15.84298
.word 0x000FDFDF  // sqrt(252) = 15.87451
.word 0x000FE7ED  // sqrt(253) = 15.90597
.word 0x000FEFF7  // sqrt(254) = 15.93738
.word 0x000FF7FD  // sqrt(255) = 15.96872
.word 0x00100000  // sqrt(256) = 16.00000

