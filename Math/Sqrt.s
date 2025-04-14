
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
	cmp r0, #256
	bls sqrt_0_256

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
	it cs
	ldrcs r2, =#0xFFFFFFFF
	mov r0, r2

	// Correcrt sqrt value using itaration
	mov r0, r8
	sub r1, r9, #1
	bl sqrt_iter_correction_calc

	pop {r1-r9, lr}
	bx lr

sqrt_0_256:
	ldr r2, =#sqrt_0_256_table
	ldr r0, [r2, r0, lsl #2 ]
	pop {r1-r9, lr}
	bx lr


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

	mul r6, r5, r5			// R6 <= ( Corrected(X) ^ 2 )
	rsb r0, r6, #0xFFFFFFFF	// R0 <= 1 - (Corrected(X)^2)

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


.type sqrt_iter_correction_calc, %function
.align 4
sqrt_iter_correction_calc:
	// So, I have to think how to make this method.
	// I think it I have error here.
	push {r4-r8, lr}
	// Error correction
	cmp r1, #7				// Skip values lower 256
	bls skipIterations
	// Get
	tst r1, #1
	ite ne
	ldrne r7, =#0x000EEE1D // 14.930140 << 16 (on 2^33, i.e 7.xxx * 2 )
	ldreq r7, =#0x000A8EA7 // 10.557250 << 16
	lsr r6, r1, #1
	rsb r6, r6, #16
	lsr r7, r7, r6

nextItaration:
	umull r5, r6, r2, r2
	rsbs r5, r5, #0
	bne lowerPartNotZero
	cmp r6, r0
	beq skipIterations4bit
	rsb  r5, r5, #0
	rsbs r5, r5, #0
lowerPartNotZero:
	sbcs r6, r0, r6
	ite cc
	subcc r2, r2, r7
	addcs r2, r2, r7
	lsr r7, r7, #1
	cmp r7, #0
	beq skipIterations
	b nextItaration

skipIterations:

	// I think, I have to think. !!!!!!!!!!
	// Something went wrong on realization, because of error about 4bit exists
	// I don't know is it faster or slower then float sqrt(x)
	// But initial approximation is good
	// It is tempory FixUP maximum error value of (12 / (2^16) )
	mov r7, #0x8
nextItaration4bit:
	umull r5, r6, r2, r2
	rsbs r5, r5, #0
	bne lowerPartNotZero4bit
	cmp r6, r0
	beq skipIterations4bit
	rsb  r5, r5, #0
	rsbs r5, r5, #0
lowerPartNotZero4bit:
	sbcs r6, r0, r6
	ite cc
	subcc r2, r2, r7
	addcs r2, r2, r7
	lsr r7, r7, #1
	cmp r7, #0
	beq skipIterations4bit
	b nextItaration4bit

skipIterations4bit:
	mov r0, r2
	pop {r4-r8, lr}
	bx lr


.align 4
sqrt_0_256_table:
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
.word 0x00041F83 // sqrt(17) = 4.123093, err = -0.000013
.word 0x00043E1D // sqrt(18) = 4.242630, err = -0.000011
.word 0x00045BE0 // sqrt(19) = 4.358887, err = -0.000012
.word 0x000478DD // sqrt(20) = 4.472122, err = -0.000014
.word 0x00049523 // sqrt(21) = 4.582565, err = -0.000010
.word 0x0004B0BF // sqrt(22) = 4.690414, err = -0.000001
.word 0x0004CBBB // sqrt(23) = 4.795822, err = -0.000009
.word 0x0004E623 // sqrt(24) = 4.898972, err = -0.000008
.word 0x00050000 // sqrt(25) = 5.000000, err = 0.000000
.word 0x00051959 // sqrt(26) = 5.099014, err = -0.000005
.word 0x00053237 // sqrt(27) = 5.196152, err = -0.000001
.word 0x00054A9F // sqrt(28) = 5.291489, err = -0.000014
.word 0x0005629A // sqrt(29) = 5.385162, err = -0.000002
.word 0x00057A2B // sqrt(30) = 5.477219, err = -0.000007
.word 0x00059159 // sqrt(31) = 5.567764, err = -0.000000
.word 0x0005A827 // sqrt(32) = 5.656845, err = -0.000009
.word 0x0005BE9B // sqrt(33) = 5.744553, err = -0.000010
.word 0x0005D4B9 // sqrt(34) = 5.830948, err = -0.000004
.word 0x0005EA84 // sqrt(35) = 5.916077, err = -0.000003
.word 0x00060000 // sqrt(36) = 6.000000, err = 0.000000
.word 0x0006152F // sqrt(37) = 6.082748, err = -0.000014
.word 0x00062A17 // sqrt(38) = 6.164413, err = -0.000001
.word 0x00063EB8 // sqrt(39) = 6.244995, err = -0.000003
.word 0x00065316 // sqrt(40) = 6.324554, err = -0.000001
.word 0x00066733 // sqrt(41) = 6.403122, err = -0.000002
.word 0x00067B11 // sqrt(42) = 6.480728, err = -0.000013
.word 0x00068EB4 // sqrt(43) = 6.557434, err = -0.000004
.word 0x0006A21C // sqrt(44) = 6.633240, err = -0.000010
.word 0x0006B54C // sqrt(45) = 6.708191, err = -0.000013
.word 0x0006C846 // sqrt(46) = 6.782318, err = -0.000012
.word 0x0006DB0C // sqrt(47) = 6.855652, err = -0.000003
.word 0x0006ED9E // sqrt(48) = 6.928192, err = -0.000011
.word 0x00070000 // sqrt(49) = 7.000000, err = 0.000000
.word 0x00071231 // sqrt(50) = 7.071060, err = -0.000008
.word 0x00072434 // sqrt(51) = 7.141418, err = -0.000010
.word 0x0007360A // sqrt(52) = 7.211090, err = -0.000012
.word 0x000747B5 // sqrt(53) = 7.280106, err = -0.000004
.word 0x00075935 // sqrt(54) = 7.348465, err = -0.000004
.word 0x00076A8B // sqrt(55) = 7.416183, err = -0.000015
.word 0x00077BBA // sqrt(56) = 7.483307, err = -0.000008
.word 0x00078CC1 // sqrt(57) = 7.549820, err = -0.000014
.word 0x00079DA3 // sqrt(58) = 7.615768, err = -0.000005
.word 0x0007AE5F // sqrt(59) = 7.681137, err = -0.000009
.word 0x0007BEF7 // sqrt(60) = 7.745956, err = -0.000010
.word 0x0007CF6C // sqrt(61) = 7.810242, err = -0.000008
.word 0x0007DFBE // sqrt(62) = 7.873993, err = -0.000015
.word 0x0007EFEF // sqrt(63) = 7.937241, err = -0.000013
.word 0x00080000 // sqrt(64) = 8.000000, err = 0.000000
.word 0x00080FF0 // sqrt(65) = 8.062256, err = -0.000002
.word 0x00081FC0 // sqrt(66) = 8.124023, err = -0.000015
.word 0x00082F73 // sqrt(67) = 8.185349, err = -0.000004
.word 0x00083F07 // sqrt(68) = 8.246201, err = -0.000011
.word 0x00084E7E // sqrt(69) = 8.306610, err = -0.000014
.word 0x00085DD9 // sqrt(70) = 8.366592, err = -0.000008
.word 0x00086D18 // sqrt(71) = 8.426147, err = -0.000002
.word 0x00087C3B // sqrt(72) = 8.485275, err = -0.000006
.word 0x00088B43 // sqrt(73) = 8.543991, err = -0.000013
.word 0x00089A31 // sqrt(74) = 8.602310, err = -0.000015
.word 0x0008A906 // sqrt(75) = 8.660248, err = -0.000006
.word 0x0008B7C1 // sqrt(76) = 8.717789, err = -0.000009
.word 0x0008C664 // sqrt(77) = 8.774963, err = -0.000001
.word 0x0008D4EE // sqrt(78) = 8.831757, err = -0.000004
.word 0x0008E360 // sqrt(79) = 8.888184, err = -0.000011
.word 0x0008F1BB // sqrt(80) = 8.944260, err = -0.000012
.word 0x00090000 // sqrt(81) = 9.000000, err = 0.000000
.word 0x00090E2D // sqrt(82) = 9.055374, err = -0.000011
.word 0x00091C45 // sqrt(83) = 9.110428, err = -0.000006
.word 0x00092A47 // sqrt(84) = 9.165146, err = -0.000006
.word 0x00093834 // sqrt(85) = 9.219543, err = -0.000001
.word 0x0009460B // sqrt(86) = 9.273605, err = -0.000013
.word 0x000953CF // sqrt(87) = 9.327377, err = -0.000002
.word 0x0009617E // sqrt(88) = 9.380829, err = -0.000003
.word 0x00096F19 // sqrt(89) = 9.433975, err = -0.000006
.word 0x00097CA1 // sqrt(90) = 9.486832, err = -0.000001
.word 0x00098A15 // sqrt(91) = 9.539383, err = -0.000009
.word 0x00099777 // sqrt(92) = 9.591660, err = -0.000004
.word 0x0009A4C6 // sqrt(93) = 9.643646, err = -0.000005
.word 0x0009B203 // sqrt(94) = 9.695358, err = -0.000001
.word 0x0009BF2D // sqrt(95) = 9.746780, err = -0.000014
.word 0x0009CC47 // sqrt(96) = 9.797958, err = -0.000001
.word 0x0009D94E // sqrt(97) = 9.848846, err = -0.000011
.word 0x0009E645 // sqrt(98) = 9.899490, err = -0.000005
.word 0x0009F32A // sqrt(99) = 9.949860, err = -0.000015
.word 0x000A0000 // sqrt(100) = 10.000000, err = 0.000000
.word 0x000A0CC4 // sqrt(101) = 10.049866, err = -0.000010
.word 0x000A1979 // sqrt(102) = 10.099503, err = -0.000002
.word 0x000A261D // sqrt(103) = 10.148880, err = -0.000012
.word 0x000A32B2 // sqrt(104) = 10.198029, err = -0.000010
.word 0x000A3F38 // sqrt(105) = 10.246948, err = -0.000003
.word 0x000A4BAE // sqrt(106) = 10.295624, err = -0.000006
.word 0x000A5815 // sqrt(107) = 10.344070, err = -0.000010
.word 0x000A646E // sqrt(108) = 10.392303, err = -0.000001
.word 0x000A70B7 // sqrt(109) = 10.440292, err = -0.000014
.word 0x000A7CF3 // sqrt(110) = 10.488083, err = -0.000006
.word 0x000A8920 // sqrt(111) = 10.535645, err = -0.000009
.word 0x000A953F // sqrt(112) = 10.582993, err = -0.000013
.word 0x000AA151 // sqrt(113) = 10.630142, err = -0.000004
.word 0x000AAD55 // sqrt(114) = 10.677078, err = -0.000000
.word 0x000AB94B // sqrt(115) = 10.723801, err = -0.000005
.word 0x000AC534 // sqrt(116) = 10.770325, err = -0.000005
.word 0x000AD110 // sqrt(117) = 10.816650, err = -0.000003
.word 0x000ADCDF // sqrt(118) = 10.862778, err = -0.000003
.word 0x000AE8A1 // sqrt(119) = 10.908707, err = -0.000005
.word 0x000AF456 // sqrt(120) = 10.954437, err = -0.000014
.word 0x000B0000 // sqrt(121) = 11.000000, err = 0.000000
.word 0x000B0B9C // sqrt(122) = 11.045349, err = -0.000012
.word 0x000B172D // sqrt(123) = 11.090530, err = -0.000006
.word 0x000B22B2 // sqrt(124) = 11.135529, err = -0.000000
.word 0x000B2E2A // sqrt(125) = 11.180328, err = -0.000012
.word 0x000B3997 // sqrt(126) = 11.224960, err = -0.000012
.word 0x000B44F9 // sqrt(127) = 11.269424, err = -0.000003
.word 0x000B504F // sqrt(128) = 11.313705, err = -0.000003
.word 0x000B5B99 // sqrt(129) = 11.357803, err = -0.000013
.word 0x000B66D9 // sqrt(130) = 11.401749, err = -0.000006
.word 0x000B720D // sqrt(131) = 11.445511, err = -0.000012
.word 0x000B7D37 // sqrt(132) = 11.489120, err = -0.000005
.word 0x000B8856 // sqrt(133) = 11.532562, err = -0.000000
.word 0x000B936A // sqrt(134) = 11.575836, err = -0.000001
.word 0x000B9E73 // sqrt(135) = 11.618942, err = -0.000008
.word 0x000BA972 // sqrt(136) = 11.661896, err = -0.000008
.word 0x000BB467 // sqrt(137) = 11.704697, err = -0.000003
.word 0x000BBF51 // sqrt(138) = 11.747330, err = -0.000010
.word 0x000BCA32 // sqrt(139) = 11.789825, err = -0.000001
.word 0x000BD508 // sqrt(140) = 11.832153, err = -0.000006
.word 0x000BDFD4 // sqrt(141) = 11.874329, err = -0.000013
.word 0x000BEA97 // sqrt(142) = 11.916367, err = -0.000009
.word 0x000BF550 // sqrt(143) = 11.958252, err = -0.000009
.word 0x000C0000 // sqrt(144) = 12.000000, err = 0.000000
.word 0x000C0AA5 // sqrt(145) = 12.041580, err = -0.000014
.word 0x000C1542 // sqrt(146) = 12.083038, err = -0.000008
.word 0x000C1FD5 // sqrt(147) = 12.124344, err = -0.000012
.word 0x000C2A5F // sqrt(148) = 12.165512, err = -0.000013
.word 0x000C34E0 // sqrt(149) = 12.206543, err = -0.000013
.word 0x000C3F58 // sqrt(150) = 12.247437, err = -0.000012
.word 0x000C49C7 // sqrt(151) = 12.288193, err = -0.000013
.word 0x000C542E // sqrt(152) = 12.328827, err = -0.000001
.word 0x000C5E8B // sqrt(153) = 12.369308, err = -0.000008
.word 0x000C68E0 // sqrt(154) = 12.409668, err = -0.000006
.word 0x000C732C // sqrt(155) = 12.449890, err = -0.000009
.word 0x000C7D70 // sqrt(156) = 12.489990, err = -0.000006
.word 0x000C87AB // sqrt(157) = 12.529953, err = -0.000011
.word 0x000C91DE // sqrt(158) = 12.569794, err = -0.000011
.word 0x000C9C09 // sqrt(159) = 12.609512, err = -0.000008
.word 0x000CA62C // sqrt(160) = 12.649109, err = -0.000002
.word 0x000CB046 // sqrt(161) = 12.688568, err = -0.000009
.word 0x000CBA59 // sqrt(162) = 12.727921, err = -0.000002
.word 0x000CC463 // sqrt(163) = 12.767136, err = -0.000010
.word 0x000CCE66 // sqrt(164) = 12.806244, err = -0.000005
.word 0x000CD861 // sqrt(165) = 12.845230, err = -0.000002
.word 0x000CE254 // sqrt(166) = 12.884094, err = -0.000004
.word 0x000CEC3F // sqrt(167) = 12.922836, err = -0.000012
.word 0x000CF623 // sqrt(168) = 12.961472, err = -0.000010
.word 0x000D0000 // sqrt(169) = 13.000000, err = 0.000000
.word 0x000D09D4 // sqrt(170) = 13.038391, err = -0.000014
.word 0x000D13A2 // sqrt(171) = 13.076691, err = -0.000006
.word 0x000D1D68 // sqrt(172) = 13.114868, err = -0.000009
.word 0x000D2727 // sqrt(173) = 13.152939, err = -0.000008
.word 0x000D30DF // sqrt(174) = 13.190903, err = -0.000003
.word 0x000D3A8F // sqrt(175) = 13.228745, err = -0.000012
.word 0x000D4439 // sqrt(176) = 13.266495, err = -0.000004
.word 0x000D4DDB // sqrt(177) = 13.304123, err = -0.000012
.word 0x000D5777 // sqrt(178) = 13.341660, err = -0.000005
.word 0x000D610B // sqrt(179) = 13.379074, err = -0.000014
.word 0x000D6A99 // sqrt(180) = 13.416397, err = -0.000011
.word 0x000D7420 // sqrt(181) = 13.453613, err = -0.000011
.word 0x000D7DA0 // sqrt(182) = 13.490723, err = -0.000015
.word 0x000D871A // sqrt(183) = 13.527740, err = -0.000009
.word 0x000D908D // sqrt(184) = 13.564651, err = -0.000008
.word 0x000D99F9 // sqrt(185) = 13.601456, err = -0.000015
.word 0x000DA35F // sqrt(186) = 13.638168, err = -0.000013
.word 0x000DACBF // sqrt(187) = 13.674789, err = -0.000005
.word 0x000DB618 // sqrt(188) = 13.711304, err = -0.000005
.word 0x000DBF6B // sqrt(189) = 13.747726, err = -0.000001
.word 0x000DC8B7 // sqrt(190) = 13.784042, err = -0.000006
.word 0x000DD1FD // sqrt(191) = 13.820267, err = -0.000008
.word 0x000DDB3D // sqrt(192) = 13.856400, err = -0.000007
.word 0x000DE477 // sqrt(193) = 13.892441, err = -0.000003
.word 0x000DEDAA // sqrt(194) = 13.928375, err = -0.000013
.word 0x000DF6D8 // sqrt(195) = 13.964233, err = -0.000007
.word 0x000E0000 // sqrt(196) = 14.000000, err = 0.000000
.word 0x000E0921 // sqrt(197) = 14.035660, err = -0.000009
.word 0x000E123D // sqrt(198) = 14.071243, err = -0.000004
.word 0x000E1B53 // sqrt(199) = 14.106735, err = -0.000001
.word 0x000E2463 // sqrt(200) = 14.142136, err = -0.000000
.word 0x000E2D6D // sqrt(201) = 14.177444, err = -0.000002
.word 0x000E3671 // sqrt(202) = 14.212662, err = -0.000009
.word 0x000E3F70 // sqrt(203) = 14.247803, err = -0.000004
.word 0x000E4869 // sqrt(204) = 14.282852, err = -0.000005
.word 0x000E515C // sqrt(205) = 14.317810, err = -0.000011
.word 0x000E5A4A // sqrt(206) = 14.352692, err = -0.000008
.word 0x000E6332 // sqrt(207) = 14.387482, err = -0.000013
.word 0x000E6C15 // sqrt(208) = 14.422195, err = -0.000010
.word 0x000E74F2 // sqrt(209) = 14.456818, err = -0.000015
.word 0x000E7DCA // sqrt(210) = 14.491364, err = -0.000013
.word 0x000E869D // sqrt(211) = 14.525833, err = -0.000006
.word 0x000E8F6A // sqrt(212) = 14.560211, err = -0.000009
.word 0x000E9832 // sqrt(213) = 14.594513, err = -0.000007
.word 0x000EA0F5 // sqrt(214) = 14.628738, err = -0.000000
.word 0x000EA9B2 // sqrt(215) = 14.662872, err = -0.000006
.word 0x000EB26A // sqrt(216) = 14.696930, err = -0.000009
.word 0x000EBB1D // sqrt(217) = 14.730911, err = -0.000009
.word 0x000EC3CB // sqrt(218) = 14.764816, err = -0.000007
.word 0x000ECC74 // sqrt(219) = 14.798645, err = -0.000004
.word 0x000ED517 // sqrt(220) = 14.832382, err = -0.000015
.word 0x000EDDB6 // sqrt(221) = 14.866058, err = -0.000010
.word 0x000EE650 // sqrt(222) = 14.899658, err = -0.000006
.word 0x000EEEE5 // sqrt(223) = 14.933182, err = -0.000003
.word 0x000EF775 // sqrt(224) = 14.966629, err = -0.000001
.word 0x000F0000 // sqrt(225) = 15.000000, err = 0.000000
.word 0x000F0886 // sqrt(226) = 15.033295, err = -0.000002
.word 0x000F1107 // sqrt(227) = 15.066513, err = -0.000006
.word 0x000F1983 // sqrt(228) = 15.099655, err = -0.000014
.word 0x000F21FB // sqrt(229) = 15.132736, err = -0.000010
.word 0x000F2A6E // sqrt(230) = 15.165741, err = -0.000010
.word 0x000F32DC // sqrt(231) = 15.198669, err = -0.000015
.word 0x000F3B46 // sqrt(232) = 15.231537, err = -0.000009
.word 0x000F43AB // sqrt(233) = 15.264328, err = -0.000010
.word 0x000F4C0C // sqrt(234) = 15.297058, err = -0.000000
.word 0x000F5467 // sqrt(235) = 15.329697, err = -0.000013
.word 0x000F5CBF // sqrt(236) = 15.362289, err = -0.000002
.word 0x000F6511 // sqrt(237) = 15.394791, err = -0.000014
.word 0x000F6D60 // sqrt(238) = 15.427246, err = -0.000003
.word 0x000F75A9 // sqrt(239) = 15.459610, err = -0.000015
.word 0x000F7DEF // sqrt(240) = 15.491928, err = -0.000005
.word 0x000F8630 // sqrt(241) = 15.524170, err = -0.000005
.word 0x000F8E6C // sqrt(242) = 15.556335, err = -0.000014
.word 0x000F96A5 // sqrt(243) = 15.588455, err = -0.000002
.word 0x000F9ED9 // sqrt(244) = 15.620499, err = -0.000001
.word 0x000FA708 // sqrt(245) = 15.652466, err = -0.000010
.word 0x000FAF33 // sqrt(246) = 15.684372, err = -0.000015
.word 0x000FB75B // sqrt(247) = 15.716232, err = -0.000001
.word 0x000FBF7D // sqrt(248) = 15.748001, err = -0.000015
.word 0x000FC79C // sqrt(249) = 15.779724, err = -0.000010
.word 0x000FCFB7 // sqrt(250) = 15.811386, err = -0.000002
.word 0x000FD7CD // sqrt(251) = 15.842972, err = -0.000008
.word 0x000FDFDF // sqrt(252) = 15.874496, err = -0.000011
.word 0x000FE7ED // sqrt(253) = 15.905960, err = -0.000014
.word 0x000FEFF7 // sqrt(254) = 15.937363, err = -0.000015
.word 0x000FF7FD // sqrt(255) = 15.968704, err = -0.000015
.word 0x00100000 // sqrt(256) = 16.000000, err = 0.000000

