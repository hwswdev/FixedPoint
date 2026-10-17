
/*
  ********************************************************************************************
  * @file      Atan2Fixed.s dedicated to STM32Fxx device
  * @author    Evgeny Sobolev
  * @version   V1.0.0
  * @date      2026-10-17
*/


.syntax unified
.cpu cortex-m3
.thumb

.global atan2Fixed
.type atan2Fixed, %function
.align 4
atan2Fixed:
	// R0 <= Q (Q31)
	// R1 <= I (Q31)
    push {r4-r9, lr}
    cmp r0, #0
    beq atan2Fixed_exit_zero
    cmp r1, #0
    beq atan2Fixed_exit_zero

	// Rescale arg0 value
    tst r0, #0x80000000
    ite ne
    negsne r4, r0
    moveq r4, r0

	// Rescale arg1 value
    tst r1, #0x80000000
    ite ne
    negsne r5, r1
    moveq r5, r1

    // Get maximum count of leading zeros
    cmp r4, r5
    ite hi
    movhi r7, r4
    movls r7, r5
    clz r8, r7

    // Rescale values
    // R4 <= Q0.32 << MAX_SHIFT
	lsl r4, r4, r8
    // R5 <= I0.32 << MAX_SHIFT
	lsl r5, r5, r8

	// Divide Q by 2^16
	lsr r4, r4, #16

	// Append linearization
	movw r6, #0x0000
	movt r6, #0x0001
	add r4, r4, r6

	// Divide
	udiv r7, r5, r4

	// Scale result
	lsl r7, r7, #16

	// Substruct to inverse line
	movw r6, #0x0000
	movt r6, #0x8000
	sub r7, r6, r7

	// Scale to axis PI/4*1.15533654773
	// 0xE84C626E
	movw r6, #0x626E
	movt r6, #0xE84C

	// R0 <= R7*PI/4
	umull r4, r5, r6, r7

	// Scale result to 0..PI/4 interval
	lsl r6, r5, #1
	orr r6, r6, r4, lsr (32-1)

	// So change sign
	cmp r0, #0
	ite mi
	negsmi r0, r6
	movpl  r0, r6

	cmp r1, #0
	it mi
	negsmi r0, r0

atan2Fixed_exit:
    pop {r4-r9, lr}
    bx lr

atan2Fixed_exit_zero:
    mov r0, #0
    pop {r4-r9, lr}
    bx lr

	
	
