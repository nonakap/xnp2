
	INCLUDE	sdraw.inc
	IMPORT	np2_pal16
	EXPORT	sdraw_getproctbl

	AREA	.text, CODE, READONLY

qvga16p_0		stmdb	sp!, {r4 - r8, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		lr, pal16_0
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		lr, [lr]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r6, [r0, #S_SRC]
				ldr		r7, [r0, #S_DST]
				ldrh	r12, [r4, r0]
				ldr		r8, [r0, #S_WIDTH]
				orr		lr, lr, lr lsr #16
putylp_0		cmp		r12, #0
				beq		putyed_0
				mov		r12, r7
				mov		r2, #0
putxlp_0		strh	lr, [r12], r3
				strh	lr, [r12], r3
				add		r2, r2, #4
				cmp		r2, r8
				bcc		putxlp_0
putyed_0		add		r4, r4, #2
				add		r6, r6, #(SURFACE_WIDTH * 2)
				add		r7, r7, r5
				cmp		r4, r1
				ldrcch	r12, [r4, r0]
				bcc		putylp_0
				str		r6, [r0, #S_SRC]
				str		r7, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r8, pc}
pal16_0			dcd		(np2_pal16 + (NP2PAL_TEXT2 * 4))

qvga16p_1		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_1
				ldr		lr, pmask_1
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r7, [r0, #S_SRC]
				ldrh	r12, [r0, r4]
				ldr		r8, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_1		cmp		r12, #0
				beq		putyed_1
				ldr		r9, [r7]			; r2 = 0
				str		r4, [r0, #S_Y]
				str		r8, [r0, #S_DST]
				mov		r2, #0
putxlp_1		and		r5, r9, #255
				mov		r12, r9 lsr #8
				and		r12, r12, #255
				ldr		r5, [r11, r5 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				mov		r4, r9 lsr #16
				and		r4, r4, #255
				add		r5, r5, r12
				mov		r12, r9 lsr #24
				ldr		r6, [r11, r4 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				add		r9, r2, #SURFACE_WIDTH
				ldr		r9, [r9, r7]
				add		r6, r12, r6
				add		r2, r2, #4
				and		r4, r9, #255
				mov		r12, r9 lsr #8
				and		r12, r12, #255
				ldr		r4, [r11, r4 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				mov		r9, r9 lsr #16
				add		r5, r5, r4
				add		r5, r5, r12
				and		r4, r9, #255
				mov		r12, r9 lsr #8
				ldr		r4, [r11, r4 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				and		r5, r5, lr
				add		r6, r6, r4
				add		r6, r6, r12
				and		r6, r6, lr
				mov		r9, r5 lsr #2
				add		r9, r9, r5 lsr #18
				mov		r12, r6 lsr #2
				add		r12, r12, r6 lsr #18
				strh	r9, [r8], r3
				strh	r12, [r8], r3
				cmp		r2, r10
				ldrcc	r9, [r2, r7]
				bcc		putxlp_1
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r8, [r0, #S_DST]
putyed_1		add		r4, r4, #2
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, r5
				cmp		r4, r1
				ldrcch	r12, [r4, r0]
				bcc		putylp_1
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_1			dcd		(np2_pal16 + (NP2PAL_GRPH * 4))
pmask_1			dcd		(&07e0f81f << 2)

qvga16p_2		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_2
				ldr		lr, pmask_2
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_SRC2]
				ldr		r12, [r4, r0]
				ldr		r9, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_2		cmp		r12, #0
				beq		putyed_2
				str		r9, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldr		r4, [r7]			; r2 = 0
				ldr		r12, [r8]			; r2 = 0
				mov		r2, #0
				str		r1, [sp, #-4]!
putxlp_2		add		r4, r12, r4
				and		r5, r4, #255
				mov		r12, r4 lsr #8
				and		r12, r12, #255
				ldr		r5, [r11, r5 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				mov		r6, r4 lsr #16
				and		r6, r6, #255
				add		r5, r12, r5
				mov		r12, r4 lsr #24
				ldr		r6, [r11, r6 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				add		r1, r2, #SURFACE_WIDTH
				ldr		r4, [r1, r7]
				ldr		r1, [r1, r8]
				add		r2, r2, #4
				add		r6, r6, r12
				add		r4, r1, r4
				and		r1, r4, #255
				mov		r12, r4 lsr #8
				and		r12, r12, #255
				ldr		r1, [r11, r1 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				mov		r4, r4 lsr #16
				add		r5, r5, r1
				add		r5, r5, r12
				and		r1, r4, #255
				mov		r12, r4 lsr #8
				ldr		r1, [r11, r1 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				and		r5, r5, lr
				add		r6, r6, r1
				add		r6, r6, r12
				and		r6, r6, lr
				mov		r4, r5 lsr #2
				mov		r12, r6 lsr #2
				add		r4, r4, r5 lsr #18
				add		r12, r12, r6 lsr #18
				strh	r4, [r9], r3
				strh	r12, [r9], r3
				cmp		r2, r10
				ldrcc	r4, [r2, r7]
				ldrcc	r12, [r2, r8]
				bcc		putxlp_2
				ldr		r9, [r0, #S_DST]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r1, [sp], #4
putyed_2		add		r4, r4, #2
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, #(SURFACE_WIDTH * 2)
				add		r9, r9, r5
				cmp		r4, r1
				ldrcch	r12, [r4, r0]
				bcc		putylp_2
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_SRC2]
				str		r9, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_2			dcd		(np2_pal16 + (NP2PAL_GRPH * 4))
pmask_2			dcd		(&07e0f81f << 2)

qvga16p_gi		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_gi
				ldr		lr, pmask_gi
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r7, [r0, #S_SRC]
				ldrh	r12, [r4, r0]
				ldr		r8, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_gi		cmp		r12, #0
				beq		putyed_gi
				str		r4, [r0, #S_Y]
				ldr		r4, [r7]			; r2 = 0
				mov		r9, r8
				mov		r2, #0
putxlp_gi		and		r5, r4, #255
				mov		r12, r4 lsr #8
				and		r12, r12, #255
				ldr		r5, [r11, r5 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				mov		r6, r4 lsr #16
				and		r6, r6, #255
				add		r5, r12, r5
				mov		r12, r4 lsr #24
				ldr		r6, [r11, r6 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				and		r5, r5, lr
				add		r2, r2, #4
				mov		r4, r5 lsr #1
				orr		r4, r4, r5 lsr #17
				add		r6, r12, r6
				and		r6, r6, lr
				strh	r4, [r9], r3
				mov		r12, r6 lsr #1
				orr		r12, r12, r6 lsr #17
				cmp		r2, r10
				strh	r12, [r9], r3
				ldrcc	r4, [r2, r7]
				bcc		putxlp_gi
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
putyed_gi		add		r4, r4, #2
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, r5
				cmp		r4, r1
				ldrcch	r12, [r4, r0]
				bcc		putylp_gi
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_gi		dcd		(np2_pal16 + (NP2PAL_GRPH * 4))
pmask_gi		dcd		(&07e0f81f << 1)

qvga16p_2i		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_2i
				ldr		lr, pmask_2i
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_SRC2]
				ldrh	r12, [r0, r4]
				ldr		r9, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_2i		cmp		r12, #0
				beq		putyed_2i
				str		r9, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldr		r6, [r8, #SURFACE_WIDTH]
				mov		r2, #0
				str		r1, [sp, #-4]!
putxlp_2i		ldr		r4, [r7, r2]
				ands	r12, r6, #(&f0 << 0)
					andeq	r12, r4, #255
					movne	r12, r12 lsr #4
					addne	r12, r12, #(NP2PAL_TEXT - NP2PAL_GRPH)
				ldr		r5, [r11, r12 lsl #2]
				ands	r12, r6, #(&f0 << 8)
					moveq	r12, r4 lsr #8
					andeq	r12, r12, #255
					movne	r12, r12 lsr #12
					addne	r12, r12, #(NP2PAL_TEXT - NP2PAL_GRPH)
				ldr		r1, [r11, r12 lsl #2]
				ands	r12, r6, #(&f0 << 16)
					moveq	r12, r4 lsr #16
					andeq	r12, r12, #255
					movne	r12, r12 lsr #20
					addne	r12, r12, #(NP2PAL_TEXT - NP2PAL_GRPH)
				movs	r6, r6 lsr #28
					moveq	r6, r4 lsr #24
					addne	r6, r6, #(NP2PAL_TEXT - NP2PAL_GRPH)
				ldr		r12, [r11, r12 lsl #2]
				ldr		r6, [r11, r6 lsl #2]
				add		r5, r5, r1
				ldr		r1, [r8, r2]
		;;		ldr		r4, [r7, r2]
				add		r6, r6, r12
				add		r4, r1, r4
				and		r1, r4, #255
				mov		r12, r4 lsr #8
				and		r12, r12, #255
				ldr		r1, [r11, r1 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				add		r2, r2, #4
				add		r5, r5, r1
				add		r5, r5, r12
				mov		r12, r4 lsr #16
				and		r12, r12, #255
				mov		r4, r4 lsr #24
				ldr		r12, [r11, r12 lsl #2]
				ldr		r4, [r11, r4 lsl #2]
				and		r5, r5, lr
				add		r6, r6, r12
				add		r6, r6, r4
				and		r6, r6, lr
				mov		r12, r5 lsr #2
				orr		r12, r12, r5 lsr #18
				strh	r12, [r9], r3
				mov		r12, r6 lsr #2
				orr		r12, r12, r6 lsr #18
				strh	r12, [r9], r3
				add		r6, r2, #SURFACE_WIDTH
				cmp		r2, r10
				ldrcc	r6, [r6, r8]
				bcc		putxlp_2i
				ldr		r1, [sp], #4
				ldr		r9, [r0, #S_DST]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
putyed_2i		add		r4, r4, #2
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, #(SURFACE_WIDTH * 2)
				add		r9, r9, r5
				cmp		r4, r1
				ldrcch	r12, [r4, r0]
				bcc		putylp_2i
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_SRC2]
				str		r9, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_2i		dcd		(np2_pal16 + (NP2PAL_GRPH * 4))
pmask_2i		dcd		(&07e0f81f << 2)

qvga16p_1d		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_1d
				ldr		lr, pmask_1d
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r7, [r0, #S_SRC]
				ldrb	r12, [r4, r0]
				ldr		r8, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_1d		cmp		r12, #0
				beq		putyed_1d
				str		r4, [r0, #S_Y]
				ldr		r4, [r7]			; r2 = 0
				mov		r9, r8
				mov		r2, #0
putxlp_1d		and		r5, r4, #255
				mov		r12, r4 lsr #8
				and		r12, r12, #255
				ldr		r5, [r11, r5 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				mov		r6, r4 lsr #16
				and		r6, r6, #255
				add		r5, r12, r5
				mov		r12, r4 lsr #24
				ldr		r6, [r11, r6 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				and		r5, r5, lr
				add		r2, r2, #4
				mov		r4, r5 lsr #1
				orr		r4, r4, r5 lsr #17
				add		r6, r12, r6
				and		r6, r6, lr
				strh	r4, [r9], r3
				mov		r12, r6 lsr #1
				orr		r12, r12, r6 lsr #17
				cmp		r2, r10
				strh	r12, [r9], r3
				ldrcc	r4, [r2, r7]
				bcc		putxlp_1d
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
putyed_1d		add		r4, r4, #1
				add		r7, r7, #SURFACE_WIDTH
				add		r8, r8, r5
				cmp		r4, r1
				ldrccb	r12, [r4, r0]
				bcc		putylp_1d
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_1d		dcd		(np2_pal16 + (NP2PAL_GRPH * 4))
pmask_1d		dcd		(&07e0f81f << 1)

qvga16p_2d		stmdb	sp!, {r4 - r11, lr}
				add		r0, r0, #S_HDRSIZE
				ldr		r11, pal16_2d
				ldr		lr, pmask_2d
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_SRC2]
				ldrb	r12, [r4, r0]
				ldr		r9, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
putylp_2d		cmp		r12, #0
				beq		putyed_2d
				str		r4, [r0, #S_Y]
				ldr		r4, [r7]			; r2 = 0
				ldr		r12, [r8]			; r2 = 0
				str		r9, [r0, #S_DST]
				mov		r2, #0
putxlp_2d		add		r4, r12, r4
				and		r5, r4, #255
				mov		r12, r4 lsr #8
				and		r12, r12, #255
				ldr		r5, [r11, r5 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				mov		r6, r4 lsr #16
				and		r6, r6, #255
				add		r5, r12, r5
				mov		r12, r4 lsr #24
				ldr		r6, [r11, r6 lsl #2]
				ldr		r12, [r11, r12 lsl #2]
				and		r5, r5, lr
				add		r2, r2, #4
				mov		r4, r5 lsr #1
				orr		r4, r4, r5 lsr #17
				add		r6, r12, r6
				and		r6, r6, lr
				strh	r4, [r9], r3
				mov		r12, r6 lsr #1
				orr		r12, r12, r6 lsr #17
				cmp		r2, r10
				strh	r12, [r9], r3
				ldrcc	r4, [r2, r7]
				ldrcc	r12, [r2, r8]
				bcc		putxlp_2d
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		r9, [r0, #S_DST]
putyed_2d		add		r4, r4, #1
				add		r7, r7, #SURFACE_WIDTH
				add		r8, r8, #SURFACE_WIDTH
				add		r9, r9, r5
				cmp		r4, r1
				ldrccb	r12, [r4, r0]
				bcc		putylp_2d
				str		r7, [r0, #S_SRC]
				str		r8, [r0, #S_SRC2]
				str		r9, [r0, #S_DST]
				str		r4, [r0, #S_Y]
				ldmia	sp!, {r4 - r11, pc}
pal16_2d		dcd		(np2_pal16 + (NP2PAL_GRPH * 4))
pmask_2d		dcd		(&07e0f81f << 1)


sdraw_getproctbl
				mov		r0, pc
				mov		pc, lr
				dcd		qvga16p_0
				dcd		qvga16p_1
				dcd		qvga16p_1
				dcd		qvga16p_2
				dcd		qvga16p_0
				dcd		qvga16p_1
				dcd		qvga16p_gi
				dcd		qvga16p_2i
				dcd		qvga16p_0
				dcd		qvga16p_1
				dcd		qvga16p_gi
				dcd		qvga16p_2i
				dcd		qvga16p_0
				dcd		qvga16p_1d
				dcd		qvga16p_1d
				dcd		qvga16p_2d

	END

