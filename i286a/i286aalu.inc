
	MACRO
$label	ADD8	$d, $s
$label		mov		r1, $s lsl #24
			adds	r1, r1, $d lsl #24
			orrvs	r8, r8, #O_FLAG
			mov		r1, r1 lsr #24
			bicvc	r8, r8, #O_FLAG
			eor		r12, r1, $s
			ldrb	r2, [r10, r1]
			eor		r12, r12, $d
			addcs	r2, r2, #C_FLAG
			and		r12, r12, #A_FLAG
			orr		r2, r2, r12
			bic		r8, r8, #&ff
			add		r8, r8, r2
	MEND

	MACRO
$label	OR8		$d, $s
$label		orr		r1, $d, $s
			bic		r8, r8, #O_FLAG
			ldrb	r12, [r10, r1]
			bic		r8, r8, #&ff
			orr		r8, r12, r8
	MEND

	MACRO
$label	ADC8	$d, $s
$label		and		r1, r8, #C_FLAG
			add		r1, r1, $s
			add		r1, r1, $d
			bic		r8, r8, #O_FLAG
			eor		r12, r1, $d
			eor		r2, r1, $s
			and		r2, r12, r2
			eor		r12, r12, $s
			and		r2, r2, #&80
			and		r12, r12, #A_FLAG
			orr		r8, r8, r2 lsl #4
			ldrb	r2, [r10, r1]
			bic		r8, r8, #&ff
			orr		r2, r12, r2
			orr		r8, r2, r8
	MEND

	MACRO
$label	SBB8	$d, $s
$label		and		r12, r8, #C_FLAG
			sub		r1, $d, $s
			bic		r8, r8, #O_FLAG
			sub		r1, r1, r12
			eor		r12, $s, $d
			eor		r2, r1, $d
			and		r2, r12, r2
			eor		r12, r1, r12
			and		r2, r2, #&80
			and		r12, r12, #A_FLAG
			orr		r8, r8, r2 lsl #4
			mov		r2, r1, lsl #(32 - 9)
			ldrb	r2, [r10, r2 lsr #(32 - 9)]
			bic		r8, r8, #&ff
			orr		r2, r12, r2
			orr		r8, r2, r8
	MEND

	MACRO
$label	AND8	$d, $s
$label		and		r1, $d, $s
			bic		r8, r8, #O_FLAG
			ldrb	r12, [r10, r1]
			bic		r8, r8, #&ff
			orr		r8, r12, r8
	MEND

	MACRO
$label	SUB8	$d, $s
$label		mov		r1, $d lsl #24
			subs	r1, r1, $s lsl #24
			orrvs	r8, r8, #O_FLAG
			mov		r1, r1 lsr #24
			bicvc	r8, r8, #O_FLAG
			eor		r12, r1, $s
			ldrb	r2, [r10, r1]
			eor		r12, r12, $d
			addcc	r2, r2, #C_FLAG
			and		r12, r12, #A_FLAG
			orr		r2, r2, r12
			bic		r8, r8, #&ff
			add		r8, r8, r2
	MEND

	MACRO
$label	XOR8	$d, $s
$label		eor		r1, $d, $s
			bic		r8, r8, #O_FLAG
			ldrb	r12, [r10, r1]
			bic		r8, r8, #&ff
			orr		r8, r12, r8
	MEND

	MACRO
$label	ADD16	$d, $s
$label		mov		r1, $s lsl #16
			adds	r1, r1, $d lsl #16
			orrvs	r8, r8, #O_FLAG
			mov		r1, r1 lsr #16
			bicvc	r8, r8, #O_FLAG
			and		r3, r1, #&ff
			eor		r12, r1, $d
			ldrb	r2, [r10, r3]
			eor		r12, r12, $s
			and		r2, r2, #P_FLAG
			addcs	r2, r2, #C_FLAG
			addeq	r2, r2, #Z_FLAG
			addmi	r2, r2, #S_FLAG
			and		r12, r12, #A_FLAG
			bic		r8, r8, #&ff
			orr		r3, r2, r12
			add		r8, r3, r8
	MEND

	MACRO
$label	OR16	$d, $s
$label		orr		r1, $d, $s
			bic		r8, r8, #O_FLAG
			and		r2, r1, #&ff
			ldrb	r12, [r10, r2]
			bic		r8, r8, #&ff
			and		r12, r12, #P_FLAG
			movs	r2, r1, lsl #16
			addeq	r12, r12, #Z_FLAG
			addmi	r12, r12, #S_FLAG
			orr		r8, r12, r8
	MEND

	MACRO
$label	ADC16	$d, $s
$label		and		r1, r8, #C_FLAG
			add		r1, r1, $s
			add		r1, r1, $d
			bic		r8, r8, #O_FLAG
			eor		r12, r1, $d
			eor		r2, r1, $s
			and		r2, r12, r2
			eor		r12, r12, $s
			and		r2, r2, #&8000
			and		r12, r12, #A_FLAG
			orr		r8, r8, r2 lsr #4
			and		r2, r1, #&ff
			movs	r3, r1 lsl #16
			addeq	r12, r12, #Z_FLAG
			addmi	r12, r12, #S_FLAG
			ldrb	r2, [r10, r2]
			bic		r8, r8, #&ff
			and		r2, r2, #P_FLAG
			orr		r2, r12, r2
			orr		r8, r2, r8
			tst		r1, #&10000
			orrne	r8, r8, #C_FLAG
	MEND

	MACRO
$label	SBB16	$d, $s
$label		and		r12, r8, #C_FLAG
			sub		r1, $d, $s
			bic		r8, r8, #&ff
			subs	r1, r1, r12
			addmi	r8, r8, #C_FLAG
			eor		r12, $s, $d
			eor		r2, r1, $d
			bic		r8, r8, #O_FLAG
			and		r2, r12, r2
			eor		r12, r1, r12
			and		r3, r1, #&ff
			and		r2, r2, #&8000
			ldrb	r3, [r10, r3]
			orr		r8, r8, r2 lsr #4
			and		r2, r3, #P_FLAG
			and		r12, r12, #A_FLAG
			movs	r3, r1 lsl #16
			addeq	r12, r12, #Z_FLAG
			addmi	r12, r12, #S_FLAG
			orr		r2, r12, r2
			orr		r8, r2, r8
	MEND

	MACRO
$label	AND16	$d, $s
$label		and		r1, $d, $s
			bic		r8, r8, #O_FLAG
			and		r2, r1, #&ff
			ldrb	r12, [r10, r2]
			bic		r8, r8, #&ff
			and		r12, r12, #P_FLAG
			movs	r2, r1, lsl #16
			addeq	r12, r12, #Z_FLAG
			addmi	r12, r12, #S_FLAG
			orr		r8, r12, r8
	MEND

	MACRO
$label	SUB16	$d, $s
$label		mov		r1, $d lsl #16
			subs	r1, r1, $s lsl #16
			orrvs	r8, r8, #O_FLAG
			mov		r1, r1 lsr #16
			bicvc	r8, r8, #O_FLAG
			and		r3, r1, #&ff
			eor		r12, r1, $d
			ldrb	r2, [r10, r3]
			eor		r12, r12, $s
			and		r2, r2, #P_FLAG
			addcc	r2, r2, #C_FLAG
			addeq	r2, r2, #Z_FLAG
			addmi	r2, r2, #S_FLAG
			and		r12, r12, #A_FLAG
			bic		r8, r8, #&ff
			orr		r3, r2, r12
			add		r8, r3, r8
	MEND

	MACRO
$label	XOR16	$d, $s
$label		eor		r1, $d, $s
			bic		r8, r8, #O_FLAG
			and		r2, r1, #&ff
			ldrb	r12, [r10, r2]
			bic		r8, r8, #&ff
			and		r12, r12, #P_FLAG
			movs	r2, r1, lsl #16
			addeq	r12, r12, #Z_FLAG
			addmi	r12, r12, #S_FLAG
			orr		r8, r12, r8
	MEND

	MACRO
$label	INC8	$s
$label		add		r1, $s, #1
			bic		r8, r8, #O_FLAG
			eor		r12, r1, $s
			bic		r8, r8, #(&ff - C_FLAG)
			and		r2, r12, r1
			and		r12, r12, #A_FLAG
			and		r3, r1, #&ff
			tst		r2, #&80
			orrne	r8, r8, #O_FLAG
			ldrb	r2, [r10, r3]
			orr		r8, r8, r12
			orr		r8, r8, r2
	MEND

	MACRO
$label	INC16	$s
$label		add		r1, $s, #1
			bic		r8, r8, #O_FLAG
			eor		r12, r1, $s
			bic		r8, r8, #(&ff - C_FLAG)
			and		r2, r12, r1
			and		r12, r12, #A_FLAG
			and		r3, r1, #&ff
			tst		r2, #&8000
			orrne	r8, r8, #O_FLAG
			ldrb	r2, [r10, r3]
			movs	r3, r1, lsl #16
			orreq	r12, r12, #Z_FLAG
			orrmi	r12, r12, #S_FLAG
			and		r2, r2, #P_FLAG
			orr		r8, r8, r12
			orr		r8, r8, r2
	MEND

	MACRO
$label	DEC8	$s
$label		sub		r1, $s, #1
			bic		r8, r8, #O_FLAG
			eor		r12, r1, $s
			bic		r8, r8, #(&ff - C_FLAG)
			and		r2, r12, $s
			and		r12, r12, #A_FLAG
			and		r3, r1, #&ff
			tst		r2, #&80
			orrne	r8, r8, #O_FLAG
			ldrb	r2, [r10, r3]
			orr		r8, r8, r12
			orr		r8, r8, r2
	MEND

	MACRO
$label	DEC16	$s
$label		sub		r1, $s, #1
			bic		r8, r8, #O_FLAG
			eor		r12, r1, $s
			bic		r8, r8, #(&ff - C_FLAG)
			and		r2, r12, $s
			and		r12, r12, #A_FLAG
			and		r3, r1, #&ff
			tst		r2, #&8000
			orrne	r8, r8, #O_FLAG
			ldrb	r2, [r10, r3]
			movs	r3, r1 lsl #16
			orreq	r12, r12, #Z_FLAG
			orrmi	r12, r12, #S_FLAG
			and		r2, r2, #P_FLAG
			orr		r8, r8, r12
			orr		r8, r8, r2
	MEND


	MACRO
$label	NOT8	$s
$label		mvn		r1, $s
	MEND

	MACRO
$label	NOT16	$s
$label		mvn		r1, $s
	MEND


	MACRO
$label	NEG8	$s
$label		rsb		r1, $s, #0
			bic		r8, r8, #O_FLAG
			and		r12, r1, $s
			eor		r2, r1, $s
			bic		r8, r8, #&ff
			and		r12, r12, #&80
			and		r2, r2, #A_FLAG
			orr		r8, r8, r12 lsl #4
			mov		r12, r1 lsl #(32 - 9)
			orr		r8, r8, r2
			ldrb	r2, [r10, r12 lsr #(32 - 9)]
			orr		r8, r2, r8
	MEND

	MACRO
$label	NEG16	$s
$label		bic		r8, r8, #&ff
			rsbs	r1, $s, #0
			addmi	r8, r8, #C_FLAG
			and		r12, r1, $s
			bic		r8, r8, #O_FLAG
			eor		r2, r1, $s
			and		r12, r12, #&8000
			and		r2, r2, #A_FLAG
			orr		r8, r8, r12 lsr #4
			and		r12, r1, #&ff
			orr		r8, r8, r2
			ldrb	r2, [r10, r12]
			and		r2, r2, #P_FLAG
			orr		r8, r2, r8
			movs	r12, r1, lsl #16
			orreq	r8, r8, #Z_FLAG
			orrmi	r8, r8, #S_FLAG
	MEND


	MACRO
$label	MUL8	$d, $s
			mul		r1, $d, $s
			movs	r12, r1 lsr #8
			biceq	r8, r8, #O_FLAG
			biceq	r8, r8, #C_FLAG
			orrne	r8, r8, #O_FLAG
			orrne	r8, r8, #C_FLAG
	MEND

	MACRO
$label	MUL16	$d, $s
			mul		r1, $d, $s
			movs	r12, r1 lsr #16
			biceq	r8, r8, #O_FLAG
			biceq	r8, r8, #C_FLAG
			orrne	r8, r8, #O_FLAG
			orrne	r8, r8, #C_FLAG
	MEND

	MACRO
$label	IMUL8	$d, $s
			mov		r1, $d lsl #24
			mov		r12, $s lsl #24
			mov		r1, r1 asr #24
			mov		r12, r12 asr #24
			mul		r1, r12, r1
			add		r12, r1, #&80
			movs	r12, r12 lsr #8
			biceq	r8, r8, #O_FLAG
			biceq	r8, r8, #C_FLAG
			orrne	r8, r8, #O_FLAG
			orrne	r8, r8, #C_FLAG
	MEND

	MACRO
$label	IMUL16	$d, $s
			mov		r1, $d lsl #16
			mov		r12, $s lsl #16
			mov		r1, r1 asr #16
			mov		r12, r12 asr #16
			mul		r1, r12, r1
			add		r12, r1, #&8000
			movs	r12, r12 lsr #16
			biceq	r8, r8, #O_FLAG
			biceq	r8, r8, #C_FLAG
			orrne	r8, r8, #O_FLAG
			orrne	r8, r8, #C_FLAG
	MEND

	MACRO
$label	REGPUSH	$r, $clk
$label		CPUWORK	$clk
			ldrh	r0, [r9, #CPU_SP]
			ldrh	r1, [r9, $r]
			ldr		r2, [r9, #CPU_SS_BASE]
			sub		r0, r0, #2
			mov		r3, r0 lsl #16
			strh	r0, [r9, #CPU_SP]
			add		r0, r2, r3 lsr #16
			mov		lr, r11
			b		i286a_memorywrite_w
	MEND

	MACRO
$label	POP		$clk
$label		ldrh	r1, [r9, #CPU_SP]
			ldr		r0, [r9, #CPU_SS_BASE]
			CPUWORK	$clk
			add		r2, r1, #2
			add		r0, r1, r0
			strh	r2, [r9, #CPU_SP]
			bl		i286a_memoryread_w
	MEND

	MACRO
$label	REGPOP	$r, $clk
$label		POP		$clk
			strh	r0, [r9, $r]
			mov		pc, r11
	MEND

	END

