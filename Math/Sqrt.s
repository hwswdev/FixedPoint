
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

.type sqrtFixed, %function
.align 4
sqrtFixed:
	push {lr}
	bl sqrt_0_31
	pop {lr}
	bx lr

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

.type getMsbX, %function
.align 4
getMsbX:
	push {lr}
	bl getMsb
	lsr r0, r0, #1
	add r0, r0, #1
	lsl r0, r0, #1
	pop {lr}
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

.type sqrt, %function
.align 4
sqrt_0_31:
	push {r1-r3, lr}
	mov r3, r0
	bl getMsbX
	sub r1, r0, #2
	ldr r2, =#0x55555555  // 1/0x00000003
	lsr r2, r2, r1		  // Divisor
	mov r0, #1
	lsl r0, r0, r1  	  // R0 <= ( 1 << MSB )
    sub r0, r3, r0
    umull r0, r1, r0, r2  // R1 <= dX



	mov r1, r0, lsr #1
	ldr r2, =#sqrt_0_31_table
	ldr r0, [r2, r3, lsl #2 ]
	pop {r1-r3, lr}
	bx lr


.align 4
div_c_val:
.word 0x55555555  // 1/0x00000003
.word 0x2AAAAAAA  // 1/0x00000006
.word 0x15555555  // 1/0x0000000C
.word 0x0AAAAAAA  // 1/0x00000018
.word 0x05555555  // 1/0x00000030
.word 0x02AAAAAA  // 1/0x00000060
.word 0x01555555  // 1/0x000000C0
.word 0x00AAAAAA  // 1/0x00000180
.word 0x00555555  // 1/0x00000300
.word 0x002AAAAA  // 1/0x00000600
.word 0x00155555  // 1/0x00000C00
.word 0x000AAAAA  // 1/0x00001800
.word 0x00055555  // 1/0x00003000
.word 0x0002AAAA  // 1/0x00006000
.word 0x00015555  // 1/0x0000C000
.word 0x0000AAAA  // 1/0x00018000
.word 0x00005555  // 1/0x00030000
.word 0x00002AAA  // 1/0x00060000
.word 0x00001555  // 1/0x000C0000
.word 0x00000AAA  // 1/0x00180000
.word 0x00000555  // 1/0x00300000
.word 0x000002AA  // 1/0x00600000
.word 0x00000155  // 1/0x00C00000
.word 0x000000AA  // 1/0x01800000
.word 0x00000055  // 1/0x03000000
.word 0x0000002A  // 1/0x06000000
.word 0x00000015  // 1/0x0C000000
.word 0x0000000A  // 1/0x18000000
.word 0x00000005  // 1/0x30000000
.word 0x00000002  // 1/0x60000000
.word 0x00000001  // 1/0xC0000000


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

