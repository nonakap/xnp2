
; r4 - tmp
; r7 - clock
; r8 - IP/flag
; r9 - i286core/mem
; r10 - iflag
; r11 - ret


	INCLUDE	i286a.inc
	IMPORT	i286a_memoryread
	IMPORT	i286a_memoryread_w
;;	IMPORT	ea_assert

	EXPORT	i286a_selector
	EXPORT	i286a_ea
	EXPORT	i286a_lea
	EXPORT	i286a_a

	AREA	.text, CODE, READONLY


i286a_selector	stmdb	sp!, {r4 - r5, lr}
				tst		r0, #4
				moveq	r1, #(CPU_GDTR + 2)
				movne	r1, #(CPU_LDTRC + 2)
				add		r2, r1, #2
				ldrh	r1, [r1, r9]
				ldrb	r2, [r2, r9]
				bic		r4, r0, #7
				add		r4, r4, r1
				add		r4, r4, r2 lsl #16
				add		r0, r4, #2
				bl		i286a_memoryread_w
				mov		r5, r0
				add		r0, r4, #4
				bl		i286a_memoryread
				add		r0, r5, r0 lsl #16
				ldmia	sp!, {r4 - r5, pc}


; ---- calc_ea_dst

	MACRO
$label	EAR1	$r, $b
$label		ldrh	r1, [r9, $r]
			ldr		r2, [r9, $b]
			add		r0, r2, r1
			mov		pc, lr
	MEND

	MACRO
$label	EAR1D8	$r, $b
$label	;;	ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r5, r8 lsr #16
			mov		r4, lr
			bl		i286a_memoryread
			ldrh	r1, [r9, $r]
			ldr		r2, [r9, $b]
			mov		r3, r0 lsl #24
			mov		r12, r1 lsl #16
			add		r1, r12, r3 asr #8
			add		r8, r8, #(1 << 16)
			add		r0, r2, r1 lsr #16
			mov		pc, r4
	MEND

	MACRO
$label	EAR1D16	$r, $b
$label	;;	ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r5, r8 lsr #16
			mov		r4, lr
			bl		i286a_memoryread_w
			ldrh	r1, [r9, $r]
			ldr		r2, [r9, $b]
			add		r8, r8, #(2 << 16)
			add		r3, r1, r0
			bic		r1, r3, #(1 << 16)
			add		r0, r1, r2
			mov		pc, r4
	MEND

	MACRO
$label	EAR2	$r1, $r2, $b
$label		ldrh	r1, [r9, $r1]
			ldrh	r2, [r9, $r2]
			ldr		r3, [r9, $b]
			add		r12, r2, r1
			bic		r12, r12, #(1 << 16)
			add		r0, r12, r3
			mov		pc, lr
	MEND

	MACRO
$label	EAR2D8	$r1, $r2, $b
$label	;;	ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r5, r8 lsr #16
			mov		r4, lr
			bl		i286a_memoryread
			ldrh	r1, [r9, $r1]
			ldrh	r2, [r9, $r2]
			mov		r12, r0 lsl #24
			ldr		r3, [r9, $b]
			mov		r12, r12 asr #8
			add		r12, r12, r1 lsl #16
			add		r12, r12, r2 lsl #16
			add		r8, r8, #(1 << 16)
			add		r0, r3, r12 lsr #16
			mov		pc, r4
	MEND

	MACRO
$label	EAR2D16	$r1, $r2, $b
$label	;;	ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r5, r8 lsr #16
			mov		r4, lr
			bl		i286a_memoryread_w
			ldrh	r1, [r9, $r1]
			ldrh	r2, [r9, $r2]
			ldr		r3, [r9, $b]
			add		r12, r1, r0
			add		r8, r8, #(2 << 16)
			add		r12, r12, r2
			bic		r12, r12, #(3 << 16)
			add		r0, r12, r3
			mov		pc, r4
	MEND

i286a_ea
	if 0
				ldr		r1, [r9, #CPU_CS_BASE]
				cmp		r1, r5
				beq		ea_r
				str		r0, [sp, #-4]!
				mov		r4, lr
				add		r0, r1, r8 lsr #16
				bl		ea_assert
				ldr		r0, [sp], #4
				mov		lr, r4
ea_r
	endif
				and		r1, r0, #(&18 << 3)
				and		r2, r0, #7
				add		r3, pc, r1 lsr #1
				add		pc, r3, r2 lsl #2

				b		ea_bx_si
				b		ea_bx_di
				b		ea_bp_si
				b		ea_bp_di
				b		ea_si
				b		ea_di
				b		ea_d16
				b		ea_bx

				b		ea_bx_si_d8
				b		ea_bx_di_d8
				b		ea_bp_si_d8
				b		ea_bp_di_d8
				b		ea_si_d8
				b		ea_di_d8
				b		ea_bp_d8
				b		ea_bx_d8

				b		ea_bx_si_d16
				b		ea_bx_di_d16
				b		ea_bp_si_d16
				b		ea_bp_di_d16
				b		ea_si_d16
				b		ea_di_d16
				b		ea_bp_d16
				b		ea_bx_d16

ea_bx_si		EAR2	#CPU_BX, #CPU_SI, #CPU_DS_FIX
ea_bx_si_d8		EAR2D8	#CPU_BX, #CPU_SI, #CPU_DS_FIX
ea_bx_si_d16	EAR2D16	#CPU_BX, #CPU_SI, #CPU_DS_FIX
ea_bx_di		EAR2	#CPU_BX, #CPU_DI, #CPU_DS_FIX
ea_bx_di_d8		EAR2D8	#CPU_BX, #CPU_DI, #CPU_DS_FIX
ea_bx_di_d16	EAR2D16	#CPU_BX, #CPU_DI, #CPU_DS_FIX
ea_bp_si		EAR2	#CPU_BP, #CPU_SI, #CPU_SS_FIX
ea_bp_si_d8		EAR2D8	#CPU_BP, #CPU_SI, #CPU_SS_FIX
ea_bp_si_d16	EAR2D16	#CPU_BP, #CPU_SI, #CPU_SS_FIX
ea_bp_di		EAR2	#CPU_BP, #CPU_DI, #CPU_SS_FIX
ea_bp_di_d8		EAR2D8	#CPU_BP, #CPU_DI, #CPU_SS_FIX
ea_bp_di_d16	EAR2D16	#CPU_BP, #CPU_DI, #CPU_SS_FIX
ea_si			EAR1	#CPU_SI, #CPU_DS_FIX
ea_si_d8		EAR1D8	#CPU_SI, #CPU_DS_FIX
ea_si_d16		EAR1D16	#CPU_SI, #CPU_DS_FIX
ea_di			EAR1	#CPU_DI, #CPU_DS_FIX
ea_di_d8		EAR1D8	#CPU_DI, #CPU_DS_FIX
ea_di_d16		EAR1D16	#CPU_DI, #CPU_DS_FIX
ea_bx			EAR1	#CPU_BX, #CPU_DS_FIX
ea_bx_d8		EAR1D8	#CPU_BX, #CPU_DS_FIX
ea_bx_d16		EAR1D16	#CPU_BX, #CPU_DS_FIX
ea_bp_d8		EAR1D8	#CPU_BP, #CPU_SS_FIX
ea_bp_d16		EAR1D16	#CPU_BP, #CPU_SS_FIX

ea_d16		;;	ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r5, r8 lsr #16
				mov		r4, lr
				bl		i286a_memoryread_w
				ldr		r1, [r9, #CPU_DS_FIX]
				add		r8, r8, #(2 << 16)
				add		r0, r0, r1
				mov		pc, r4


; ---- calc_lea

	MACRO
$label	LER1	$r
$label		ldrh	r0, [r9, $r]
			mov		pc, lr
	MEND

	MACRO
$label	LER1D8	$r
$label		add		r0, r5, r8 lsr #16
			mov		r4, lr
			bl		i286a_memoryread
			ldrh	r1, [r9, $r]
			add		r8, r8, #(1 << 16)
			tst		r0, #&80
			orrne	r0, r0, #&ff00
			add		r0, r0, r1
			bic		r0, r0, #(1 << 16)
			mov		pc, r4
	MEND

	MACRO
$label	LER1D16	$r
$label		add		r0, r5, r8 lsr #16
			mov		r4, lr
			bl		i286a_memoryread_w
			ldrh	r1, [r9, $r]
			add		r8, r8, #(2 << 16)
			add		r3, r1, r0
			bic		r0, r3, #(1 << 16)
			mov		pc, r4
	MEND

	MACRO
$label	LER2	$r1, $r2
$label		ldrh	r1, [r9, $r1]
			ldrh	r2, [r9, $r2]
			add		r12, r2, r1
			bic		r0, r12, #(1 << 16)
			mov		pc, lr
	MEND

	MACRO
$label	LER2D8	$r1, $r2
$label		add		r0, r5, r8 lsr #16
			mov		r4, lr
			bl		i286a_memoryread
			ldrh	r1, [r9, $r1]
			ldrh	r2, [r9, $r2]
			tst		r0, #&80
			orrne	r0, r0, #&ff00
			add		r0, r0, r1
			add		r0, r0, r2
			bic		r0, r0, #(3 << 16)
			add		r8, r8, #(1 << 16)
			mov		pc, r4
	MEND

	MACRO
$label	LER2D16	$r1, $r2
$label		add		r0, r5, r8 lsr #16
			mov		r4, lr
			bl		i286a_memoryread_w
			ldrh	r1, [r9, $r1]
			ldrh	r2, [r9, $r2]
			add		r8, r8, #(2 << 16)
			add		r12, r1, r0
			add		r12, r12, r2
			bic		r0, r12, #(3 << 16)
			mov		pc, r4
	MEND

i286a_lea		and		r1, r0, #(&18 << 3)
				and		r2, r0, #7
				add		r3, pc, r1 lsr #1
				add		pc, r3, r2 lsl #2

				b		lea_bx_si
				b		lea_bx_di
				b		lea_bp_si
				b		lea_bp_di
				b		lea_si
				b		lea_di
				b		lea_d16
				b		lea_bx

				b		lea_bx_si_d8
				b		lea_bx_di_d8
				b		lea_bp_si_d8
				b		lea_bp_di_d8
				b		lea_si_d8
				b		lea_di_d8
				b		lea_bp_d8
				b		lea_bx_d8

				b		lea_bx_si_d16
				b		lea_bx_di_d16
				b		lea_bp_si_d16
				b		lea_bp_di_d16
				b		lea_si_d16
				b		lea_di_d16
				b		lea_bp_d16
				b		lea_bx_d16

lea_bx_si		LER2	#CPU_BX, #CPU_SI
lea_bx_si_d8	LER2D8	#CPU_BX, #CPU_SI
lea_bx_si_d16	LER2D16	#CPU_BX, #CPU_SI
lea_bx_di		LER2	#CPU_BX, #CPU_DI
lea_bx_di_d8	LER2D8	#CPU_BX, #CPU_DI
lea_bx_di_d16	LER2D16	#CPU_BX, #CPU_DI
lea_bp_si		LER2	#CPU_BP, #CPU_SI
lea_bp_si_d8	LER2D8	#CPU_BP, #CPU_SI
lea_bp_si_d16	LER2D16	#CPU_BP, #CPU_SI
lea_bp_di		LER2	#CPU_BP, #CPU_DI
lea_bp_di_d8	LER2D8	#CPU_BP, #CPU_DI
lea_bp_di_d16	LER2D16	#CPU_BP, #CPU_DI
lea_si			LER1	#CPU_SI
lea_si_d8		LER1D8	#CPU_SI
lea_si_d16		LER1D16	#CPU_SI
lea_di			LER1	#CPU_DI
lea_di_d8		LER1D8	#CPU_DI
lea_di_d16		LER1D16	#CPU_DI
lea_bx			LER1	#CPU_BX
lea_bx_d8		LER1D8	#CPU_BX
lea_bx_d16		LER1D16	#CPU_BX
lea_bp_d8		LER1D8	#CPU_BP
lea_bp_d16		LER1D16	#CPU_BP

lea_d16			add		r0, r5, r8 lsr #16
				add		r8, r8, #(2 << 16)
				b		i286a_memoryread_w


; ---- calc_a

	MACRO
$label	AR1		$r, $b
$label		ldrh	r0, [r9, $r]
			ldr		r6, [r9, $b]
			mov		pc, lr
	MEND

	MACRO
$label	AR1D8	$r, $b
$label		ldr		r0, [r9, #CPU_CS_BASE]
			mov		r4, lr
			add		r0, r0, r8 lsr #16
			bl		i286a_memoryread
			ldrh	r1, [r9, $r]
			ldr		r6, [r9, $b]
			add		r8, r8, #(1 << 16)
			tst		r0, #&80
			orrne	r0, r0, #&ff00
			add		r0, r0, r1
			bic		r0, r0, #(1 << 16)
			mov		pc, r4
	MEND

	MACRO
$label	AR1D16	$r, $b
$label		ldr		r0, [r9, #CPU_CS_BASE]
			mov		r4, lr
			add		r0, r0, r8 lsr #16
			bl		i286a_memoryread_w
			ldrh	r1, [r9, $r]
			ldr		r6, [r9, $b]
			add		r8, r8, #(2 << 16)
			add		r3, r1, r0
			bic		r0, r3, #(1 << 16)
			mov		pc, r4
	MEND

	MACRO
$label	AR2		$r1, $r2, $b
$label		ldrh	r1, [r9, $r1]
			ldrh	r2, [r9, $r2]
			ldr		r6, [r9, $b]
			add		r12, r2, r1
			bic		r0, r12, #(1 << 16)
			mov		pc, lr
	MEND

	MACRO
$label	AR2D8	$r1, $r2, $b
$label		ldr		r0, [r9, #CPU_CS_BASE]
			mov		r4, lr
			add		r0, r0, r8 lsr #16
			bl		i286a_memoryread
			ldrh	r1, [r9, $r1]
			ldrh	r2, [r9, $r2]
			ldr		r6, [r9, $b]
			tst		r0, #&80
			orrne	r0, r0, #&ff00
			add		r0, r0, r1
			add		r0, r0, r2
			bic		r0, r0, #(3 << 16)
			add		r8, r8, #(1 << 16)
			mov		pc, r4
	MEND

	MACRO
$label	AR2D16	$r1, $r2, $b
$label		ldr		r0, [r9, #CPU_CS_BASE]
			mov		r4, lr
			add		r0, r0, r8 lsr #16
			bl		i286a_memoryread_w
			ldrh	r1, [r9, $r1]
			ldrh	r2, [r9, $r2]
			ldr		r6, [r9, $b]
			add		r8, r8, #(2 << 16)
			add		r12, r1, r0
			add		r12, r12, r2
			bic		r0, r12, #(3 << 16)
			mov		pc, r4
	MEND

i286a_a			and		r1, r0, #(&18 << 3)
				and		r2, r0, #7
				add		r3, pc, r1 lsr #1
				add		pc, r3, r2 lsl #2

				b		a_bx_si
				b		a_bx_di
				b		a_bp_si
				b		a_bp_di
				b		a_si
				b		a_di
				b		a_d16
				b		a_bx

				b		a_bx_si_d8
				b		a_bx_di_d8
				b		a_bp_si_d8
				b		a_bp_di_d8
				b		a_si_d8
				b		a_di_d8
				b		a_bp_d8
				b		a_bx_d8

				b		a_bx_si_d16
				b		a_bx_di_d16
				b		a_bp_si_d16
				b		a_bp_di_d16
				b		a_si_d16
				b		a_di_d16
				b		a_bp_d16
				b		a_bx_d16

a_bx_si			AR2		#CPU_BX, #CPU_SI, #CPU_DS_FIX
a_bx_si_d8		AR2D8	#CPU_BX, #CPU_SI, #CPU_DS_FIX
a_bx_si_d16		AR2D16	#CPU_BX, #CPU_SI, #CPU_DS_FIX
a_bx_di			AR2		#CPU_BX, #CPU_DI, #CPU_DS_FIX
a_bx_di_d8		AR2D8	#CPU_BX, #CPU_DI, #CPU_DS_FIX
a_bx_di_d16		AR2D16	#CPU_BX, #CPU_DI, #CPU_DS_FIX
a_bp_si			AR2		#CPU_BP, #CPU_SI, #CPU_SS_FIX
a_bp_si_d8		AR2D8	#CPU_BP, #CPU_SI, #CPU_SS_FIX
a_bp_si_d16		AR2D16	#CPU_BP, #CPU_SI, #CPU_SS_FIX
a_bp_di			AR2		#CPU_BP, #CPU_DI, #CPU_SS_FIX
a_bp_di_d8		AR2D8	#CPU_BP, #CPU_DI, #CPU_SS_FIX
a_bp_di_d16		AR2D16	#CPU_BP, #CPU_DI, #CPU_SS_FIX
a_si			AR1		#CPU_SI, #CPU_DS_FIX
a_si_d8			AR1D8	#CPU_SI, #CPU_DS_FIX
a_si_d16		AR1D16	#CPU_SI, #CPU_DS_FIX
a_di			AR1		#CPU_DI, #CPU_DS_FIX
a_di_d8			AR1D8	#CPU_DI, #CPU_DS_FIX
a_di_d16		AR1D16	#CPU_DI, #CPU_DS_FIX
a_bx			AR1		#CPU_BX, #CPU_DS_FIX
a_bx_d8			AR1D8	#CPU_BX, #CPU_DS_FIX
a_bx_d16		AR1D16	#CPU_BX, #CPU_DS_FIX
a_bp_d8			AR1D8	#CPU_BP, #CPU_SS_FIX
a_bp_d16		AR1D16	#CPU_BP, #CPU_SS_FIX

a_d16			ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				ldr		r6, [r9, #CPU_DS_FIX]
				add		r8, r8, #(2 << 16)
				b		i286a_memoryread_w

	END

