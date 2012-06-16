
	INCLUDE		i286a.inc
	INCLUDE		i286aalu.inc

	IMPORT		iflags

	IMPORT		i286a_memoryread
	IMPORT		i286a_memoryread_w
	IMPORT		i286a_memorywrite
	IMPORT		i286a_memorywrite_w

	IMPORT		iocore_inp8
	IMPORT		iocore_inp16
	IMPORT		iocore_out8
	IMPORT		iocore_out16

	EXPORT		i286a_rep_insb
	EXPORT		i286a_rep_insw
	EXPORT		i286a_rep_outsb
	EXPORT		i286a_rep_outsw
	EXPORT		i286a_rep_movsb
	EXPORT		i286a_rep_movsw
	EXPORT		i286a_rep_lodsb
	EXPORT		i286a_rep_lodsw
	EXPORT		i286a_rep_stosb
	EXPORT		i286a_rep_stosw
	EXPORT		i286a_repe_cmpsb
	EXPORT		i286a_repe_cmpsw
	EXPORT		i286a_repne_cmpsb
	EXPORT		i286a_repne_cmpsw
	EXPORT		i286a_repe_scasb
	EXPORT		i286a_repe_scasw
	EXPORT		i286a_repne_scasb
	EXPORT		i286a_repne_scasw

	AREA	.text, CODE, READONLY

i286a_rep_insb	ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]			; DI:SI
				cmp		r4, #0
				moveq	pc, r11
				CPUSVF
				mov		r6, #&10000
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repinsblp		ldrh	r0, [r9, #CPU_DX]
				CPUSVC
				bl		iocore_inp8
				CPULDC
				ldr		r2, [r9, #CPU_ES_BASE]
				mov		r1, r0
				CPUWORK	#4
				add		r0, r2, r5 lsr #16
				bl		i286a_memorywrite
				add		r5, r5, r6
				subs	r4, r4, #1
				bne		repinsblp
				strh	r4, [r9, #CPU_CX]
				str		r5, [r9, #CPU_SI]
				CPULDF
				mov		pc, r11

i286a_rep_insw	ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]			; DI:SI
				cmp		r4, #0
				moveq	pc, r11
				CPUSVF
				mov		r6, #&20000
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repinswlp		ldrh	r0, [r9, #CPU_DX]
				CPUSVC
				bl		iocore_inp16
				CPULDC
				ldr		r2, [r9, #CPU_ES_BASE]
				mov		r1, r0
				CPUWORK	#4
				add		r0, r2, r5 lsr #16
				bl		i286a_memorywrite_w
				add		r5, r5, r6
				subs	r4, r4, #1
				bne		repinswlp
				strh	r4, [r9, #CPU_CX]
				str		r5, [r9, #CPU_SI]
				CPULDF
				mov		pc, r11


i286a_rep_outsb	ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldrh	r5, [r9, #CPU_SI]
				cmp		r4, #0
				moveq	pc, r11
				CPUSVF
				mov		r5, r5 lsl #16
				ldr		r2, [r9, #CPU_DS_FIX]
				mov		r6, #&10000
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repoutsblp		CPUWORK	#4
				add		r0, r2, r5 lsr #16
				bl		i286a_memoryread
				mov		r1, r0
				ldrh	r0, [r9, #CPU_DX]
				add		r5, r5, r6
				CPUSVC
				bl		iocore_out8
				CPULDC
				subs	r4, r4, #1
				ldrne	r2, [r9, #CPU_DS_FIX]
				bne		repoutsblp
				mov		r5, r5 lsr #16
				strh	r4, [r9, #CPU_CX]
				strh	r5, [r9, #CPU_SI]
				CPULDF
				mov		pc, r11

i286a_rep_outsw	ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldrh	r5, [r9, #CPU_SI]
				cmp		r4, #0
				moveq	pc, r11
				CPUSVF
				mov		r5, r5 lsl #16
				ldr		r2, [r9, #CPU_DS_FIX]
				mov		r6, #&20000
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repoutswlp		CPUWORK	#4
				add		r0, r2, r5 lsr #16
				bl		i286a_memoryread_w
				mov		r1, r0
				ldrh	r0, [r9, #CPU_DX]
				add		r5, r5, r6
				CPUSVC
				bl		iocore_out16
				CPULDC
				subs	r4, r4, #1
				ldrne	r2, [r9, #CPU_DS_FIX]
				bne		repoutswlp
				mov		r5, r5 lsr #16
				strh	r4, [r9, #CPU_CX]
				strh	r5, [r9, #CPU_SI]
				CPULDF
				mov		pc, r11


i286a_rep_movsb	ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]			; DI:SI
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				ldr		r2, [r9, #CPU_DS_FIX]
				mov		r11, r5 lsl #16
				mov		r6, #&10000
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repmovsblp		add		r0, r2, r11 lsr #16
				bl		i286a_memoryread
				ldr		r2, [r9, #CPU_ES_BASE]
				mov		r1, r0
				add		r11, r11, r6
				add		r0, r2, r5 lsr #16
				bl		i286a_memorywrite
				add		r5, r5, r6
				CPUWKS	#4
				blt		repmovsbbreak
				ldr		r2, [r9, #CPU_DS_FIX]
				subs	r4, r4, #1
				bne		repmovsblp
				mov		r5, r5 lsr #16
				mov		r11, r11 lsr #16
				orr		r0, r11, r5 lsl #16
				strh	r4, [r9, #CPU_CX]
				str		r0, [r9, #CPU_SI]
				ldr		pc, [sp], #4
repmovsbbreak	ldrb	r1, [r9, #CPU_PREFIX]
				mov		r5, r5 lsr #16
				mov		r11, r11 lsr #16
				add		r1, r1, #1
				subs	r4, r4, #1
				orr		r0, r11, r5 lsl #16
				subne	r8, r8, r1 lsl #16
				strh	r4, [r9, #CPU_CX]
				str		r0, [r9, #CPU_SI]
				ldr		pc, [sp], #4

i286a_rep_movsw	ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]			; DI:SI
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				ldr		r2, [r9, #CPU_DS_FIX]
				mov		r11, r5 lsl #16
				mov		r6, #&20000
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repmovswlp		add		r0, r2, r11 lsr #16
				bl		i286a_memoryread_w
				ldr		r2, [r9, #CPU_ES_BASE]
				mov		r1, r0
				add		r11, r11, r6
				add		r0, r2, r5 lsr #16
				bl		i286a_memorywrite_w
				add		r5, r5, r6
				CPUWKS	#4
				blt		repmovswbreak
				subs	r4, r4, #1
				ldr		r2, [r9, #CPU_DS_FIX]
				bne		repmovswlp
				mov		r5, r5 lsr #16
				mov		r11, r11 lsr #16
				orr		r0, r11, r5 lsl #16
				strh	r4, [r9, #CPU_CX]
				str		r0, [r9, #CPU_SI]
				ldr		pc, [sp], #4
repmovswbreak	ldrb	r1, [r9, #CPU_PREFIX]
				mov		r5, r5 lsr #16
				mov		r11, r11 lsr #16
				add		r1, r1, #1
				orr		r0, r11, r5 lsl #16
				subs	r4, r4, #1
				subne	r8, r8, r1 lsl #16
				strh	r4, [r9, #CPU_CX]
				str		r0, [r9, #CPU_SI]
				ldr		pc, [sp], #4


i286a_rep_lodsb	ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldrh	r5, [r9, #CPU_SI]
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				mov		r6, #&10000
				mov		r5, r5 lsl #16
				ldr		r11, [r9, #CPU_DS_FIX]
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
replodsblp		add		r0, r11, r5 lsr #16
				add		r5, r5, r6
				bl		i286a_memoryread
				CPUWORK	#4
				subs	r4, r4, #1
				bne		replodsblp
				strb	r0, [r9, #CPU_AL]
				mov		r5, r5 lsr #16
				strh	r4, [r9, #CPU_CX]
				strh	r5, [r9, #CPU_SI]
				ldr		pc, [sp], #4

i286a_rep_lodsw	ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldrh	r5, [r9, #CPU_SI]
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				mov		r6, #&20000
				mov		r5, r5 lsl #16
				ldr		r11, [r9, #CPU_DS_FIX]
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
replodswlp		add		r0, r11, r5 lsr #16
				add		r5, r5, r6
				bl		i286a_memoryread_w
				CPUWORK	#4
				subs	r4, r4, #1
				bne		replodswlp
				strh	r0, [r9, #CPU_AX]
				mov		r5, r5 lsr #16
				strh	r4, [r9, #CPU_CX]
				strh	r5, [r9, #CPU_SI]
				ldr		pc, [sp], #4


i286a_rep_stosb	ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#4
				ldr		r5, [r9, #CPU_SI]
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				mov		r6, #&10000
				ldr		r11, [r9, #CPU_ES_BASE]
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repstosblp		ldrb	r1, [r9, #CPU_AL]
				add		r0, r11, r5 lsr #16
				add		r5, r5, r6
				bl		i286a_memorywrite
				CPUWKS	#3
				ble		repstosbbreak
				subs	r4, r4, #1
				bne		repstosblp
				strh	r4, [r9, #CPU_CX]
				str		r5, [r9, #CPU_SI]
				ldr		pc, [sp], #4
repstosbbreak	ldrb	r1, [r9, #CPU_PREFIX]
				str		r5, [r9, #CPU_SI]
				subs	r4, r4, #1
				add		r1, r1, #1
				subne	r8, r8, r1 lsl #16
				strh	r4, [r9, #CPU_CX]
				ldr		pc, [sp], #4

i286a_rep_stosw	ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#4
				ldr		r5, [r9, #CPU_SI]
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				mov		r6, #&20000
				ldr		r11, [r9, #CPU_ES_BASE]
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repstoswlp		ldrh	r1, [r9, #CPU_AX]
				add		r0, r11, r5 lsr #16
				add		r5, r5, r6
				bl		i286a_memorywrite_w
				CPUWKS	#3
				blt		repstoswbreak
				subs	r4, r4, #1
				bne		repstoswlp
				strh	r4, [r9, #CPU_CX]
				str		r5, [r9, #CPU_SI]
				ldr		pc, [sp], #4
repstoswbreak	ldrb	r1, [r9, #CPU_PREFIX]
				str		r5, [r9, #CPU_SI]
				subs	r4, r4, #1
				add		r1, r1, #1
				subne	r8, r8, r1 lsl #16
				strh	r4, [r9, #CPU_CX]
				ldr		pc, [sp], #4


i286a_repe_cmpsb
				ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]			; DI:SI
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				ldr		r2, [r9, #CPU_DS_FIX]
				mov		r11, r5 lsl #16
				mov		r6, #&10000
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repecmpsblp		add		r0, r2, r11 lsr #16
				bl		i286a_memoryread
				ldr		r2, [r9, #CPU_ES_BASE]
				mov		r10, r0 lsl #24
				add		r11, r11, r6
				add		r0, r2, r5 lsr #16
				bl		i286a_memoryread
				add		r5, r5, r6
				CPUWORK	#9
				ldr		r2, [r9, #CPU_DS_FIX]
				subs	r4, r4, #1
				beq		repecmpsbbreak
				cmp		r10, r0 lsl #24
				beq		repecmpsblp
repecmpsbbreak		bic		r8, r8, #&ff
					subs	r1, r10, r0 lsl #24
					eor		r12, r1, r10
					ldr		r10, repecmpsb_flag
					orrvs	r8, r8, #O_FLAG
					bicvc	r8, r8, #O_FLAG
					addcc	r8, r8, #C_FLAG
					ldrb	r2, [r10, r1 lsr #24]
					eor		r12, r1, r0 lsl #24
					and		r12, r12, #(A_FLAG << 24)
					orr		r8, r8, r2
					orr		r8, r8, r12 lsr #24
				mov		r5, r5 lsr #16
				mov		r11, r11 lsr #16
				orr		r0, r11, r5 lsl #16
				strh	r4, [r9, #CPU_CX]
				str		r0, [r9, #CPU_SI]
				ldr		pc, [sp], #4
repecmpsb_flag	dcd		iflags

i286a_repe_cmpsw
				ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]			; DI:SI
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				ldr		r2, [r9, #CPU_DS_FIX]
				mov		r11, r5 lsl #16
				mov		r6, #&20000
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repecmpswlp		add		r0, r2, r11 lsr #16
				bl		i286a_memoryread_w
				ldr		r2, [r9, #CPU_ES_BASE]
				mov		r10, r0 lsl #16
				add		r11, r11, r6
				add		r0, r2, r5 lsr #16
				bl		i286a_memoryread_w
				add		r5, r5, r6
				CPUWORK	#9
				ldr		r2, [r9, #CPU_DS_FIX]
				subs	r4, r4, #1
				beq		repecmpswbreak
				cmp		r10, r0 lsl #16
				beq		repecmpswlp
repecmpswbreak		bic		r8, r8, #&ff
					subs	r1, r10, r0 lsl #16
					eor		r12, r1, r10
					and		r3, r1, #(&ff << 16)
					ldr		r10, repecmpsw_flag
					orrvs	r8, r8, #O_FLAG
					bicvc	r8, r8, #O_FLAG
					addcc	r8, r8, #C_FLAG
					addmi	r8, r8, #S_FLAG
					addeq	r8, r8, #Z_FLAG
					ldrb	r2, [r10, r3 lsr #16]
					eor		r12, r1, r0 lsl #16
					and		r12, r12, #(A_FLAG << 16)
					and		r2, r2, #P_FLAG
					orr		r8, r2, r8
					orr		r8, r8, r12 lsr #16
				mov		r5, r5 lsr #16
				mov		r11, r11 lsr #16
				orr		r0, r11, r5 lsl #16
				strh	r4, [r9, #CPU_CX]
				str		r0, [r9, #CPU_SI]
				ldr		pc, [sp], #4
repecmpsw_flag	dcd		iflags


i286a_repne_cmpsb
				ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]			; DI:SI
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				ldr		r2, [r9, #CPU_DS_FIX]
				mov		r11, r5 lsl #16
				mov		r6, #&10000
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repnecmpsblp	add		r0, r2, r11 lsr #16
				bl		i286a_memoryread
				ldr		r2, [r9, #CPU_ES_BASE]
				mov		r10, r0 lsl #24
				add		r11, r11, r6
				add		r0, r2, r5 lsr #16
				bl		i286a_memoryread
				add		r5, r5, r6
				CPUWORK	#9
				ldr		r2, [r9, #CPU_DS_FIX]
				subs	r4, r4, #1
				beq		repnecmpsbbreak
				cmp		r10, r0 lsl #24
				bne		repnecmpsblp
repnecmpsbbreak		bic		r8, r8, #&ff
					subs	r1, r10, r0 lsl #24
					eor		r12, r1, r10
					ldr		r10, repnecmpsb_flag
					orrvs	r8, r8, #O_FLAG
					bicvc	r8, r8, #O_FLAG
					addcc	r8, r8, #C_FLAG
					ldrb	r2, [r10, r1 lsr #24]
					eor		r12, r1, r0 lsl #24
					and		r12, r12, #(A_FLAG << 24)
					orr		r8, r8, r2
					orr		r8, r8, r12 lsr #24
				mov		r5, r5 lsr #16
				mov		r11, r11 lsr #16
				orr		r0, r11, r5 lsl #16
				strh	r4, [r9, #CPU_CX]
				str		r0, [r9, #CPU_SI]
				ldr		pc, [sp], #4
repnecmpsb_flag	dcd		iflags

i286a_repne_cmpsw
				ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]			; DI:SI
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				ldr		r2, [r9, #CPU_DS_FIX]
				mov		r11, r5 lsl #16
				mov		r6, #&20000
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repnecmpswlp	add		r0, r2, r11 lsr #16
				bl		i286a_memoryread_w
				ldr		r2, [r9, #CPU_ES_BASE]
				mov		r10, r0 lsl #16
				add		r11, r11, r6
				add		r0, r2, r5 lsr #16
				bl		i286a_memoryread_w
				add		r5, r5, r6
				CPUWORK	#9
				ldr		r2, [r9, #CPU_DS_FIX]
				subs	r4, r4, #1
				beq		repnecmpswbreak
				cmp		r10, r0 lsl #16
				bne		repnecmpswlp
repnecmpswbreak		bic		r8, r8, #&ff
					subs	r1, r10, r0 lsl #16
					eor		r12, r1, r10
					and		r3, r1, #(&ff << 16)
					ldr		r10, repnecmpsw_flag
					orrvs	r8, r8, #O_FLAG
					bicvc	r8, r8, #O_FLAG
					addcc	r8, r8, #C_FLAG
					addmi	r8, r8, #S_FLAG
					addeq	r8, r8, #Z_FLAG
					ldrb	r2, [r10, r3 lsr #16]
					eor		r12, r1, r0 lsl #16
					and		r12, r12, #(A_FLAG << 16)
					and		r2, r2, #P_FLAG
					orr		r8, r2, r8
					orr		r8, r8, r12 lsr #16
				mov		r5, r5 lsr #16
				mov		r11, r11 lsr #16
				orr		r0, r11, r5 lsl #16
				strh	r4, [r9, #CPU_CX]
				str		r0, [r9, #CPU_SI]
				ldr		pc, [sp], #4
repnecmpsw_flag	dcd		iflags


i286a_repe_scasb
				ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				mov		r6, #&10000
				ldr		r11, [r9, #CPU_ES_BASE]
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repescasblp		add		r0, r11, r5 lsr #16
				add		r5, r5, r6
				bl		i286a_memoryread
				ldrb	r2, [r9, #CPU_AL]
				CPUWORK	#8
				subs	r4, r4, #1
				beq		repescasbbreak
				cmp		r2, r0
				beq		repescasblp
repescasbbreak	SUB8	r2, r0
				strh	r4, [r9, #CPU_CX]
				str		r5, [r9, #CPU_SI]
				ldr		pc, [sp], #4

i286a_repe_scasw
				ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				mov		r6, #&20000
				ldr		r11, [r9, #CPU_ES_BASE]
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repescaswlp		add		r0, r11, r5 lsr #16
				add		r5, r5, r6
				bl		i286a_memoryread_w
				ldrh	r2, [r9, #CPU_AX]
				CPUWORK	#8
				subs	r4, r4, #1
				beq		repescaswbreak
				cmp		r2, r0
				beq		repescaswlp
repescaswbreak	SUB16	r2, r0
				strh	r4, [r9, #CPU_CX]
				str		r5, [r9, #CPU_SI]
				ldr		pc, [sp], #4


i286a_repne_scasb
				ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				mov		r6, #&10000
				ldr		r11, [r9, #CPU_ES_BASE]
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repnescasblp	add		r0, r11, r5 lsr #16
				add		r5, r5, r6
				bl		i286a_memoryread
				ldrb	r2, [r9, #CPU_AL]
				CPUWORK	#8
				subs	r4, r4, #1
				beq		repnescasbbreak
				cmp		r2, r0
				bne		repnescasblp
repnescasbbreak	SUB8	r2, r0
				strh	r4, [r9, #CPU_CX]
				str		r5, [r9, #CPU_SI]
				ldr		pc, [sp], #4

i286a_repne_scasw
				ldrh	r4, [r9, #CPU_CX]
				CPUWORK	#5
				ldr		r5, [r9, #CPU_SI]
				cmp		r4, #0
				moveq	pc, r11
				str		r11, [sp, #-4]!
				mov		r6, #&20000
				ldr		r11, [r9, #CPU_ES_BASE]
				tst		r8, #D_FLAG
				rsbne	r6, r6, #0
repnescaswlp	add		r0, r11, r5 lsr #16
				add		r5, r5, r6
				bl		i286a_memoryread_w
				ldrh	r2, [r9, #CPU_AX]
				CPUWORK	#8
				subs	r4, r4, #1
				beq		repnescaswbreak
				cmp		r2, r0
				bne		repnescaswlp
repnescaswbreak	SUB16	r2, r0
				strh	r4, [r9, #CPU_CX]
				str		r5, [r9, #CPU_SI]
				ldr		pc, [sp], #4

	END

