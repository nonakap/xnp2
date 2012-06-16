
; ---- cgwindow

	IMPORT	cgwindow

CGW_LOW				equ		0
CGW_HIGH			equ		4
CGW_WRITABLE		equ		8


; ---- dmac

	IMPORT	dmac

; DMACH_adrs		equ		0
; DMACH_leng		equ		4
; DMACH_adrsorg		equ		6
; DMACH_lengorg		equ		8
; DMACH_action		equ		10
; DMACH_outproc		equ		12
; DMACH_inproc		equ		16
; DMACH_extproc		equ		20
; DMACH_mode		equ		24
; DMACH_sreq		equ		25
; DMACH_ready		equ		26
; DMACH_mask		equ		27
DMACH_SIZE			equ		28

; dmac_dmach		equ		0
; dmac_lh			equ		(DMACH_SIZE * 4) + 0
; dmac_work			equ		(DMACH_SIZE * 4) + 4
DMAC_WORKING		equ		(DMACH_SIZE * 4) + 5
; dmac_mask			equ		(DMACH_SIZE * 4) + 6
; dmac_stat			equ		(DMACH_SIZE * 4) + 7
; dmac_devices		equ		(DMACH_SIZE * 4) + 8
; dmac_device		equ		(DMACH_SIZE * 4) + 12


; ---- egc

	IMPORT	egc

; egc_access		equ		0
; egc_fgbg			equ		2
; egc_ope			equ		4
; egc_fg			equ		6
; egc_mask			equ		8
; egc_bg			equ		10
EGC_SFT				equ		12
; egc_leng			equ		14
; egc_lastvram		equ		16
; egc_patreg		equ		24
; egc_fgc			equ		32
; egc_bgc			equ		40
;							and more..


; ---- gdc

	IMPORT	gdcs

; gdcs_access		equ		0
; gdcs_disp			equ		1
GDCS_TEXTDISP		equ		2
GDCS_MSWACC			equ		3
GDCS_GRPHDISP		equ		4
; gdcs_palchange	equ		5
; gdcs_mode2		equ		6


; ---- grcg

	IMPORT	grcg

; grcg_counter		equ		0
; grcg_mode			equ		4
GRCG_MODEREG		equ		6
GRCG_TILE			equ		8
; grcg_gdcwithgrcg	equ		16
; grcg_chip			equ		20


; ---- pic

	IMPORT	pic

; PI_icw			equ		0
PI_IMR				equ		4
; PI_isr			equ		5
; PI_irr			equ		6
; PI_ocw3			equ		7
; PI_pry			equ		8
; PI_writeicw		equ		9
; PI_padding		equ		10
PI_SIZE				equ		12

PIC_MASTER			equ		0
PIC_SLAVE			equ		PI_SIZE

	MACRO
$label	NOINTREXIT
$label		tst		r8, #I_FLAG
			moveq	pc, r11
			ldr		r2, [r0, #(PIC_MASTER + PI_IMR)]
			mov		r12, #(&ff << 24)
			ldr		r3, [r0, #(PIC_SLAVE + PI_IMR)]
			eor		r0, r12, r2 lsl #24
			tst		r0, r2 lsl #8
			eoreq	r0, r12, r3 lsl #24
			tsteq	r0, r3 lsl #8
			moveq	pc, r11
	MEND

	MACRO
$label	PICEXISTINTR
$label		ldr		r2, [r0, #(PIC_MASTER + PI_IMR)]
			mov		r12, #(&ff << 24)
			ldr		r3, [r0, #(PIC_SLAVE + PI_IMR)]
			eor		r0, r12, r2 lsl #24
			tst		r0, r2 lsl #8
			eoreq	r0, r12, r3 lsl #24
			tsteq	r0, r3 lsl #8
	MEND

	END

