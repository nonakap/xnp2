
	MACRO
$label	ROL8	$s
$label		mov		r12, $s lsr #7
			add		r1, r12, $s lsl #1
			bic		r8, r8, #C_FLAG
			eor		r2, r1, $s
			orr		r8, r12, r8
			and		r2, r2, #&80
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r2 lsl #4
	MEND

	MACRO
$label	ROR8	$s
$label		and		r12, $s, #1
			mov		r1, $s lsr #1
			add		r1, r1, r12 lsl #7
			bic		r8, r8, #C_FLAG
			eor		r2, r1, $s
			orr		r8, r12, r8
			and		r2, r2, #&80
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r2 lsl #4
	MEND

	MACRO
$label	RCL8	$s
$label		and		r12, r8, #1
			add		r1, r12, $s lsl #1
			bic		r8, r8, #C_FLAG
			orr		r8, r8, $s lsr #7
			eor		r2, r1, $s
			and		r2, r2, #&80
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r2 lsl #4
	MEND

	MACRO
$label	RCR8	$s
$label		and		r12, r8, #1
			mov		r1, $s lsr #1
			add		r1, r1, r12 lsl #7
			bic		r8, r8, #C_FLAG
			and		r2, $s, #1
			orr		r8, r2, r8
			eor		r2, r1, $s
			and		r2, r2, #&80
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r2 lsl #4
	MEND

	MACRO
$label	SHL8	$s
$label		mov		r1, $s lsl #1
			eor		r12, r1, $s
			and		r12, r12, #&80
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r12 lsl #4
			ldrb	r12, [r10, r1]
			bic		r8, r8, #&ff
			orr		r12, r12, #A_FLAG
			orr		r8, r12, r8
	MEND

	MACRO
$label	SHR8	$s
$label		mov		r1, $s lsr #1
			and		r12, $s, #&80
			and		r2, $s, #1
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r12 lsl #4
			ldrb	r12, [r10, r1]
			bic		r8, r8, #&ff
			orr		r12, r2, r12
			orr		r12, r12, #A_FLAG
			orr		r8, r12, r8
	MEND

	MACRO
$label	SAR8	$s
$label		and		r12, $s, #&80
			add		r1, r12, $s lsr #1
			and		r2, $s, #1
			ldrb	r12, [r10, r1]
			bic		r8, r8, #O_FLAG
			bic		r8, r8, #&ff
			orr		r12, r2, r12
			orr		r12, r12, #A_FLAG
			orr		r8, r12, r8
	MEND


	MACRO
$label	ROL16	$s
$label		mov		r12, $s lsr #15
			add		r1, r12, $s lsl #1
			bic		r8, r8, #C_FLAG
			eor		r2, r1, $s
			orr		r8, r12, r8
			and		r2, r2, #&8000
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r2 lsr #4
	MEND

	MACRO
$label	ROR16	$s
$label		and		r12, $s, #1
			mov		r1, $s lsr #1
			add		r1, r1, r12 lsl #15
			bic		r8, r8, #C_FLAG
			eor		r2, r1, $s
			orr		r8, r12, r8
			and		r2, r2, #&8000
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r2 lsr #4
	MEND

	MACRO
$label	RCL16	$s
$label		and		r12, r8, #1
			add		r1, r12, $s lsl #1
			bic		r8, r8, #C_FLAG
			orr		r8, r8, $s lsr #15
			eor		r2, r1, $s
			and		r2, r2, #&8000
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r2 lsr #4
	MEND

	MACRO
$label	RCR16	$s
$label		and		r12, r8, #1
			mov		r1, $s lsr #1
			add		r1, r1, r12 lsl #15
			bic		r8, r8, #C_FLAG
			and		r2, $s, #1
			orr		r8, r2, r8
			eor		r2, r1, $s
			and		r2, r2, #&8000
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r2 lsr #4
	MEND

	MACRO
$label	SHL16	$s
$label		mov		r1, $s lsl #1
			eor		r12, r1, $s
			and		r12, r12, #&8000
			bic		r8, r8, #O_FLAG
			and		r2, r1, #&ff
			orr		r8, r8, r12 lsr #4
			ldrb	r12, [r10, r2]
			bic		r8, r8, #&ff
			and		r12, r12, #P_FLAG
			orr		r8, r8, r1 lsr #16
			orr		r12, r12, #A_FLAG
			orr		r8, r12, r8
			movs	r2, r1 lsl #16
			orreq	r8, r8, #Z_FLAG
			orrmi	r8, r8, #S_FLAG
	MEND

	MACRO
$label	SHR16	$s
$label		mov		r1, $s lsr #1
			and		r12, $s, #&8000
			and		r2, $s, #1
			bic		r8, r8, #O_FLAG
			and		r3, r1, #&ff
			orr		r8, r8, r12 lsr #4
			ldrb	r12, [r10, r3]
			bic		r8, r8, #&ff
			and		r12, r12, #P_FLAG
			orr		r12, r2, r12
			orr		r12, r12, #A_FLAG
			orr		r8, r12, r8
			movs	r2, r1 lsl #16
			orreq	r8, r8, #Z_FLAG
			orrmi	r8, r8, #S_FLAG
	MEND

	MACRO
$label	SAR16	$s
$label		and		r12, $s, #&8000
			add		r1, r12, $s lsr #1
			and		r2, $s, #1
			and		r3, r1, #&ff
			bic		r8, r8, #O_FLAG
			ldrb	r12, [r10, r3]
			bic		r8, r8, #&ff
			and		r12, r12, #P_FLAG
			orr		r12, r2, r12
			orr		r12, r12, #A_FLAG
			orr		r8, r12, r8
			movs	r2, r1 lsl #16
			orreq	r8, r8, #Z_FLAG
			orrmi	r8, r8, #S_FLAG
	MEND


	MACRO
$label	ROL8CL	$s, $cl
$label		rsb		r1, $cl, #0
			and		r1, r1, #7
			orr		r12, $s, $s lsl #8
			mov		r3, r12 lsr r1
			and		r1, r3, #&ff
			and		r12, r1, #1
			bic		r8, r8, #C_FLAG
			orr		r8, r8, r12
			eor		r12, r12, r1 lsr #7
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r12 lsl #11
	MEND

	MACRO
$label	ROR8CL	$s, $cl
$label		and		r1, $cl, #7
			orr		r12, $s, $s lsl #8
			mov		r3, r12 lsr r1
			and		r1, r3, #&ff
			bic		r8, r8, #C_FLAG
			orr		r8, r8, r1 lsr #7
			eor		r12, r1, r1 lsl #1
			and		r12, r12, #&80
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r12 lsl #4
	MEND

	MACRO
$label	RCL8CL	$s, $cl
$label		mov		r1, $cl
$label.1	subs	r1, r1, #9
			bcs		$label.1
			rsb		r1, r1, #0
			and		r12, r8, #1
			orr		r12, $s, r12 lsl #8
			orr		r12, r12, r12 lsl #9
			mov		r3, r12 lsr r1
			and		r1, r3, #&ff
			and		r12, r3, #&100
			bic		r8, r8, #C_FLAG
			orr		r8, r8, r12 lsr #8
			eor		r12, r3, r3 lsr #1
			and		r12, r12, #&80
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r12 lsl #4
	MEND

	MACRO
$label	RCR8CL	$s, $cl
$label		mov		r1, $cl
$label.1	subs	r1, r1, #9
			bcs		$label.1
			add		r1, r1, #9
			and		r12, r8, #1
			orr		r12, $s, r12 lsl #8
			orr		r12, r12, r12 lsl #9
			mov		r3, r12 lsr r1
			and		r1, r3, #&ff
			and		r12, r3, #&100
			bic		r8, r8, #C_FLAG
			orr		r8, r8, r12 lsr #8
			eor		r12, r3, r3 lsl #1
			and		r12, r12, #&80
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r12 lsl #4
	MEND

	MACRO
$label	SHL8CL	$s, $cl
$label		mov		r1, $s lsl $cl
			bic		r8, r8, #O_FLAG
			eor		r12, r1, r1 lsr #1
			and		r12, r12, #&80
			orr		r8, r8, r12 lsl #4
			mov		r2, r1 lsl #(32 - 9)
			ldrb	r12, [r10, r2 lsr #(32 - 9)]
			bic		r8, r8, #&ff
			orr		r12, r12, #A_FLAG
			orr		r8, r12, r8
	MEND

	MACRO
$label	SHR8CL	$s, $cl
$label		sub		r12, $cl, #1
			bic		r8, r8, #&ff
			mov		r1, $s lsr r12
			and		r12, r1, #1
			mov		r1, $s lsr $cl
			orr		r8, r8, r12
			and		r12, r1, #&40
			bic		r8, r8, #O_FLAG
			orr		r8, r8, r12 lsl #5
			ldrb	r12, [r10, r1]
			orr		r12, r12, #A_FLAG
			orr		r8, r12, r8
	MEND

	MACRO
$label	SAR8CL	$s, $cl
$label		mov		r1, $s lsl #24
			mov		r1, r1 asr #24
			sub		r12, $cl, #1
			bic		r8, r8, #&ff
			mov		r1, r1 asr r12
			and		r12, r1, #1
			mov		r1, r1 asr #1
			orr		r8, r8, r12
			and		r1, r1, #&ff
			bic		r8, r8, #O_FLAG
			ldrb	r12, [r10, r1]
			orr		r12, r12, #A_FLAG
			orr		r8, r12, r8
	MEND


	MACRO
$label	ROL16CL	$s, $cl
$label		cmp		$cl, #1
			eoreq	r1, $s, $s lsl #1
			andeq	r1, r1, #&8000
			biceq	r8, r8, #O_FLAG
			orreq	r8, r8, r1 lsr #4
			and		r1, $cl, #15
			orr		r12, $s, $s lsl #16
			mov		r3, r12 lsl r1
			mov		r1, r3 lsr #16
			and		r12, r1, #1
			bic		r8, r8, #C_FLAG
			orr		r8, r8, r12
	MEND

	MACRO
$label	ROR16CL	$s, $cl
$label		cmp		$cl, #1
			eoreq	r1, $s, $s lsl #15
			andeq	r1, r1, #&8000
			biceq	r8, r8, #O_FLAG
			orreq	r8, r8, r1 lsr #4
			rsb		r3, $cl, #0
			and		r1, r3, #15
			orr		r12, $s, $s lsl #16
			mov		r3, r12 lsl r1
			mov		r1, r3 lsr #16
			bic		r8, r8, #C_FLAG
			orr		r8, r8, r1 lsr #15
	MEND

	MACRO
$label	RCL16CL	$s, $cl
$label		cmp		$cl, #1
			eoreq	r1, $s, $s lsl #1
			andeq	r1, r1, #&8000
			biceq	r8, r8, #O_FLAG
			orreq	r8, r8, r1 lsr #4
			mov		r1, $cl
			cmp		r1, #17
			subcs	r1, r1, #17
			and		r12, r8, #1
			orr		r3, $s, r12 lsl #16
			mov		r12, r3 lsl r1
			rsb		r1, r1, #17
			mov		r3, r3 lsr r1
			orr		r1, r3, r12
			and		r12, r1, #&10000
			bic		r8, r8, #C_FLAG
			orr		r8, r8, r12 lsr #16
	MEND

	MACRO
$label	RCR16CL	$s, $cl
$label		cmp		$cl, #1
			eoreq	r1, r8, $s lsr #15
			andeq	r1, r1, #1
			biceq	r8, r8, #O_FLAG
			orreq	r8, r8, r1 lsl #11
			mov		r1, $cl
			cmp		r1, #17
			subcs	r1, r1, #17
			and		r3, r8, #1
			orr		r12, $s, r3 lsl #16
			mov		r3, r12 lsr r1
			rsb		r1, r1, #17
			mov		r12, r12 lsl r1
			orr		r1, r12, r3
			and		r12, r1, #&10000
			bic		r8, r8, #C_FLAG
			orr		r8, r8, r12 lsr #16
	MEND

	MACRO
$label	SHL16CL	$s, $cl
$label		bic		r8, r8, #O_FLAG
			cmp		$cl, #1
			eoreq	r1, $s, $s lsl #1
			andeq	r1, r1, #&8000
			orreq	r8, r8, r1 lsr #4
			mov		r1, $s lsl $cl
			and		r2, r1, #&ff
			ldrb	r12, [r10, r2]
			bic		r8, r8, #&ff
			and		r12, r12, #P_FLAG
			orr		r8, r12, r8
			movs	r2, r1 lsl #16
			orreq	r8, r8, #Z_FLAG
			orrmi	r8, r8, #S_FLAG
			and		r2, r1, #&10000
			orr		r8, r8, r2 lsr #16
	MEND

	MACRO
$label	SHR16CL	$s, $cl
$label		bic		r8, r8, #O_FLAG
			cmp		$cl, #1
			andeq	r1, $s, #&8000
			orreq	r8, r8, r1 lsr #4
			sub		r12, $cl, #1
			bic		r8, r8, #&ff
			mov		r1, $s lsr r12
			and		r12, r1, #1
			mov		r1, r1 lsr #1
			orr		r8, r8, r12
			and		r2, r1, #&ff
			ldrb	r12, [r10, r2]
			and		r12, r12, #P_FLAG
			orr		r8, r12, r8
			movs	r2, r1 lsl #16
			orreq	r8, r8, #Z_FLAG
			orrmi	r8, r8, #S_FLAG
	MEND

	MACRO
$label	SAR16CL	$s, $cl
$label		mov		r1, $s lsl #16
			mov		r1, r1 asr #16
			sub		r12, $cl, #1
			bic		r8, r8, #&ff
			mov		r1, r1 asr r12
			and		r12, r1, #1
			mov		r1, r1 asr #1
			orr		r8, r8, r12
			and		r2, r1, #&ff
			bic		r8, r8, #O_FLAG
			ldrb	r12, [r10, r2]
			and		r12, r12, #P_FLAG
			orr		r8, r12, r8
			movs	r2, r1 lsl #16
			orreq	r8, r8, #Z_FLAG
			orrmi	r8, r8, #S_FLAG
	MEND

	END

