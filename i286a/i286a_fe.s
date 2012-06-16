
	INCLUDE		i286a.inc
	INCLUDE		i286aea.inc
	INCLUDE		i286aalu.inc
	INCLUDE		i286aop.inc

	IMPORT		i286a_ea
	IMPORT		i286a_a
	IMPORT		i286a_memoryread
	IMPORT		i286a_memoryread_w
	IMPORT		i286a_memorywrite
	IMPORT		i286a_memorywrite_w
	IMPORT		i286a_localint

	EXPORT		i286aopfe
	EXPORT		i286aopff

	AREA	.text, CODE, READONLY

i286aopfe		GETPCF8
				tst		r0, #(1 << 3)
				bne		decea8
incea8			OP_EA8	INC8, #2, #7
decea8			OP_EA8	DEC8, #2, #7


; ----

i286aopff		GETPCF8
				and		r12, r0, #(7 << 3)
				adr		r1, opefftbl
				ldr		pc, [r1, r12 lsr #1]
opefftbl		dcd		incea16
				dcd		decea16
				dcd		callea16
				dcd		callfarea16
				dcd		jmpea16
				dcd		jmpfarea16
				dcd		pushea16
				dcd		popea16

incea16			OP_EA16	INC16, #2, #7
decea16			OP_EA16	DEC16, #2, #7

callea16		cmp		r0, #&c0
				bcc		call16m
				CPUWORK	#7
				R16SRC	r0, r5
				ldrh	r0, [r5, #CPU_REG]
				mov		r1, r8 lsr #16
				sub		r8, r8, r1 lsl #16
				orr		r8, r8, r0 lsl #16
				b		call16e
call16m			CPUWORK	#11
				bl		i286a_ea
				bl		i286a_memoryread_w
				mov		r1, r8 lsr #16
				sub		r8, r8, r1 lsl #16
				orr		r8, r8, r0 lsl #16
call16e			ldrh	r2, [r9, #CPU_SP]
				ldr		r3, [r9, #CPU_SS_BASE]
				subs	r2, r2, #2
				addcc	r2, r2, #&10000
				strh	r2, [r9, #CPU_SP]
				add		r0, r2, r3
				mov		lr, r11
				b		i286a_memorywrite_w

callfarea16		cmp		r0, #&c0
				bcs		callfar16r
				CPUWORK	#16
				bl		i286a_a
				add		r4, r0, #2
				add		r0, r0, r6
				bic		r4, r4, #(1 << 16)
				bl		i286a_memoryread_w			; ip
				mov		r5, r8 lsr #16
				add		r8, r8, r0 lsl #16
				sub		r8, r8, r5 lsl #16
				add		r0, r4, r6
				bl		i286a_memoryread_w			; cs
				mov		r2, r0 lsl #4
				ldrh	r1, [r9, #CPU_CS]
				strh	r0, [r9, #CPU_CS]
				ldrh	r4, [r9, #CPU_SP]
				ldr		r6, [r9, #CPU_SS_BASE]
				str		r2, [r9, #CPU_CS_BASE]
				subs	r4, r4, #2
				addcc	r4, r4, #&10000
				add		r0, r4, r6
				bl		i286a_memorywrite_w			; cs
				subs	r4, r4, #2
				addcc	r4, r4, #&10000
				mov		r1, r5
				strh	r4, [r9, #CPU_SP]
				add		r0, r4, r6
				mov		lr, r11
				b		i286a_memorywrite_w			; ip
callfar16r		mov		r6, #6
				sub		r8, r8, #(2 << 16)
				b		i286a_localint

jmpea16			cmp		r0, #&c0
				bcc		jmp16m
				CPUWORK	#7
				R16SRC	r0, r5
				ldrh	r0, [r5, #CPU_REG]
				mov		r8, r8 lsl #16
				mov		r8, r8 lsr #16
				orr		r8, r8, r0 lsl #16
				mov		pc, r11
jmp16m			CPUWORK	#11
				bl		i286a_ea
				bl		i286a_memoryread_w
				mov		r8, r8 lsl #16
				mov		r8, r8 lsr #16
				orr		r8, r8, r0 lsl #16
				mov		pc, r11

jmpfarea16		cmp		r0, #&c0
				bcs		jmpfar16r
				CPUWORK	#11
				bl		i286a_a
				add		r4, r0, #2
				add		r0, r0, r6
				bic		r4, r4, #(1 << 16)
				bl		i286a_memoryread_w
				mov		r8, r8 lsl #16
				mov		r8, r8 lsr #16
				orr		r8, r8, r0 lsl #16
				add		r0, r4, r6
				bl		i286a_memoryread_w
				mov		r1, r0 lsl #4
				strh	r0, [r9, #CPU_CS]
				str		r1, [r9, #CPU_CS_BASE]
				mov		pc, r11
jmpfar16r		mov		r6, #6
				sub		r8, r8, #(2 << 16)
				b		i286a_localint

pushea16		cmp		r0, #&c0
				bcc		push16m
				CPUWORK	#3
				R16SRC	r0, r5
				ldrh	r1, [r5, #CPU_REG]
				b		push16e
push16m			CPUWORK	#5
				bl		i286a_ea
				bl		i286a_memoryread_w
				mov		r1, r0
push16e			ldrh	r2, [r9, #CPU_SP]
				ldr		r3, [r9, #CPU_SS_BASE]
				subs	r2, r2, #2
				addcc	r2, r2, #&10000
				strh	r2, [r9, #CPU_SP]
				add		r0, r2, r3
				mov		lr, r11
				b		i286a_memorywrite_w

popea16			CPUWORK	#5
				ldrh	r2, [r9, #CPU_SP]
				ldr		r3, [r9, #CPU_SS_BASE]
				mov		r4, r0
				add		r0, r2, r3
				add		r2, r2, #2
				bic		r2, r2, #&10000
				strh	r2, [r9, #CPU_SP]
				bl		i286a_memoryread_w
				cmp		r4, #&c0
				bcc		pop16m
				R16SRC	r4, r1
				strh	r0, [r1, #CPU_REG]
				mov		pc, r11
pop16m			mov		r6, r0
				mov		r0, r4
				bl		i286a_ea
				mov		r1, r6
				mov		lr, r11
				b		i286a_memoryread_w

	END

