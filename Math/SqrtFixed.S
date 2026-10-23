
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
  * @description  accuracy is about 1.5e-5, i.e. 1LSB
  *
  *******************************************************************************************
 */

.syntax unified
.cpu cortex-m3
.thumb

.global sqrtFixed

.type sqrtFixed, %function
.align 4
sqrtFixed:
	push { r1-r9, lr }
	// Check if zero, don't calculate
	cmp r0, #0
	beq sqrtFixedEnd
	// **************************************************
	// Calculate linear approxymation
	// **************************************************
	clz r7, r0
	add r6, r7, #1
	rsb r8, r7, #(34)
	lsl r9, r0, r6			// R9 <= X << (ToMaxVal+1)
	tst r8, #1
	ittee eq
	ldreq r6, =0xb504f333	// Y0_32bit
	ldreq r5, =0x4afb0ccc	// dY_32bit
	ldrne r6, =0x80000000   // Y0_31bit
	ldrne r5, =0x3504f333   // dY_31bit
	umull r4, r5, r9, r5	// R5 <= (dY * X - X0) << MAX
	add r4, r5, r6			// R4 now is linear approxymation of SQRT(x) << MAX
	lsr r6, r7, #1			// Calculate result shift	Shift = 16 - MaxBitCount / 2
	lsr r6, r4, r6			// R6 <= Result of linear approxymation

	// **************************************************
	// Calculate quad correction
	// **************************************************
	// Calculate X-shift offset parable
	mov r4, r9
	tst r4, #0x80000000
	it eq
	rsbeq r4, r4, #0xFFFFFFFF
	sub r4, r4, #0x80000000
	lsl r4, r4, #1
	umull r4, r5, r4, r4
	rsb r4, r5, #0xFFFFFFFF
	// Get X-shift scaled parable value
	ldr r5, =#0x57d86660	// Maximum X correction shifted by 4 bit's
	umull r4, r5, r4, r5	// R5 <= X correction shifted by 4bit's and by max bits
	lsr r4, r5, r7			// R4 <= X correction shifted by 4bit's
	lsr r4, r4, #4			// R4 <= X correction value
	// Shift X value, to get max sacled parable
	add r4, r4, r0			// R4 <= X + Xcorrection
	add r5, r7, #1
	lsl r4, r4, r5			// R4 <= (X + Xcorrection) << (ToMaxVal+1)
	// Now, X-corection applied
	// Calculate Y correction parable, based on corrected X-value
	tst r4, #0x80000000
	it eq
	rsbeq r4, r4, #0xFFFFFFFF
	sub r4, r4, #0x80000000
	lsl r4, r4, #1
	umull r4, r5, r4, r4
	rsb r4, r5, #0xFFFFFFFF
	// Get Y-scale value, depend on sqrt(2) or (2) is scale factor
	tst r8, #0x01
	ite eq
	ldreq r5, =#1726663841	// CorrY * 2 << N
	ldrne r5, =#1220935711	// CorrY * sqrt(2) << N
	umull r4, r5, r4, r5
	lsr r4, r7, #1
	lsr r4, r5, r4
	lsr r4, r4, #5
	add r6, r4, r6			// R6 <= Result of quad approxymation

	// **************************************************
	// Calculate double parable correction
	// **************************************************
	// R9 is now not corrected, but scaled parable value
	ldr r5, = #0x7504f333	// X-value, where is maximum of quad error correction
	cmp r9, r5				// Xcorrection central point
	ittee ls
	subls r4, r5, r9		// R4 <= value to calculate parable
	ldrls r5, =#0x8c02d41d	// R5 <= Scale of parable, left side
	subhi r4, r9, r5		// R4 <= value to calculate parable
	ldrhi r5, =#0x75e30c0c	// R5 <= Scale of parable, right side
	umull r4, r5, r4, r5	// R5 <= Scaled parable argument value (shifted by 2)
	lsl r4, r5, #2			// R4 <= Scaled parable argument value
	// So, now I can calculate parable value
	umull r4, r5, r4, r4	// R5 <= parable value
	rsb r4, r5, #0xFFFFFFFF	// R4 <= inverse parable value of X-value
	// So, now I have to calculate Y-parable, based on X-value
	// But it's the same as before
	tst r4, #0x80000000
	it eq
	rsbeq r4, r4, #0xFFFFFFFF
	sub r4, r4, #0x80000000
	lsl r4, r4, #1
	umull r4, r5, r4, r4
	rsb r4, r5, #0xFFFFFFFF
	// So, now I have to multiply by Scale factor.
	and r7, r8, #1	// R7 <= index of Factor << N, or Factor * sqrt(2) << N
	ldr r5, =#0x7504f333
	cmp r9, r5
	ite hi
	addhi r5, r7, #2
	movls r5, r7
	ldr r7, =sqrtDoubleParableScale
	ldr r5, [ r7, r5, lsl 2 ]
	umull r4, r5, r4, r5
	// Scale correction factor
	rsb r7, r8, #32
	lsr r7, r7, #1
	lsr r4, r5, r7
	// Add correction to the value
	add r6, r6, r4

	// Here is ~101 CPU clock on Cortex M7
	//********************************
	// ++++++++Commented block++++++++
	//mov r0, r6
	//pop { r1-r9, lr }
	//bx lr
	// --------Commented Block -------
	//********************************

	// SQRT iterative correction
	ldr r4, =sqrtMaxErrorPowArray
	ldrb r4, [r4, r8]
	mov r7, #1
	lsl r7, r7, r4

sqrtCorrLoop:
	cmp r7, #0
	beq sqrtEndCorrLoop			// Skip if nothing to do
	umull r4, r5, r6, r6
	rsbs r8, r4, #0
	bne sqrtMulLpNotZero
	sbcs r8, r0, r5
	beq sqrtEndCorrLoop
	rsbs r8, r4, #0
sqrtMulLpNotZero:
	sbcs r8, r0, r5
	it cc
	subcc r6, r6, r7
	bcc sqrtSkipSum
	rsb r4, r7, #0xFFFFFFFF
	cmp r4, r6
	ite hi
	addhi r6, r6, r7
	ldrls r6, =#0xFFFFFFFF
sqrtSkipSum:
	lsrs r7, r7, #1
	b sqrtCorrLoop
sqrtEndCorrLoop:
	mov r0, r6
sqrtFixedEnd:
	pop { r1-r9, lr }
	bx lr

.align 4
sqrtDoubleParableScale:
.word 0x00064b12 	// L
.word 0x00047333	// L
.word 0x0005d399 	// R
.word 0x00041eb8	// R

.align 4
sqrtMaxErrorPowArray:
.byte 0x00 // err = 0.0000, bitCount=0
.byte 0x00 // err = 0.0000, bitCount=1
.byte 0x07 // err = 0.0022, bitCount=2
.byte 0x07 // err = 0.0034, bitCount=3
.byte 0x08 // err = 0.0048, bitCount=4
.byte 0x08 // err = 0.0069, bitCount=5
.byte 0x08 // err = 0.0068, bitCount=6
.byte 0x08 // err = 0.0056, bitCount=7
.byte 0x08 // err = 0.0039, bitCount=8
.byte 0x07 // err = 0.0029, bitCount=9
.byte 0x07 // err = 0.0022, bitCount=10
.byte 0x06 // err = 0.0016, bitCount=11
.byte 0x06 // err = 0.0012, bitCount=12
.byte 0x06 // err = 0.0010, bitCount=13
.byte 0x05 // err = 0.0009, bitCount=14
.byte 0x05 // err = 0.0009, bitCount=15
.byte 0x06 // err = 0.0011, bitCount=16
.byte 0x06 // err = 0.0013, bitCount=17
.byte 0x06 // err = 0.0017, bitCount=18
.byte 0x07 // err = 0.0023, bitCount=19
.byte 0x07 // err = 0.0033, bitCount=20
.byte 0x08 // err = 0.0046, bitCount=21
.byte 0x08 // err = 0.0065, bitCount=22
.byte 0x09 // err = 0.0092, bitCount=23
.byte 0x09 // err = 0.0130, bitCount=24
.byte 0x0A // err = 0.0184, bitCount=25
.byte 0x0A // err = 0.0260, bitCount=26
.byte 0x0B // err = 0.0367, bitCount=27
.byte 0x0B // err = 0.0519, bitCount=28
.byte 0x0C // err = 0.0733, bitCount=29
.byte 0x0C // err = 0.1037, bitCount=30
.byte 0x0C
.byte 0x0D




.global sqrtFixedSin
.type sqrtFixedSin, %function
.align 4
sqrtFixedSin:
    push { r1-r9, lr }

    // Проверка аргумента на ноль (корень из 0 = 0)
    cmp r0, #0
    beq sqrtFixedSin_End

    // *****************************************************************
    // БЛОК 1: Базовая кусочно-линейная аппроксимация (linSqrt)
    // *****************************************************************
    clz r7, r0                  // r7 <= clz(x)
    add r6, r7, #1              // r6 <= clz(x) + 1
    rsb r8, r7, #32             // r8 <= bitCount (34 - clz)
    lsl r9, r0, r6              // r9 <= Xcorr (нормализованный аргумент)

    tst r8, #1                  // Проверка четности старшего бита
    ittee eq
    ldreq r6, =0xb504f333       // Y0 для чётных степеней (sqrt(2))
    ldreq r5, =0x4afb0ccc       // dY для чётных степеней   (1257966796 (2515933592 ÷ 2) в HEX равен 4AFB0CCC)
    ldrne r6, =0x80000000       // Y0 для нечётных степеней (1.0)
    ldrne r5, =0x3504F333       // dY для нечётных степеней (1779033703 (3558067406 ÷ 2) в HEX равен 6A09E667/2=3504F333)

    umull r1, r4, r9, r5        // r4 <= (dY * Xcorr) >> 32
    add r4, r4, r6              // r4 <= линейная аппроксимация в масштабе Q32

    lsr r3, r7, #1              // Вычисляем сдвиг мантиссы: clz / 2
    lsr r6, r4, r3              // r6 <= linSqrt(x) (базовый корень в формате Q16.16)

    // Если Xcorr равен 0, то коррекция фазы не требуется
    cmp r9, #0
    beq sqrtFixedSin_FastEnd

    // *****************************************************************
    // БЛОК 2: Генерация степеней Xcorr для полинома фазы
    // *****************************************************************
    umull r1, r4, r9, r9        // r4 <= QXcorr = (Xcorr * Xcorr) >> 32
    umull r1, r2, r4, r9        // r2 <= TXcorr = (QXcorr * Xcorr) >> 32
    umull r1, r3, r2, r9        // r3 <= FXcorr = (TXcorr * Xcorr) >> 32

    // *****************************************************************
    // БЛОК 3: Сборка полинома фазы Xnew
    // *****************************************************************
    mov r0, 0x80000000
    sub r0, r0, r9		        // r0 <= -Xcorr

    // Добавляем QXcorr * QK / 2^31
    ldr r1, =2191184939         // QK
    umull r1, r4, r4, r1        // r4 <= (QXcorr * QK) >> 32
    add r0, r0, r4, lsl #1

    // Добавляем TXcorr * TK / 2^32
    ldr r1, =2061584301         // TK
    umull r1, r4, r2, r1        // r4 <= (TXcorr * TK) >> 32
    add r0, r0, r4, lsr #1

    // Вычитаем FXcorr * FK / 2^32
    ldr r1, =2233382993         // FK
    umull r1, r4, r3, r1        // r4 <= (FXcorr * FK) >> 32
    sub r4, r0, r4, lsr #1

    // *****************************************************************
    // БЛОК 4: Вызов Вашего rSinFixed(angle, radius)
    // *****************************************************************
    // Передаем параметры строго по вашему ABI
    lsr r3, r7, #1              // Вычисляем сдвиг мантиссы: clz / 2
    ldr r1, =2211908156         // r1 <= константный Scale из SMath
    umull r0, r1, r1, r5		// r1 <= масштабируем на R5
    lsr r1, r1, #3
    lsr r1, r1, r3              // r3 содержит сохраненный сдвиг (clz / 2)
    mov r0, r4              	// r0 <= angle = Xnew
    bl rSinFixed                // Вызов функции. На выходе r0 <= знаковый ec(x)

	// Смещение
	mov r1, #20
	lsl r1, r1, #16
    umull r2, r4, r1, r5		// r1 <= масштабируем на R5
    lsr r1, r4, r3              // r3 содержит сохраненный сдвиг (clz / 2)
	sub r0, r0, r1
    // Финальное смещение мантиссы по сетке clz/2 и сумма
    add r0, r6, r0              // Итоговый результат: linSqrt + коррекция
sqrtFixedSin_End:
    pop { r1-r9, lr }
    bx lr

sqrtFixedSin_FastEnd:
    mov r0, r6                  // Если поправка нулевая, возвращаем linSqrt
    pop { r1-r9, lr }
    bx lr
