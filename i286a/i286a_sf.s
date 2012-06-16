
	INCLUDE		i286a.inc
	INCLUDE		i286aea.inc
	INCLUDE		i286asft.inc

	IMPORT		i286a_ea
	IMPORT		i286a_memoryread
	IMPORT		i286a_memoryread_w
	IMPORT		i286a_memorywrite
	IMPORT		i286a_memorywrite_w

	EXPORT		i286asft8_1
	EXPORT		i286asft16_1
	EXPORT		i286asft8_cl
	EXPORT		i286asft8_d8
	EXPORT		i286asft16_cl
	EXPORT		i286asft16_d8

	AREA	.text, CODE, READONLY

i286asft8_1		GETPCF8
				and		r6, r0, #(7 << 3)
				cmp		r0, #&c0
				bcc		sft8m
				CPUWORK	#2
				R8SRC	r0, r5
				add		r5, r5, #CPU_REG
				adr		r1, sft_reg8
				ldrb	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft8m			CPUWORK	#7
				bl		i286a_ea
				cmp		r0, #I286_MEMWRITEMAX
				bcs		sft8e
				add		r5, r9, r0
				adr		r1, sft_reg8
				ldrb	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft8e			mov		r5, r0
				bl		i286a_memoryread
				adr		r1, sft_ext8
				ldr		pc, [r1, r6 lsr #1]

sft_reg8		dcd		rol_r8_1
				dcd		ror_r8_1
				dcd		rcl_r8_1
				dcd		rcr_r8_1
				dcd		shl_r8_1
				dcd		shr_r8_1
				dcd		shl_r8_1
				dcd		sar_r8_1

sft_ext8		dcd		rol_e8_1
				dcd		ror_e8_1
				dcd		rcl_e8_1
				dcd		rcr_e8_1
				dcd		shl_e8_1
				dcd		shr_e8_1
				dcd		shl_e8_1
				dcd		sar_e8_1

rol_r8_1		ROL8	r4
				strb	r1, [r5]
				mov		pc, r11
ror_r8_1		ROR8	r4
				strb	r1, [r5]
				mov		pc, r11
rcl_r8_1		RCL8	r4
				strb	r1, [r5]
				mov		pc, r11
rcr_r8_1		RCR8	r4
				strb	r1, [r5]
				mov		pc, r11
shl_r8_1		SHL8	r4
				strb	r1, [r5]
				mov		pc, r11
shr_r8_1		SHR8	r4
				strb	r1, [r5]
				mov		pc, r11
sar_r8_1		SAR8	r4
				strb	r1, [r5]
				mov		pc, r11

rol_e8_1		ROL8	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
ror_e8_1		ROR8	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
rcl_e8_1		RCL8	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
rcr_e8_1		RCR8	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
shl_e8_1		SHL8	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
shr_e8_1		SHR8	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
sar_e8_1		SAR8	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite


; ----

i286asft16_1	GETPCF8
				and		r6, r0, #(7 << 3)
				cmp		r0, #&c0
				bcc		sft16m
				CPUWORK	#2
				R16SRC	r0, r5
				add		r5, r5, #CPU_REG
				adr		r1, sft_reg16
				ldrh	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft16m			CPUWORK	#7
				bl		i286a_ea
				ACCWORD	r0, sft16e
				add		r5, r9, r0
				adr		r1, sft_reg16
				ldrh	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft16e			mov		r5, r0
				bl		i286a_memoryread_w
				adr		r1, sft_ext16
				ldr		pc, [r1, r6 lsr #1]

sft_reg16		dcd		rol_r16_1
				dcd		ror_r16_1
				dcd		rcl_r16_1
				dcd		rcr_r16_1
				dcd		shl_r16_1
				dcd		shr_r16_1
				dcd		shl_r16_1
				dcd		sar_r16_1

sft_ext16		dcd		rol_e16_1
				dcd		ror_e16_1
				dcd		rcl_e16_1
				dcd		rcr_e16_1
				dcd		shl_e16_1
				dcd		shr_e16_1
				dcd		shl_e16_1
				dcd		sar_e16_1

rol_r16_1		ROL16	r4
				strh	r1, [r5]
				mov		pc, r11
ror_r16_1		ROR16	r4
				strh	r1, [r5]
				mov		pc, r11
rcl_r16_1		RCL16	r4
				strh	r1, [r5]
				mov		pc, r11
rcr_r16_1		RCR16	r4
				strh	r1, [r5]
				mov		pc, r11
shl_r16_1		SHL16	r4
				strh	r1, [r5]
				mov		pc, r11
shr_r16_1		SHR16	r4
				strh	r1, [r5]
				mov		pc, r11
sar_r16_1		SAR16	r4
				strh	r1, [r5]
				mov		pc, r11

rol_e16_1		ROL16	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
ror_e16_1		ROR16	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
rcl_e16_1		RCL16	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
rcr_e16_1		RCR16	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
shl_e16_1		SHL16	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
shr_e16_1		SHR16	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
sar_e16_1		SAR16	r0
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w


; ----

i286asft8_cl	GETPCF8
				and		r6, r0, #(7 << 3)
				cmp		r0, #&c0
				bcc		sft8clm
				CPUWORK	#2
				R8SRC	r0, r5
				ldrb	r0, [r9, #CPU_CL]
				ands	r0, r0, #&1f
				moveq	pc, r11
				add		r5, r5, #CPU_REG
				adr		r1, sft_reg8cl
				ldrb	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft8clm			CPUWORK	#7
				bl		i286a_ea
				cmp		r0, #I286_MEMWRITEMAX
				bcs		sft8cle
				add		r5, r9, r0
				ldrb	r0, [r9, #CPU_CL]
				ands	r0, r0, #&1f
				moveq	pc, r11
				adr		r1, sft_reg8cl
				ldrb	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft8cle			ldrb	r4, [r9, #CPU_CL]
				ands	r4, r4, #&1f
				moveq	pc, r11
				mov		r5, r0
				bl		i286a_memoryread
				adr		r1, sft_ext8cl
				ldr		pc, [r1, r6 lsr #1]

i286asft8_d8	GETPCF8
				and		r6, r0, #(7 << 3)
				cmp		r0, #&c0
				bcc		sft8d8m
				CPUWORK	#2
				R8SRC	r0, r5
				GETPC8
				ands	r0, r0, #&1f
				moveq	pc, r11
				add		r5, r5, #CPU_REG
				adr		r1, sft_reg8cl
				ldrb	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft8d8m			CPUWORK	#7
				bl		i286a_ea
				cmp		r0, #I286_MEMWRITEMAX
				bcs		sft8d8e
				add		r5, r9, r0
				GETPC8
				ands	r0, r0, #&1f
				moveq	pc, r11
				adr		r1, sft_reg8cl
				ldrb	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft8d8e			mov		r5, r0
				GETPC8
				ands	r4, r0, #&1f
				moveq	pc, r11
				mov		r0, r5
				bl		i286a_memoryread
				adr		r1, sft_ext8cl
				ldr		pc, [r1, r6 lsr #1]

sft_reg8cl		dcd		rol_r8_cl
				dcd		ror_r8_cl
				dcd		rcl_r8_cl
				dcd		rcr_r8_cl
				dcd		shl_r8_cl
				dcd		shr_r8_cl
				dcd		shl_r8_cl
				dcd		sar_r8_cl

sft_ext8cl		dcd		rol_e8_cl
				dcd		ror_e8_cl
				dcd		rcl_e8_cl
				dcd		rcr_e8_cl
				dcd		shl_e8_cl
				dcd		shr_e8_cl
				dcd		shl_e8_cl
				dcd		sar_e8_cl

rol_r8_cl		ROL8CL	r4, r0
				strb	r1, [r5]
				mov		pc, r11
ror_r8_cl		ROR8CL	r4, r0
				strb	r1, [r5]
				mov		pc, r11
rcl_r8_cl		RCL8CL	r4, r0
				strb	r1, [r5]
				mov		pc, r11
rcr_r8_cl		RCR8CL	r4, r0
				strb	r1, [r5]
				mov		pc, r11
shl_r8_cl		SHL8CL	r4, r0
				strb	r1, [r5]
				mov		pc, r11
shr_r8_cl		SHR8CL	r4, r0
				strb	r1, [r5]
				mov		pc, r11
sar_r8_cl		SAR8CL	r4, r0
				strb	r1, [r5]
				mov		pc, r11

rol_e8_cl		ROL8CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
ror_e8_cl		ROR8CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
rcl_e8_cl		RCL8CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
rcr_e8_cl		RCR8CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
shl_e8_cl		SHL8CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
shr_e8_cl		SHR8CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite
sar_e8_cl		SAR8CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite


; ----

i286asft16_cl	GETPCF8
				and		r6, r0, #(7 << 3)
				cmp		r0, #&c0
				bcc		sft16clm
				CPUWORK	#5
				R16SRC	r0, r5
				ldrb	r0, [r9, #CPU_CL]
				ands	r0, r0, #&1f
				moveq	pc, r11
				CPUWORK	r0
				add		r5, r5, #CPU_REG
				adr		r1, sft_reg16cl
				ldrh	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft16clm		CPUWORK	#8
				bl		i286a_ea
				ACCWORD	r0, sft16cle
				add		r5, r9, r0
				ldrb	r0, [r9, #CPU_CL]
				ands	r0, r0, #&1f
				moveq	pc, r11
				CPUWORK	r0
				adr		r1, sft_reg16cl
				ldrh	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft16cle		ldrb	r4, [r9, #CPU_CL]
				ands	r4, r4, #&1f
				moveq	pc, r11
				CPUWORK	r4
				mov		r5, r0
				bl		i286a_memoryread_w
				adr		r1, sft_ext16cl
				ldr		pc, [r1, r6 lsr #1]

i286asft16_d8	GETPCF8
				and		r6, r0, #(7 << 3)
				cmp		r0, #&c0
				bcc		sft16d8m
				CPUWORK	#5
				R16SRC	r0, r5
				GETPC8
				ands	r0, r0, #&1f
				moveq	pc, r11
				CPUWORK	r0
				add		r5, r5, #CPU_REG
				adr		r1, sft_reg16cl
				ldrh	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft16d8m		CPUWORK	#8
				bl		i286a_ea
				ACCWORD	r0, sft16d8e
				add		r5, r9, r0
				GETPC8
				ands	r0, r0, #&1f
				moveq	pc, r11
				CPUWORK	r0
				adr		r1, sft_reg16cl
				ldrh	r4, [r5]
				ldr		pc, [r1, r6 lsr #1]
sft16d8e		mov		r5, r0
				GETPC8
				ands	r4, r0, #&1f
				moveq	pc, r11
				CPUWORK	r4
				mov		r0, r5
				bl		i286a_memoryread_w
				adr		r1, sft_ext16cl
				ldr		pc, [r1, r6 lsr #1]

sft_reg16cl		dcd		rol_r16_cl
				dcd		ror_r16_cl
				dcd		rcl_r16_cl
				dcd		rcr_r16_cl
				dcd		shl_r16_cl
				dcd		shr_r16_cl
				dcd		shl_r16_cl
				dcd		sar_r16_cl

sft_ext16cl		dcd		rol_e16_cl
				dcd		ror_e16_cl
				dcd		rcl_e16_cl
				dcd		rcr_e16_cl
				dcd		shl_e16_cl
				dcd		shr_e16_cl
				dcd		shl_e16_cl
				dcd		sar_e16_cl

rol_r16_cl		ROL16CL	r4, r0
				strh	r1, [r5]
				mov		pc, r11
ror_r16_cl		ROR16CL	r4, r0
				strh	r1, [r5]
				mov		pc, r11
rcl_r16_cl		RCL16CL	r4, r0
				strh	r1, [r5]
				mov		pc, r11
rcr_r16_cl		RCR16CL	r4, r0
				strh	r1, [r5]
				mov		pc, r11
shl_r16_cl		SHL16CL	r4, r0
				strh	r1, [r5]
				mov		pc, r11
shr_r16_cl		SHR16CL	r4, r0
				strh	r1, [r5]
				mov		pc, r11
sar_r16_cl		SAR16CL	r4, r0
				strh	r1, [r5]
				mov		pc, r11

rol_e16_cl		ROL16CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
ror_e16_cl		ROR16CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
rcl_e16_cl		RCL16CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
rcr_e16_cl		RCR16CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
shl_e16_cl		SHL16CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
shr_e16_cl		SHR16CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w
sar_e16_cl		SAR16CL	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286a_memorywrite_w

	END

