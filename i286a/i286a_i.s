
	INCLUDE		i286a.inc

	IMPORT		i286acore
	IMPORT		i286a_memoryread
	IMPORT		i286a_memorywrite_w
	EXPORT		i286a_localint
	EXPORT		i286a_trapint
;;	IMPORT		i286a_trapintr
;;	EXPORT		i286c_interrupt
	EXPORT		i286a_interrupt

	AREA	.text, CODE, READONLY

				; r6 - num / r8 - IP / r11 - ret
i286a_localint	ldrh	r4, [r9, #CPU_SP]
				ldr		r5, [r9, #CPU_SS_BASE]
				CPUWORK	#20
				mov		r4, r4 lsl #16
				sub		r4, r4, #(2 << 16)
				mov		r1, r8
				add		r0, r5, r4 lsr #16
				bl		i286a_memorywrite_w
				sub		r4, r4, #(2 << 16)
				ldrh	r1, [r9, #CPU_CS]
				add		r0, r5, r4 lsr #16
				bl		i286a_memorywrite_w
				sub		r4, r4, #(2 << 16)
				mov		r1, r8 lsr #16
				mov		r4, r4 lsr #16
				sub		r8, r8, r1 lsl #16
				add		r0, r4, r5
				strh	r4, [r9, #CPU_SP]
				bl		i286a_memorywrite_w
			;;	mov		r0, #0
				ldr		r1, [r9, r6 lsl #2]
			;;	strb	r0, [r9, #CPU_TRAP]
				bic		r8, r8, #(T_FLAG + I_FLAG)
				mov		r0, r1 lsr #16
				orr		r8, r8, r1 lsl #16
				mov		r2, r0 lsl #4
				strh	r0, [r9, #CPU_CS]
				str		r2, [r9, #CPU_CS_BASE]
				mov		pc, r11

i286a_trapint	mov		r6, lr
				ldrh	r4, [r9, #CPU_SP]
				ldr		r5, [r9, #CPU_SS_BASE]
				CPUWORK	#20
				mov		r4, r4 lsl #16
				sub		r4, r4, #(2 << 16)
				mov		r1, r8
				add		r0, r5, r4 lsr #16
				bl		i286a_memorywrite_w
				sub		r4, r4, #(2 << 16)
				ldrh	r1, [r9, #CPU_CS]
				add		r0, r5, r4 lsr #16
				bl		i286a_memorywrite_w
				sub		r4, r4, #(2 << 16)
				mov		r1, r8 lsr #16
				mov		r4, r4 lsr #16
				sub		r8, r8, r1 lsl #16
				add		r0, r4, r5
				strh	r4, [r9, #CPU_SP]
				bl		i286a_memorywrite_w
			;;	mov		r0, #0
				ldr		r1, [r9, #4]
			;;	strb	r0, [r9, #CPU_TRAP]
				bic		r8, r8, #(T_FLAG + I_FLAG)
				mov		r0, r1 lsr #16
				orr		r8, r8, r1 lsl #16
				mov		r2, r0 lsl #4
				strh	r0, [r9, #CPU_CS]
				str		r2, [r9, #CPU_CS_BASE]
				mov		pc, r6

;; i286c_interrupt
i286a_interrupt	ldr		r1, iai_r9
				stmdb	sp!, {r4 - r9, lr}
				mov		r9, r1
				mov		r6, r0
				CPULD
				ldr		r5, [r9, #CPU_CS_BASE]
				ldrh	r4, [r9, #CPU_SP]
				CPUWORK	#20
				add		r0, r5, r8 lsr #16
				ldr		r5, [r9, #CPU_SS_BASE]
				bl		i286a_memoryread
				mov		r4, r4 lsl #16
				cmp		r0, #&f4
				addeq	r8, r8, #(1 << 16)
				sub		r4, r4, #(2 << 16)
				mov		r1, r8
				add		r0, r5, r4 lsr #16
				bl		i286a_memorywrite_w
				sub		r4, r4, #(2 << 16)
				ldrh	r1, [r9, #CPU_CS]
				add		r0, r5, r4 lsr #16
				bl		i286a_memorywrite_w
				sub		r4, r4, #(2 << 16)
				mov		r1, r8 lsr #16
				mov		r4, r4 lsr #16
				sub		r8, r8, r1 lsl #16
				add		r0, r4, r5
				strh	r4, [r9, #CPU_SP]
				bl		i286a_memorywrite_w
			;;	mov		r0, #0
				ldr		r1, [r9, r6 lsl #2]
			;;	strb	r0, [r9, #CPU_TRAP]
				bic		r8, r8, #(T_FLAG + I_FLAG)
				mov		r0, r1 lsr #16
				orr		r8, r8, r1 lsl #16
				mov		r2, r0 lsl #4
				strh	r0, [r9, #CPU_CS]
				str		r2, [r9, #CPU_CS_BASE]
				CPUSV
				ldmia	sp!, {r4 - r9, pc}
iai_r9			dcd		i286acore + CPU_SIZE

	END

