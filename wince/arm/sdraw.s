
	INCLUDE	sdraw.inc
	IMPORT	np2_pal16
	EXPORT	sdraw_getproctbl

	AREA	.text, CODE, READONLY

sdraw16p_0		stmdb	sp!, {r4 - r8, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		lr, pal16_0
				ldr		r6, [r0, #S_SRC]
				ldr		r7, [r0, #S_DST]
				ldr		r8, [r0, #S_WIDTH]
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldrh	lr, [lr]
putylp_0		ldrb	r12, [r0, r4]
				cmp		r12, #0
				beq		putyed_0
				mov		r12, r7
				mov		r2, #0
putxlp_0		strh	lr, [r12], r3
				strh	lr, [r12], r3
				add		r2, r2, #2
				cmp		r2, r8
				bcc		putxlp_0
putyed_0		add		r4, r4, #1
				add		r6, r6, #SURFACE_WIDTH
				add		r7, r7, r5
				cmp		r4, r1
				bcc		putylp_0
				str		r6, [r0, #S_SRC]
				str		r7, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r8, pc}
pal16_0			dcd		np2_pal16 + (NP2PAL_TEXT2 * 2)


sdraw16p_1		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_1
				ldr		r5, [r0, #S_Y]
				ldr		r3, [r0, #S_XALIGN]
				ldr		lr, [r0, #S_YALIGN]
				ldrb	r12, [r5, r0]
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_1		cmp		r12, #0
				beq		putyed_1
				ldr		r6, [r7]			; r2 = 0
				mov		r9, r8
				mov		r2, #0
putxlp_1		and		r4, r6, #&ff
				and		r12, r6, #(&ff << 8)
				mov		r4, r4 lsl #1
				mov		r12, r12 lsr #7
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				add		r2, r2, #4
				and		r4, r6, #(&ff << 16)
				and		r12, r6, #(&ff << 24)
				mov		r4, r4 lsr #15
				mov		r12, r12 lsr #23
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				cmp		r2, r10
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				ldrcc	r6, [r2, r7]
				bcc		putxlp_1
putyed_1		add		r5, r5, #1
				add		r7, r7, #SURFACE_WIDTH
				add		r8, r8, lr
				cmp		r5, r1
				ldrccb	r12, [r5, r0]
				bcc		putylp_1
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_DST]
				str		r5, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_1			dcd		np2_pal16 + (NP2PAL_GRPH * 2)


sdraw16p_2		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_2
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_SRC2]
				ldrb	r12, [r0, r4]
				ldr		r9, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_2		cmp		r12, #0
				beq		putyed_2
				ldr		r12, [r7]			; r2 = 0
				ldr		lr, [r8]
				mov		r2, #0
				mov		r6, r9
putxlp_2		orr		lr, lr, r12
				and		r5, lr, #&ff
				and		r12, lr, #(&ff << 8)
				mov		r5, r5 lsl #1
				mov		r12, r12 lsr #7
				ldrh	r5, [r5, r11]
				ldrh	r12, [r12, r11]
				add		r2, r2, #4
				strh	r5, [r6], r3
				strh	r12, [r6], r3
				and		r5, lr, #(&ff << 16)
				and		r12, lr, #(&ff << 24)
				mov		r5, r5 lsr #15
				mov		r12, r12 lsr #23
				ldrh	r5, [r5, r11]
				ldrh	r12, [r12, r11]
				cmp		r2, r10
				strh	r5, [r6], r3
				strh	r12, [r6], r3
				ldrcc	r12, [r2, r7]
				ldrcc	lr, [r2, r8]
				bcc		putxlp_2
				ldr		r5, [r0, #S_YALIGN]
putyed_2		add		r4, r4, #1
				add		r7, r7, #SURFACE_WIDTH
				add		r8, r8, #SURFACE_WIDTH
				add		r9, r9, r5
				cmp		r4, r1
				ldrccb	r12, [r4, r0]
				bcc		putylp_2
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_SRC2]
				str		r9, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_2			dcd		np2_pal16 + (NP2PAL_GRPH * 2)


sdraw16p_ti		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_ti
				ldr		r3, [r0, #S_XALIGN]
				ldr		r5, [r0, #S_Y]
				ldr		lr, [r0, #S_YALIGN]
				ldr		r7, [r0, #S_SRC]
				ldrb	r12, [r5, r0]
				ldr		r8, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_ti		cmp		r12, #0
				beq		putyod_ti
				ldr		r6, [r7]			; r2 = 0
				mov		r9, r8
				mov		r2, #0
putexlp_ti		and		r4, r6, #&ff
				and		r12, r6, #(&ff << 8)
				mov		r4, r4 lsl #1
				mov		r12, r12 lsr #7
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				add		r2, r2, #4
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				and		r4, r6, #(&ff << 16)
				and		r12, r6, #(&ff << 24)
				mov		r4, r4 lsr #15
				mov		r12, r12 lsr #23
				ldrh	r4, [r4, r11]
				ldrh	r12, [r11, r12]
				cmp		r2, r10
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				ldrcc	r6, [r2, r7]
				bcc		putexlp_ti
putyod_ti		add		r5, r5, #1
				ldrb	r12, [r5, r0]
				add		r7, r7, #SURFACE_WIDTH
				add		r8, r8, lr
				cmp		r12, #0
				beq		putyed_ti
				mov		r9, r8
				ldr		r6, [r7]			; r2 = 0
				mov		r2, #0
				sub		r11, r11, #((NP2PAL_GRPH - NP2PAL_TEXT) << 1)
putoxlp_ti		and		r4, r6, #&f0
				and		r12, r6, #(&f0 << 8)
				mov		r4, r4 lsr #3
				mov		r12, r12 lsr #11
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				add		r2, r2, #4
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				and		r4, r6, #(&f0 << 16)
				and		r12, r6, #(&f0 << 24)
				mov		r4, r4 lsr #19
				mov		r12, r12 lsr #27
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				cmp		r2, r10
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				ldrcc	r6, [r2, r7]
				bcc		putoxlp_ti
				add		r11, r11, #((NP2PAL_GRPH - NP2PAL_TEXT) << 1)
putyed_ti		add		r5, r5, #1
				add		r7, r7, #SURFACE_WIDTH
				add		r8, r8, lr
				cmp		r5, r1
				ldrccb	r12, [r5, r0]
				bcc		putylp_ti
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_DST]
				str		r5, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_ti		dcd		np2_pal16 + (NP2PAL_GRPH * 2)


sdraw16p_gi		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_gi
				ldr		r3, [r0, #S_XALIGN]
				ldr		r5, [r0, #S_Y]
				ldr		lr, [r0, #S_YALIGN]
				ldr		r7, [r0, #S_SRC]
				ldrb	r12, [r0, r5]
				ldr		r8, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_gi		cmp		r12, #0
				beq		putyod_gi
				ldr		r6, [r7]			; r2 = 0
				mov		r9, r8
				mov		r2, #0
putexlp_gi		and		r4, r6, #&ff
				and		r12, r6, #(&ff << 8)
				mov		r4, r4 lsl #1
				mov		r12, r12 lsr #7
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				add		r2, r2, #4
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				and		r4, r6, #(&ff << 16)
				and		r12, r6, #(&ff << 24)
				mov		r4, r4 lsr #15
				mov		r12, r12 lsr #23
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				cmp		r2, r10
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				ldrcc	r6, [r2, r7]
				bcc		putexlp_gi
putyod_gi		add		r5, r5, #1
				ldrb	r12, [r5, r0]
				add		r8, r8, lr
				cmp		r12, #0
				beq		putyed_gi
				ldrh	r4, [r11, #((NP2PAL_TEXT - NP2PAL_GRPH) << 1)]
				mov		r9, r8
				mov		r2, #0
putoxlp_gi		strh	r4, [r9], r3
				strh	r4, [r9], r3
				strh	r4, [r9], r3
				strh	r4, [r9], r3
				add		r2, r2, #4
				cmp		r2, r10
				bcc		putoxlp_gi
putyed_gi		add		r5, r5, #1
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, lr
				cmp		r5, r1
				ldrccb	r12, [r5, r0]
				bcc		putylp_gi
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_DST]
				str		r5, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_gi		dcd		np2_pal16 + (NP2PAL_GRPH * 2)


sdraw16p_2i		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_2i
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_SRC2]
				ldrb	r12, [r0, r4]
				ldr		r9, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_2i		cmp		r12, #0
				beq		putyod_2i
				ldr		r12, [r7]			; r2 = 0
				ldr		lr, [r8]			; r2 = 0
				mov		r2, #0
				mov		r6, r9
putexlp_2i		orr		lr, lr, r12
				and		r5, lr, #&ff
				and		r12, lr, #(&ff << 8)
				mov		r5, r5 lsl #1
				mov		r12, r12 lsr #7
				ldrh	r5, [r5, r11]
				ldrh	r12, [r12, r11]
				add		r2, r2, #4
				strh	r5, [r6], r3
				strh	r12, [r6], r3
				and		r5, lr, #(&ff << 16)
				and		r12, lr, #(&ff << 24)
				mov		r5, r5 lsr #15
				mov		r12, r12 lsr #23
				ldrh	r5, [r5, r11]
				ldrh	r12, [r12, r11]
				cmp		r2, r10
				strh	r5, [r6], r3
				strh	r12, [r6], r3
				ldrcc	r12, [r2, r7]
				ldrcc	lr, [r2, r8]
				bcc		putexlp_2i
				ldr		r5, [r0, #S_YALIGN]
putyod_2i		add		r4, r4, #1
				ldrb	r12, [r4, r0]
				add		r8, r8, #SURFACE_WIDTH
				add		r9, r9, r5
				cmp		r12, #0
				beq		putyed_2i
				mov		r2, #0
				ldr		lr, [r8]			; r2 = 0
				mov		r6, r9
				sub		r11, r11, #((NP2PAL_GRPH - NP2PAL_TEXT) << 1)
putoxlp_2i		and		r5, lr, #&f0
				and		r12, lr, #(&f0 << 8)
				mov		r5, r5 lsr #3
				mov		r12, r12 lsr #11
				ldrh	r5, [r5, r11]
				ldrh	r12, [r12, r11]
				add		r2, r2, #4
				strh	r5, [r6], r3
				strh	r12, [r6], r3
				and		r5, lr, #(&f0 << 16)
				and		r12, lr, #(&f0 << 24)
				mov		r5, r5 lsr #19
				mov		r12, r12 lsr #27
				ldrh	r5, [r5, r11]
				ldrh	r12, [r12, r11]
				cmp		r2, r10
				strh	r5, [r6], r3
				strh	r12, [r6], r3
				ldrcc	lr, [r2, r8]
				bcc		putoxlp_2i
				ldr		r5, [r0, #S_YALIGN]
				add		r11, r11, #((NP2PAL_GRPH - NP2PAL_TEXT) << 1)
putyed_2i		add		r4, r4, #1
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, #SURFACE_WIDTH
				add		r9, r9, r5
				cmp		r4, r1
				ldrccb	r12, [r4, r0]
				bcc		putylp_2i
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_SRC2]
				str		r9, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_2i		dcd		np2_pal16 + (NP2PAL_GRPH * 2)


sdraw16p_gie	stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_gie
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
				ldr		r3, [r0, #S_XALIGN]
				ldr		r5, [r0, #S_Y]
				ldr		lr, [r0, #S_YALIGN]
putylp_gie		ldrb	r12, [r0, r5]
				add		r5, r5, #1
				cmp		r12, #0
				beq		putyod_gie
				strb	r12, [r0, r5]
				ldr		r6, [r7]			; r2 = 0
				mov		r9, r8
				mov		r2, #0
putexlp_gie		and		r4, r6, #&ff
				and		r12, r6, #(&ff << 8)
				mov		r4, r4 lsl #1
				mov		r12, r12, lsr #7
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				add		r2, r2, #4
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				and		r4, r6, #(&ff << 16)
				and		r12, r6, #(&ff << 24)
				mov		r4, r4 lsr #15
				mov		r12, r12 lsr #23
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				cmp		r2, r10
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				ldrcc	r6, [r2, r7]
				bcc		putexlp_gie
putyod_gie		ldrb	r12, [r0, r5]
				add		r8, r8, lr
				cmp		r12, #0
				beq		putyed_gie
				mov		r9, r8
				ldr		r6, [r7]			; r2 = 0
				mov		r2, #0
				sub		r11, r11, #((NP2PAL_GRPH - NP2PAL_SKIP) << 1)
putoxlp_gie		and		r4, r6, #&ff
				and		r12, r6, #(&ff << 8)
				mov		r4, r4 lsl #1
				mov		r12, r12 lsr #7
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				add		r2, r2, #4
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				and		r4, r6, #(&ff << 16)
				and		r12, r6, #(&ff << 24)
				mov		r4, r4 lsr #15
				mov		r12, r12 lsr #23
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				cmp		r2, r10
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				ldrcc	r6, [r7, r2]
				bcc		putoxlp_gie
				add		r11, r11, #((NP2PAL_GRPH - NP2PAL_SKIP) << 1)
putyed_gie		add		r5, r5, #1
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, lr
				cmp		r5, r1
				bcc		putylp_gie
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_DST]
				str		r5, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_gie		dcd		np2_pal16 + (NP2PAL_GRPH * 2)


sdraw16p_2ie	stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_2ie
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_SRC2]
				ldr		r9, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
putylp_2ie		ldrb	r12, [r0, r4]
				add		r4, r4, #1
				cmp		r12, #0
				beq		putyod_2ie
				strb	r12, [r0, r4]
				ldr		lr, [r8]			; r2 = 0
				ldr		r12, [r7]			; r2 = 0
				mov		r2, #0
				mov		r6, r9
putexlp_2ie		orr		lr, lr, r12
				and		r5, lr, #&ff
				and		r12, lr, #(&ff << 8)
				mov		r5, r5 lsl #1
				mov		r12, r12 lsr #7
				ldrh	r5, [r5, r11]
				ldrh	r12, [r12, r11]
				add		r2, r2, #4
				strh	r5, [r6], r3
				strh	r12, [r6], r3
				and		r5, lr, #(&ff << 16)
				and		r12, lr, #(&ff << 24)
				mov		r5, r5, lsr #15
				mov		r12, r12, lsr #23
				ldrh	r5, [r5, r11]
				ldrh	r12, [r12, r11]
				cmp		r2, r10
				strh	r5, [r6], r3
				strh	r12, [r6], r3
				ldrcc	lr, [r2, r8]
				ldrcc	r12, [r2, r7]
				bcc		putexlp_2ie
				ldr		r5, [r0, #S_YALIGN]
putyod_2ie		ldrb	r12, [r0, r4]
				add		r8, r8, #SURFACE_WIDTH
				add		r9, r9, r5
				cmp		r12, #0
				beq		putyed_2ie
				mov		r2, #0
				str		r9, [r0, #S_DST]
				ldr		lr, [r8]			; r2 = 0
				sub		r11, r11, #((NP2PAL_GRPH - (NP2PALS_TXT + NP2PAL_TEXT)) << 1)
putoxlp_2ie		ldr		r12, [r2, r7]
				ands	r5, lr, #&f0
				movne	r5, r5, lsr #3
				subne	r5, r5, #(NP2PALS_TXT << 1)
				andeq	r5, r12, #&0f
				moveq	r5, r5, lsl #1
					ands	r6, lr, #(&f0 << 8)
				ldrh	r5, [r5, r11]
					movne	r6, r6 lsr #11
					subne	r6, r6, #(NP2PALS_TXT << 1)
					moveq	r6, r12 lsr #7
					andeq	r6, r6, #(&0f << 1)
				strh	r5, [r9], r3
				ands	r5, lr, #(&f0 << 16)
					ldrh	r6, [r6, r11]
				movne	r5, r5, lsr #19
				subne	r5, r5, #(NP2PALS_TXT << 1)
				moveq	r5, r12 lsr #15
				andeq	r5, r5, #(&0f << 1)
					strh	r6, [r9], r3
					ands	r6, lr, #(&f0 << 24)
				ldrh	r5, [r5, r11]
					movne	r6, r6 lsr #27
					subne	r6, r6, #(NP2PALS_TXT << 1)
					moveq	r6, r12 lsr #23
					andeq	r6, r6, #(&0f << 1)
					ldrh	r6, [r6, r11]
				strh	r5, [r9], r3
				add		r2, r2, #4
					strh	r6, [r9], r3
				cmp		r2, r10
				ldrcc	lr, [r2, r8]
				bcc		putoxlp_2ie

				ldr		r5, [r0, #S_YALIGN]
				ldr		r9, [r0, #S_DST]
				add		r11, r11, #((NP2PAL_GRPH - (NP2PALS_TXT + NP2PAL_TEXT)) << 1)
putyed_2ie		add		r4, r4, #1
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, #SURFACE_WIDTH
				add		r9, r9, r5
				cmp		r4, r1
				bcc		putylp_2ie
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_SRC2]
				str		r9, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_2ie		dcd		np2_pal16 + (NP2PAL_GRPH * 2)

sdraw16p_1d		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_1d
				ldr		r5, [r0, #S_Y]
				ldr		r3, [r0, #S_XALIGN]
				ldr		lr, [r0, #S_YALIGN]
				ldrb	r12, [r5, r0]
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_1d		cmp		r12, #0
				beq		putyed_1d
				str		r8, [r0, #S_DST]
				ldr		r6, [r7]			; r2 = 0
				add		r9, r8, lr
				mov		r2, #0
putxlp_1d		and		r4, r6, #&ff
				and		r12, r6, #(&ff << 8)
				mov		r4, r4 lsl #1
				mov		r12, r12 lsr #7
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				strh	r4, [r8], r3
				strh	r4, [r9], r3
				strh	r12, [r8], r3
				strh	r12, [r9], r3
				add		r2, r2, #4
				and		r4, r6, #(&ff << 16)
				and		r12, r6, #(&ff << 24)
				mov		r4, r4 lsr #15
				mov		r12, r12 lsr #23
				ldrh	r4, [r4, r11]
				ldrh	r12, [r12, r11]
				cmp		r2, r10
				strh	r4, [r8], r3
				strh	r4, [r9], r3
				strh	r12, [r8], r3
				strh	r12, [r9], r3
				ldrcc	r6, [r2, r7]
				bcc		putxlp_1d
				ldr		r8, [r0, #S_DST]
putyed_1d		add		r5, r5, #1
				add		r7, r7, #SURFACE_WIDTH
				add		r8, r8, lr lsl #1
				cmp		r5, r1
				ldrccb	r12, [r5, r0]
				bcc		putylp_1d
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_DST]
				str		r5, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_1d		dcd		np2_pal16 + (NP2PAL_GRPH * 2)

sdraw16p_2d		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_2d
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_SRC2]
				ldrb	r12, [r0, r4]
				ldr		r9, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_2d		cmp		r12, #0
				beq		putyed_2d
				ldr		r12, [r7]			; r2 = 0
				ldr		lr, [r8]
				str		r9, [r0, #S_DST]
				mov		r2, #0
				add		r6, r9, r5
putxlp_2d		orr		lr, lr, r12
				and		r5, lr, #&ff
				and		r12, lr, #(&ff << 8)
				mov		r5, r5 lsl #1
				mov		r12, r12 lsr #7
				ldrh	r5, [r5, r11]
				ldrh	r12, [r12, r11]
				add		r2, r2, #4
				strh	r5, [r9], r3
				strh	r5, [r6], r3
				strh	r12, [r9], r3
				strh	r12, [r6], r3
				and		r5, lr, #(&ff << 16)
				and		r12, lr, #(&ff << 24)
				mov		r5, r5 lsr #15
				mov		r12, r12 lsr #23
				ldrh	r5, [r5, r11]
				ldrh	r12, [r12, r11]
				cmp		r2, r10
				strh	r5, [r9], r3
				strh	r5, [r6], r3
				strh	r12, [r9], r3
				strh	r12, [r6], r3
				ldrcc	r12, [r2, r7]
				ldrcc	lr, [r2, r8]
				bcc		putxlp_2d
				ldr		r5, [r0, #S_YALIGN]
				ldr		r9, [r0, #S_DST]
putyed_2d		add		r4, r4, #1
				add		r7, r7, #SURFACE_WIDTH
				add		r8, r8, #SURFACE_WIDTH
				add		r9, r9, r5 lsl #1
				cmp		r4, r1
				ldrccb	r12, [r4, r0]
				bcc		putylp_2d
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_SRC2]
				str		r9, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_2d		dcd		np2_pal16 + (NP2PAL_GRPH * 2)


sdraw_getproctbl
				mov		r0, pc
				mov		pc, lr
				dcd		sdraw16p_0
				dcd		sdraw16p_1
				dcd		sdraw16p_1
				dcd		sdraw16p_2
				dcd		sdraw16p_0
				dcd		sdraw16p_ti
				dcd		sdraw16p_gi
				dcd		sdraw16p_2i
				dcd		sdraw16p_0
				dcd		sdraw16p_ti
				dcd		sdraw16p_gie
				dcd		sdraw16p_2ie
				dcd		sdraw16p_0
				dcd		sdraw16p_1d
				dcd		sdraw16p_1d
				dcd		sdraw16p_2d

	END

