
	EXPORT	__randseed
	EXPORT	rand_setseed
	EXPORT	rand_get
	EXPORT	AdjustAfterMultiply
	EXPORT	AdjustBeforeDivision
	EXPORT	sjis2jis
	EXPORT	jis2sjis
	EXPORT	satuation_s16
	EXPORT	satuation_s16x


	AREA	.data, DATA, READWRITE

__randseed		dcd		1


	AREA	.text, CODE, READONLY

rand_setseed	ldr		r1, prandseed
				str		r0, [r1]
				mov		pc, lr
rand_get		ldr		r1, prandseed
				ldr		r2, randdcd1
				ldr		r3, randdcd2
				ldr		r0, [r1]
				mla		r2, r0, r2, r3
				mov		r0, r2, asr #16
				str		r2, [r1]
				mov		pc, lr
prandseed		dcd		__randseed
randdcd1		dcd		&343fd
randdcd2		dcd		&269ec3

AdjustAfterMultiply
				and		r0, r0, #255
				mov		r1, #205				; îÕàÕÇ™0-255Ç»ÇÃÇ≈ê∏ìxí·Çµ
				mul		r1, r0, r1
				mov		r1, r1, lsr #11
				sub		r0, r0, r1, lsl #1
		if 1
				add		r0, r0, r1, lsl #3
		else
				sub		r0, r0, r1, lsl #3
				add		r0, r0, r1, lsl #4
		endif
				mov		pc, lr

AdjustBeforeDivision
				and		r1, r0, #&f0
				and		r0, r0, #15
				add		r0, r0, r1, lsr #3
				add		r0, r0, r1, lsr #1
				mov		pc, lr

sjis2jis		and		r1, r0, #255
				sub		r1, r1, r1, lsr #7
				mov		r1, r1, lsl	#23
				adds	r1, r1, #(&62 << 23)
				subpl	r1, r1, #(&a2 << 23)
				mov		r2, #&1f00
				orr		r2, r2, #&21
				add		r1, r2, r1, lsr #23
				and		r0, r0, #&3f00
				add		r0, r1, r0, lsl #1
				mov		pc, lr

jis2sjis		and		r1, r0, #&7f00
				and		r0, r0, #&7f
				tst		r1, #&100
				addeq	r0, r0, #&5e
				cmp		r0, #&60
				addcs	r0, r0, #1
				add		r0, r0, #&1f
				add		r1, r1, #&2100
				mov		r1, r1, lsr #1
				and		r1, r1, #&ff00
				eor		r1, r1, #&a000
				orr		r0, r0, r1
				mov		pc, lr

satuation_s16	movs	r2, r2 lsr #2
				moveq	pc, lr
				stmdb	sp!, {r4, lr}
				ldr		lr, dcd_ffff8000
				mov		r12, #&7f00
				orr		r12, r12, #&7f
ss16_lp			ldr		r3, [r1], #4
				ldr		r4, [r1], #4
				cmp		r3, r12
				movgt	r3, r12
				cmple	r3, lr
				movlt	r3, lr
				mov		r3, r3, lsl #16
				cmp		r4, r12
				movgt	r4, r12
				cmple	r4, lr
				movlt	r4, lr
				mov		r4, r4, lsl #16
				add		r3, r4, r3 lsr #16
				str		r3, [r0], #4
				subs	r2, r2, #1
				bne		ss16_lp
				ldmia	sp!, {r4, pc}
dcd_ffff8000	dcd		&ffff8000

satuation_s16x	movs	r2, r2 lsr #2
				moveq	pc, lr
				stmdb	sp!, {r4, lr}
				ldr		lr, dcd_ffff8000
				mov		r12, #&7f00
				orr		r12, r12, #&7f
ss16x_lp		ldr		r3, [r1], #4
				ldr		r4, [r1], #4
				cmp		r3, r12
				movgt	r3, r12
				cmple	r3, lr
				movlt	r3, lr
				mov		r3, r3, lsl #16
				cmp		r4, r12
				movgt	r4, r12
				cmple	r4, lr
				movlt	r4, lr
				mov		r4, r4, lsl #16
				subs	r2, r2, #1
				add		r3, r3, r4 lsr #16
				str		r3, [r0], #4
				bne		ss16x_lp
				ldmia	sp!, {r4, pc}

	END

