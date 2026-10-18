
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
	// R0 <= Q (Q31)  I.e Im
	// R1 <= I (Q31), I.e Re
    push {r4-r9, lr}

	// Check if zero
    orr r4, r0, r1
    cmp r4, #0
    beq atan2Fixed_exit_zero

	// Check if zero value of divisor
    cmp r1, #0
    it eq
    moveq r1, #1

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

	//bkpt
    // Get maximum count of leading zeros
    cmp r4, r5
    ite hi
    clzhi r8, r4
    clzls r8, r5
    // Rescale values
    // R4 <= Q0.32 << MAX_SHIFT
	lsl r4, r4, r8
    // R5 <= I0.32 << MAX_SHIFT
	lsl r5, r5, r8

	// NOW: R4 = abs(Q) abs(Im(S)), abs(y), shifted to get Max
	// NOW: R5 = abs(I) abs(Re(S)), abs(x), shifted to get Max
	cmp r4, r5
	bls atan2Fixed_lower

	/********************************/
	/**							   **/
	/********************************/
	// Calc PI/4 - Re/(Im + 1)
	lsr r6, r4, #16
	add r6, r6, #1
	// Dvide
	udiv r7, r5, r6
	// Rescale
	lsl r7, r7, #13
	// Limit range
	movw r6, #0x0000
	movt r6, #0x4000
	cmp r7, r6
	it hi
	movhi r7, r6
	// Reverse value
	sub r7, r6, r7


	// --- Точный расчет CorrB(PhaseB_Linear) без переполнения ---
	// CorrXB = abs(PhaseB_Linear - (2^29 + 2^28)) -> 0x30000000
	mov     r8, #0x30000000
	sub     r8, r7, r8
	cmp     r8, #0
	it      lt
	neglt   r8, r8              // r8 = CorrXB

	// Считаем (CorrXB * CorrXB) / 2^30
	umull   r2, r3, r8, r8      // r3:r2 = CorrXB^2
	lsl     r3, r3, #2
	orr     r3, r3, r2, lsr #30 // r3 = (CorrXB^2) / 2^30

	// Вычисляем параболу: 2^26 - r3
	mov     r2, #0x04000000
	sub     r3, r2, r3

	// Умножаем на 0.69 без переполнения
	movw    r2, #45220
	umull   r2, r9, r3, r2
	lsl     r9, r9, #16
	orr     r3, r9, r2, lsr #16 // r3 = Чистая поправка CorrB

	// Финал ветви B: вычитаем поправку из линейного значения
	sub r7, r7, r3


	// NOW:
	// R7 = PI/4 - Re/(Im + 1)
	// Sign table
	ubfx r4, r1, #31, #1
	ubfx r5, r0, #31, #1
	bfi r4, r5, #1, #1
	// Tbb
  	ldr r5, =#atanFixedTBBTable_0
  	tbb [ r5, r4 ]

atan2Fixed_q1_0:
	//bkpt
	mov r0, r7
    pop {r4-r9, lr}
    bx lr

atan2Fixed_q2_0:
	movw r6, #0x0000
	movt r6, #0x8000
	rsb r0, r7, r6
    pop {r4-r9, lr}
    bx lr

atan2Fixed_q3_0:
	movw r6, #0x0000
	movt r6, #0x8000
	add r0, r7, r6
    pop {r4-r9, lr}
    bx lr

atan2Fixed_q4_0:
	movw r6, #0xFFFF
	movt r6, #0xFFFF
	rsb r0, r7, r6
    pop {r4-r9, lr}
    bx lr

	/********************************/
	/**							   **/
	/********************************/
atan2Fixed_lower:
	// Calc Im/(Re + 1)
	lsr r6, r5, #16
	add r6, r6, #1
	// Dvide
	udiv r8, r4, r6
	// Rescale
	lsl r8, r8, #13
	// Limit range
	movw r6, #0x0000
	movt r6, #0x4000
	cmp r8, r6
	it hi
	movhi r8, r6
	// NOW:
	// R8 = Im/(Re + 1)
	mov r7, r8


	// --- Точный расчет CorrA(PhaseA_Linear) без переполнения ---
	// CorrXA = abs(PhaseA_Linear - 2^28) -> 2^28 = 0x10000000
	mov     r9, #0x10000000
	sub     r9, r8, r9
	cmp     r9, #0
	it      lt
	neglt   r9, r9              // r9 = CorrXA

	// Считаем (CorrXA * CorrXA) / 2^30
	umull   r2, r3, r9, r9      // r3:r2 = CorrXA^2 (64 бита)
	lsl     r3, r3, #2
	orr     r3, r3, r2, lsr #30 // r3 = (CorrXA^2) / 2^30

	// Вычисляем параболу: 2^26 - r3 -> 2^26 = 0x04000000
	mov     r2, #0x04000000
	sub     r3, r2, r3

	// Умножаем на 0.69 через длинное умножение (умножаем на 45220)
	movw    r2, #45220
	umull   r2, r9, r3, r2      // r9:r2 = r3 * 45220 (64 бита! Переполнения нет)

	// r9 содержит старшие 32 бита произведения.
	// Так как мы умножали на число в формате Q16 (45220 / 65536),
	// то для деления на 65536 нам нужно сдвинуть 64-битный результат вправо на 16 бит.
	// Собираем результат сдвига в r3:
	lsl     r9, r9, #16
	orr     r3, r9, r2, lsr #16 // r3 = Чистая поправка CorrA в твоем масштабе угла!

	// Финал ветви A: добавляем реальную поправку к линейному значению
	add     r7, r8, r3


	// Sign table
	ubfx r4, r1, #31, #1
	ubfx r5, r0, #31, #1
	bfi r4, r5, #1, #1
	// Tbb
  	ldr r5, =#atanFixedTBBTable_1
  	tbb [ r5, r4 ]

atan2Fixed_q1_1:
	//bkpt
	mov r0, r7
    pop {r4-r9, lr}
    bx lr

atan2Fixed_q2_1:
	movw r6, #0x0000
	movt r6, #0x8000
	rsb r0, r7, r6
    pop {r4-r9, lr}
    bx lr

atan2Fixed_q3_1:
	movw r6, #0x0000
	movt r6, #0x8000
	add r0, r7, r6
    pop {r4-r9, lr}
    bx lr

atan2Fixed_q4_1:
	movw r6, #0xFFFF
	movt r6, #0xFFFF
	rsb r0, r7, r6
    pop {r4-r9, lr}
    bx lr

atan2Fixed_exit_zero:
    mov r0, #0
    pop {r4-r9, lr}
    bx lr


.type atan2Fixed_q1_0, %function
.type atan2Fixed_q2_0, %function
.type atan2Fixed_q3_0, %function
.type atan2Fixed_q4_0, %function

.align 4
atanFixedTBBTable_0:
.byte ((atan2Fixed_q1_0 - atan2Fixed_q1_0 ) / 2)
.byte ((atan2Fixed_q2_0 - atan2Fixed_q1_0 ) / 2)
.byte ((atan2Fixed_q4_0 - atan2Fixed_q1_0 ) / 2)
.byte ((atan2Fixed_q3_0 - atan2Fixed_q1_0 ) / 2)


.type atan2Fixed_q1_1, %function
.type atan2Fixed_q2_1, %function
.type atan2Fixed_q3_1, %function
.type atan2Fixed_q4_1, %function

.align 4
atanFixedTBBTable_1:
.byte ((atan2Fixed_q1_1 - atan2Fixed_q1_1 ) / 2)
.byte ((atan2Fixed_q2_1 - atan2Fixed_q1_1 ) / 2)
.byte ((atan2Fixed_q4_1 - atan2Fixed_q1_1 ) / 2)
.byte ((atan2Fixed_q3_1 - atan2Fixed_q1_1 ) / 2)
	
	
