
	INCLUDE	i286a.inc
	INCLUDE	i286amem.inc
	INCLUDE	i286aio.inc

	IMPORT	vramupdate
	IMPORT	tramupdate

	IMPORT	egc_writebyte
	IMPORT	egc_readbyte
	IMPORT	egc_writeword
	IMPORT	egc_readword

	EXPORT	memfn
	EXPORT	i286_memorymap
	EXPORT	i286_vram_dispatch

	EXPORT	i286_nonram_r
	EXPORT	i286_nonram_rw

	AREA	.data, DATA, READWRITE

memfn	dcd		i286_rdex		; 00
		dcd		i286_rdex
		dcd		i286_rd
		dcd		i286_rd
		dcd		i286_rd			; 20
		dcd		i286_rd
		dcd		i286_rd
		dcd		i286_rd
		dcd		i286_rd			; 40
		dcd		i286_rd
		dcd		i286_rd
		dcd		i286_rd
		dcd		i286_rd			; 60
		dcd		i286_rd
		dcd		i286_rd
		dcd		i286_rd
		dcd		i286_rd			; 80
		dcd		i286_rd
		dcd		i286_rd
		dcd		i286_rd
		dcd		tram_rd			; a0
		dcd		vram_r0
		dcd		vram_r0
		dcd		vram_r0
		dcd		emmc_rd			; c0
		dcd		emmc_rd
		dcd		i286_rd
		dcd		i286_rd
		dcd		vram_r0			; e0
		dcd		i286_rd
		dcd		i286_rd
		dcd		i286_rb

		dcd		i286_wtex		; 00
		dcd		i286_wtex
		dcd		i286_wt
		dcd		i286_wt
		dcd		i286_wt			; 20
		dcd		i286_wt
		dcd		i286_wt
		dcd		i286_wt
		dcd		i286_wt			; 40
		dcd		i286_wt
		dcd		i286_wt
		dcd		i286_wt
		dcd		i286_wt			; 60
		dcd		i286_wt
		dcd		i286_wt
		dcd		i286_wt
		dcd		i286_wt			; 80
		dcd		i286_wt
		dcd		i286_wt
		dcd		i286_wt
		dcd		tram_wt			; a0
		dcd		vram_w0
		dcd		vram_w0
		dcd		vram_w0
		dcd		emmc_wt			; c0
		dcd		emmc_wt
		dcd		i286_wn
		dcd		i286_wn
		dcd		vram_w0			; e0
		dcd		i286_wn
		dcd		i286_wn
		dcd		i286_wn

		dcd		i286w_rdex		; 00
		dcd		i286w_rdex
		dcd		i286w_rd
		dcd		i286w_rd
		dcd		i286w_rd		; 20
		dcd		i286w_rd
		dcd		i286w_rd
		dcd		i286w_rd
		dcd		i286w_rd		; 40
		dcd		i286w_rd
		dcd		i286w_rd
		dcd		i286w_rd
		dcd		i286w_rd		; 60
		dcd		i286w_rd
		dcd		i286w_rd
		dcd		i286w_rd
		dcd		i286w_rd		; 80
		dcd		i286w_rd
		dcd		i286w_rd
		dcd		i286w_rd
		dcd		tramw_rd		; a0
		dcd		vramw_r0
		dcd		vramw_r0
		dcd		vramw_r0
		dcd		emmcw_rd		; c0
		dcd		emmcw_rd
		dcd		i286w_rd
		dcd		i286w_rd
		dcd		vramw_r0		; e0
		dcd		i286w_rd
		dcd		i286w_rd
		dcd		i286w_rb

		dcd		i286w_wtex		; 00
		dcd		i286w_wtex
		dcd		i286w_wt
		dcd		i286w_wt
		dcd		i286w_wt		; 20
		dcd		i286w_wt
		dcd		i286w_wt
		dcd		i286w_wt
		dcd		i286w_wt		; 40
		dcd		i286w_wt
		dcd		i286w_wt
		dcd		i286w_wt
		dcd		i286w_wt		; 60
		dcd		i286w_wt
		dcd		i286w_wt
		dcd		i286w_wt
		dcd		i286w_wt		; 80
		dcd		i286w_wt
		dcd		i286w_wt
		dcd		i286w_wt
		dcd		tramw_wt		; a0
		dcd		vramw_w0
		dcd		vramw_w0
		dcd		vramw_w0
		dcd		emmcw_wt		; c0
		dcd		emmcw_wt
		dcd		i286w_wn
		dcd		i286w_wn
		dcd		vramw_w0		; e0
		dcd		i286w_wn
		dcd		i286w_wn
		dcd		i286w_wn


	AREA	.text, CODE, READONLY

; ---- memory...

i286_rd			ldrb	r0, [r9, r0]
				mov		pc, lr
i286_rdex		ldr		r12, [r9, #CPU_ADRSMASK]
				and		r0, r12, r0
				ldrb	r0, [r9, r0]
				mov		pc, lr

i286w_rd		add		r2, r9, #1
				ldrb	r1, [r9, r0]
				ldrb	r3, [r2, r0]
				orr		r0, r1, r3 lsl #8
				mov		pc, lr
i286w_rdex		ldr		r12, [r9, #CPU_ADRSMASK]
				add		r2, r9, #1
				and		r0, r12, r0
				ldrb	r1, [r9, r0]
				ldrb	r3, [r2, r0]
				orr		r0, r1, r3 lsl #8
				mov		pc, lr

i286_wt			strb	r1, [r9, r0]
				mov		pc, lr
i286_wtex		ldr		r2, [r9, #CPU_ADRSMASK]
				and		r0, r2, r0
				strb	r1, [r9, r0]
				mov		pc, lr

i286w_wt		add		r2, r9, #1
				mov		r3, r1 lsr #8
				strb	r1, [r9, r0]
				strb	r3, [r2, r0]
				mov		pc, lr
i286w_wtex		ldr		r12, [r9, #CPU_ADRSMASK]
				add		r2, r9, #1
				mov		r3, r1 lsr #8
				and		r0, r12, r0
				strb	r1, [r9, r0]
				strb	r3, [r2, r0]
				mov		pc, lr



; ---- text ram

tram_rd		;;	ldr		r3, trd_vramop
				ldrb	r12, [r9, #MEMWAIT_TRAM]
				sub		r1, r0, #&a4000
				cmp		r1, #&1000
				CPUWORK	r12
				ldrcsb	r0, [r0, r9]
				movcs	pc, lr
				ldr		r2, trd_cgwnd
				tst		r0, #1
				ldreq	r3, [r2, #CGW_LOW]
				ldrne	r3, [r2, #CGW_HIGH]
				and		r0, r0, #(&f << 1)
				add		r1, r9, #FONT_ADRS
				add		r3, r1, r3
				ldrb	r0, [r3, r0 lsr #1]
				mov		pc, lr
trd_cgwnd		dcd		cgwindow
;; trd_vramop	dcd		vramop


tramw_rd	;;	ldr		r3, twrd_vramop
				ldrb	r2, [r9, #MEMWAIT_TRAM]
				ldr		r12, twrd_cgwnd
				tst		r0, #1
				CPUWORK	r2
				bne		tramw_rd_odd
				sub		r1, r0, #&a4000
				cmp		r1, #&1000
				ldrcsh	r0, [r0, r9]
				movcs	pc, lr
				ldr		r2, [r12, #CGW_LOW]
				ldr		r3, [r12, #CGW_HIGH]
				and		r0, r0, #(&f << 1)
				add		r1, r9, #FONT_ADRS
				add		r12, r1, r0 lsr #1
				ldrb	r0, [r12, r2]
				ldrb	r1, [r12, r3]
				orr		r0, r0, r1 lsl #8
				mov		pc, lr
twrd_cgwnd		dcd		cgwindow
tramw_rd_odd	add		r1, r0, #1
				cmp		r1, #&a4000
				bcc		tramw_rd_oddt
				beq		tramw_rd_3fff
				cmp		r1, #&a5000
				ble		tramw_rd_oddf
tramw_rd_oddt	ldrb	r0, [r0, r9]
				ldrb	r1, [r1, r9]
				orr		r0, r0, r1 lsl #8
				mov		pc, lr
tramw_rd_3fff	cmp		r1, #(1 << 31)		; set v / clr z
tramw_rd_oddf	ldrvc	r2, [r12, #CGW_HIGH]
				ldrne	r12, [r12, #CGW_LOW]
				addal	r3, r9, #FONT_ADRS
				andvc	r0, r0, #(&f << 1)
				andne	r1, r1, #(&f << 1)
				addvc	r2, r2, r3
				addne	r12, r12, r3
				ldrvcb	r0, [r2, r0 lsr #1]
				ldrvsb	r0, [r0, r9]
				ldrneb	r1, [r12, r1 lsr #1]
				ldreqb	r1, [r1, r9]
				orr		r0, r0, r1 lsl #8
				mov		pc, lr


tram_wt		;;	ldr		r3, twt_vramop
				ldrb	r12, [r9, #MEMWAIT_TRAM]
				cmp		r0, #&a4000
				bcs		twt_cgwnd
				CPUWORK	r12

				ldr		r3, twt_gdcs
				mov		r12, r0, lsl #(31 - 12)
				cmp		r0, #&a2000
				bcc		twt_write
				tst		r0, #1
				movne	pc, lr

twt_attr		cmp		r12, #(&1fe0 << (31 - 12))
				bcc		twt_write
				tst		r0, #2
				beq		twt_write
				ldrb	r2, [r3, #GDCS_MSWACC]
				cmp		r2, #0
				moveq	pc, lr

twt_write		strb	r1, [r0, r9]
twt_dirtyupd	ldr		r2, twt_tramupd
				ldrb	r0, [r3, #GDCS_TEXTDISP]
				mov		r1, #1
				strb	r1, [r2, r12, lsr #(32 - 12)]
				orr		r0, r0, #1
				strb	r0, [r3, #GDCS_TEXTDISP]
				mov		pc, lr


;; twt_vramop	dcd		vramop
twt_gdcs		dcd		gdcs
twt_tramupd		dcd		tramupdate


tramw_wt		ldrb	r12, [r9, #MEMWAIT_TRAM]
				cmp		r0, #&a4000
				bcs		twwt_cgwnd
				CPUWORK	r12
				ldr		r3, twt_gdcs
				mov		r12, r0 lsl #(31 - 12)
				tst		r0, #1
				bne		twwto_main
				cmp		r0, #&a2000
				strcch	r1, [r0, r9]
				bcc		twt_dirtyupd
				bcs		twt_attr

twwto_main		mov		r2, r0
				add		r0, r0, #1
				cmp		r0, #&a2000
				strleb	r1, [r2, r9]
				mov		r1, r1 lsr #8
				bgt		twt_attr
				strb	r1, [r0, r9]
				ldrb	r1, [r3, #GDCS_TEXTDISP]
				ldr		r2, twt_tramupd
				mov		r0, #1
				orr		r1, r1, #1
				strb	r0, [r2, r12 lsr #(32 - 12)]
				add		r12, r12, #(1 << (32 - 12))]
				strb	r1, [r3, #GDCS_TEXTDISP]
				strb	r0, [r2, r12 lsr #(32 - 12)]
				mov		pc, lr


twt_cgwnd		CPUWORK	r12
				ldr		r3, twt_cgwindow
				cmp		r0, #&a5000
				movcs	pc, lr
				ldrb	r12, [r3, #CGW_WRITABLE]
				and		r0, r0, #&1f
				tst		r0, #1
				tstne	r12, #1
				moveq	pc, lr
				orr		r12, r12, #&80
				strb	r12, [r3, #CGW_WRITABLE]
				ldr		r3, [r3, #CGW_HIGH]
				add		r12, r9, #FONT_ADRS
				add		r12, r12, r0 lsr #1
				strb	r1, [r12, r3]
				mov		pc, lr
twt_cgwindow	dcd		cgwindow

twwt_cgwnd		CPUWORK	r12
				ldr		r3, twt_cgwindow
				cmp		r0, #&a5000
				movcs	pc, lr
				ldrb	r12, [r3, #CGW_WRITABLE]
				tst		r0, #1
				moveq	r1, r1 lsr #8
				tst		r12, #1
				moveq	pc, lr
				orr		r12, r12, #&80
				strb	r12, [r3, #CGW_WRITABLE]
				and		r0, r0, #(&f << 1)
				ldr		r3, [r3, #CGW_HIGH]
				add		r12, r9, #FONT_ADRS
				add		r12, r12, r0 lsr #1
				strb	r1, [r12, r3]
				mov		pc, lr



; ---- vram normal

vram_r1			add		r0, r0, #VRAM_STEP
vram_r0			ldrb	r3, [r9, #MEMWAIT_VRAM]
				ldrb	r0, [r0, r9]
				CPUWORK	r3
				mov		pc, lr


vramw_r1		add		r0, r0, #VRAM_STEP
vramw_r0	;;	ldr		r3, twrd_vramop
				add		r2, r0, r9
				ldrb	r3, [r9, #MEMWAIT_VRAM]
				ldrb	r0, [r9, r0]
				ldrb	r1, [r2, #1]
				CPUWORK	r3
				orr		r0, r0, r1 lsl #8
				mov		pc, lr


vram_w0		;;	ldr		r3, vw0_vramop
				ldr		r2, vw0_gdcs
				strb	r1, [r0, r9]
				ldrb	r1, [r9, #MEMWAIT_VRAM]
				ldr		r3, vw0_vramupd
				mov		r0, r0, lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				CPUWORK	r1
				ldrb	r1, [r3, r0, lsr #(32 - 15)]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r1, r1, #1
				strb	r1, [r3, r0, lsr #(32 - 15)]
				mov		pc, lr
;; vw0_vramop	dcd		vramop
vw0_gdcs		dcd		gdcs
vw0_vramupd		dcd		vramupdate

vram_w1			add		r0, r0, #VRAM_STEP
			;;	ldr		r3, vw1_vramop
				ldr		r2, vw1_gdcs
				strb	r1, [r0, r9]
				ldrb	r1, [r9, #MEMWAIT_VRAM]
				ldr		r3, vw1_vramupd
				mov		r0, r0, lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				CPUWORK	r1
				ldrb	r1, [r3, r0, lsr #(32 - 15)]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r1, r1, #2
				strb	r1, [r3, r0, lsr #(32 - 15)]
				mov		pc, lr
;; vw1_vramop	dcd		vramop
vw1_gdcs		dcd		gdcs
vw1_vramupd		dcd		vramupdate


vramw_w0	;;	ldr		r3, vww0_vramop
				ldr		r2, vww0_gdcs
				tst		r0, #1
				bne		vww0_odd
				strh	r1, [r0, r9]
				ldrb	r1, [r9, #MEMWAIT_VRAM]
				ldr		r3, vww0_vramupd
				mov		r0, r0 lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				add		r3, r3, r0 lsr #(32 - 15)
				CPUWORK	r1
				ldrh	r1, [r3]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r1, r1, #1
				orr		r1, r1, #(1 << 8)
				strh	r1, [r3]
				mov		pc, lr
;; vww0_vramop	dcd		vramop
vww0_gdcs		dcd		gdcs
vww0_vramupd	dcd		vramupdate
vww0_odd		add		r12, r0, r9
				strb	r1, [r0, r9]
				mov		r1, r1 lsr #8
				strb	r1, [r12, #1]
				ldrb	r1, [r9, #MEMWAIT_VRAM]
				ldr		r3, vww0_vramupd
				mov		r0, r0, lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				add		r3, r3, r0 lsr #(32 - 15)
				CPUWORK	r1
				ldrb	r0, [r3]
				ldrb	r1, [r3, #1]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r0, r0, #1
				orr		r1, r1, #1
				strb	r0, [r3]
				strb	r1, [r3, #1]
				mov		pc, lr

vramw_w1		add		r0, r0, #VRAM_STEP
			;;	ldr		r3, vww1_vramop
				ldr		r2, vww1_gdcs
				tst		r0, #1
				bne		vww1_odd
				strh	r1, [r0, r9]
				ldrb	r1, [r9, #MEMWAIT_VRAM]
				ldr		r3, vww1_vramupd
				mov		r0, r0 lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				add		r3, r3, r0 lsr #(32 - 15)
				CPUWORK	r1
				ldrh	r1, [r3]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r1, r1, #2
				orr		r1, r1, #(2 << 8)
				strh	r1, [r3]
				mov		pc, lr
;; vww1_vramop	dcd		vramop
vww1_gdcs		dcd		gdcs
vww1_vramupd	dcd		vramupdate
vww1_odd		add		r12, r0, r9
				strb	r1, [r0, r9]
				mov		r1, r1 lsr #8
				strb	r1, [r12, #1]
				ldrb	r1, [r9, #MEMWAIT_VRAM]
				ldr		r3, vww1_vramupd
				mov		r0, r0, lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				add		r3, r3, r0 lsr #(32 - 15)
				CPUWORK	r1
				ldrb	r0, [r3]
				ldrb	r1, [r3, #1]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r0, r0, #2
				orr		r1, r1, #2
				strb	r0, [r3]
				strb	r1, [r3, #1]
				mov		pc, lr



; ---- grcg...

grcg_tcr1		add		r0, r0, #VRAM_STEP
grcg_tcr0		ldr		r3, grw_grcg
				ldrb	r2, [r9, #MEMWAIT_GRCG]
				bic		r0, r0, #&f8000
				ldrb	r12, [r3, #GRCG_MODEREG]
				CPUWORK	r2
				mov		r12, r12 lsl #28
				tst		r12, #(1 << 28)
				add		r0, r0, #VRAM_B
				ldreqb	r1, [r3, #GRCG_TILE + 0]
				ldreqb	r2, [r0, r9]
				eoreq	r1, r2, r1
				orreq	r12, r1, r12
				tst		r12, #(1 << 29)
				add		r0, r0, #(VRAM_R - VRAM_B)
				ldreqb	r1, [r3, #GRCG_TILE + 2]
				ldreqb	r2, [r0, r9]
				eoreq	r1, r2, r1
				orreq	r12, r1, r12
				tst		r12, #(1 << 30)
				add		r0, r0, #(VRAM_G - VRAM_R)
				ldreqb	r1, [r3, #GRCG_TILE + 4]
				ldreqb	r2, [r0, r9]
				eoreq	r1, r2, r1
				orreq	r12, r1, r12
				tst		r12, #(1 << 31)
				add		r0, r0, #(VRAM_E - VRAM_G)
				ldreqb	r1, [r3, #GRCG_TILE + 6]
				ldreqb	r2, [r0, r9]
				eoreq	r1, r2, r1
				orreq	r12, r1, r12
				and		r12, r12, #&ff
				eor		r0, r12, #&ff
				mov		pc, lr
tcr_grcg		dcd		grcg


grcgw_tcr1		add		r0, r0, #VRAM_STEP
grcgw_tcr0		ldr		r3, tcrw_grcg
				ldrb	r2, [r9, #MEMWAIT_GRCG]
				bic		r0, r0, #&f8000
				ldrb	r12, [r3, #GRCG_MODEREG]
				CPUWORK	r2
				tst		r0, #1
				mov		r12, r12 lsl #28
				bne		tcrw_odd
				tst		r12, #(1 << 28)
				add		r0, r0, #VRAM_B
				ldreqh	r1, [r3, #(GRCG_TILE + 0)]
				ldreqh	r2, [r0, r9]
				add		r0, r0, #(VRAM_R - VRAM_B)
				eoreq	r1, r2, r1
				orreq	r12, r1, r12
				tst		r12, #(1 << 29)
				ldreqh	r1, [r3, #(GRCG_TILE + 2)]
				ldreqh	r2, [r0, r9]
				add		r0, r0, #(VRAM_G - VRAM_R)
				eoreq	r1, r2, r1
				orreq	r12, r1, r12
				tst		r12, #(1 << 30)
				ldreqh	r1, [r3, #(GRCG_TILE + 4)]
				ldreqh	r2, [r0, r9]
				add		r0, r0, #(VRAM_E - VRAM_G)
				eoreq	r1, r2, r1
				orreq	r12, r1, r12
				tst		r12, #(1 << 31)
				ldreqh	r1, [r3, #(GRCG_TILE + 6)]
				ldreqh	r2, [r0, r9]
				eoreq	r1, r2, r1
				orreq	r12, r1, r12
				mvn		r12, r12 lsl #16
				mov		r0, r12 lsr #16
				mov		pc, lr
tcrw_grcg		dcd		grcg
tcrw_odd		add		r0, r0, r9
				str		lr, [sp, #-4]!
				add		r0, r0, #VRAM_B
				tst		r12, #(1 << 28)
				ldreqb	r1, [r0]
				ldreqb	r2, [r0, #1]
				ldreqh	lr, [r3, #GRCG_TILE + 0]
				add		r0, r0, #(VRAM_R - VRAM_B)
				orreq	r1, r1, r2 lsl #8
				eoreq	r1, r1, lr
				orreq	r12, r1, r12
				tst		r12, #(1 << 29)
				ldreqb	r1, [r0]
				ldreqb	r2, [r0, #1]
				ldreqh	lr, [r3, #GRCG_TILE + 2]
				add		r0, r0, #(VRAM_G - VRAM_R)
				orreq	r1, r1, r2 lsl #8
				eoreq	r1, r1, lr
				orreq	r12, r1, r12
				tst		r12, #(1 << 30)
				ldreqb	r1, [r0]
				ldreqb	r2, [r0, #1]
				ldreqh	lr, [r3, #GRCG_TILE + 4]
				add		r0, r0, #(VRAM_E - VRAM_G)
				orreq	r1, r1, r2 lsl #8
				eoreq	r1, r1, lr
				orreq	r12, r1, r12
				tst		r12, #(1 << 31)
				ldreqb	r1, [r0]
				ldreqb	r2, [r0, #1]
				ldreqh	lr, [r3, #GRCG_TILE + 6]
				orreq	r1, r1, r2 lsl #8
				eoreq	r1, r1, lr
				orreq	r12, r1, r12
				mvn		r12, r12 lsl #16
				mov		r0, r12 lsr #16
				ldr		pc, [sp], #4


grcg_tdw0		mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grw_vramupd
				ldrb	r12, [r2, r0]
				orr		r12, r12, #1
				strb	r12, [r2, r0]
				ldr		r2, grw_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				b		grcg_tdw
grcg_tdw1		mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grw_vramupd
				ldrb	r12, [r2, r0]
				orr		r12, r12, #2
				strb	r12, [r2, r0]
				ldr		r2, grw_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				add		r0, r0, #VRAM_STEP
grcg_tdw		add		r0, r0, #VRAM_B
				ldr		r3, grw_grcg
				add		r0, r0, r9
				ldrb	r2, [r3, #GRCG_MODEREG]
				orr		r1, r1, r2 lsl #16
				tst		r1, #(1 << 16)
				ldreqb	r2, [r3, #(GRCG_TILE + 0)]
				streqb	r2, [r0]
				tst		r1, #(2 << 16)
				add		r0, r0, #(VRAM_R - VRAM_B)
				ldreqb	r2, [r3, #(GRCG_TILE + 2)]
				streqb	r2, [r0]
				tst		r1, #(4 << 16)
				add		r0, r0, #(VRAM_G - VRAM_R)
				ldreqb	r2, [r3, #(GRCG_TILE + 4)]
				streqb	r2, [r0]
				tst		r1, #(8 << 16)
				add		r0, r0, #(VRAM_E - VRAM_G)
				ldreqb	r2, [r3, #(GRCG_TILE + 6)]
				streqb	r2, [r0]
			;;	ldr		r3, grw_vramop
				ldrb	r3, [r9, #MEMWAIT_GRCG]
				CPUWORK	r3
				mov		pc, lr


grcg_rmw0		cmp		r1, #&ff
				beq		grcg_tdw0
				cmp		r1, #0
				beq		grcg_clock
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grw_vramupd
				ldrb	r12, [r2, r0]
				orr		r12, r12, #1
				strb	r12, [r2, r0]
				ldr		r2, grw_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				b		grcg_rmw
grcg_rmw1		cmp		r1, #&ff
				beq		grcg_tdw1
				cmp		r1, #0
				beq		grcg_clock
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grw_vramupd
				ldrb	r12, [r2, r0]
				orr		r12, r12, #2
				strb	r12, [r2, r0]
				ldr		r2, grw_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				add		r0, r0, #VRAM_STEP
grcg_rmw		add		r0, r0, #VRAM_B
				ldr		r3, grw_grcg
				add		r0, r0, r9
				ldrb	r2, [r3, #GRCG_MODEREG]
				orr		r1, r1, r2 lsl #16
				tst		r1, #(1 << 16)
				bne		grmw_bed
					ldrb	r12, [r0]
					ldrb	r2, [r3, #(GRCG_TILE + 0)]
					and		r2, r2, r1
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
grmw_bed		tst		r1, #(2 << 16)
				add		r0, r0, #(VRAM_R - VRAM_B)
				bne		grmw_red
					ldrb	r12, [r0]
					ldrb	r2, [r3, #(GRCG_TILE + 2)]
					and		r2, r2, r1
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
grmw_red		tst		r1, #(4 << 16)
				add		r0, r0, #(VRAM_G - VRAM_R)
				bne		grmw_ged
					ldrb	r12, [r0]
					ldrb	r2, [r3, #(GRCG_TILE + 4)]
					and		r2, r2, r1
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
grmw_ged		tst		r1, #(8 << 16)
				bne		grcg_clock
				add		r0, r0, #(VRAM_E - VRAM_G)
					ldrb	r12, [r0]
					ldrb	r2, [r3, #(GRCG_TILE + 6)]
					and		r2, r2, r1
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
grcg_clock	;;	ldr		r3, grw_vramop
				ldrb	r3, [r9, #MEMWAIT_GRCG]
				CPUWORK	r3
				mov		pc, lr
grw_vramupd		dcd		vramupdate
grw_gdcs		dcd		gdcs
grw_grcg		dcd		grcg
;; grw_vramop	dcd		vramop


grcgw_tdw0		ldr		r2, grww_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grw_vramupd
				add		r2, r2, r0
				ldrb	r12, [r2]
				orr		r12, r12, #1
				strb	r12, [r2]
				ldrb	r12, [r2, #1]
				orr		r12, r12, #1
				strb	r12, [r2, #1]
				b		grcgw_tdw
grcgw_tdw1		ldr		r2, grww_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grww_vramupd
				add		r2, r2, r0
				ldrb	r12, [r2]
				orr		r12, r12, #2
				strb	r12, [r2]
				ldrb	r12, [r2, #1]
				orr		r12, r12, #2
				strb	r12, [r2, #1]
				add		r0, r0, #VRAM_STEP
grcgw_tdw		add		r2, r9, #VRAM_B
				ldr		r3, grww_grcg
				add		r0, r0, r2
				ldrb	r2, [r3, #GRCG_MODEREG]
				orr		r1, r1, r2 lsl #16
				tst		r1, #(1 << 16)
				ldreqb	r2, [r3, #(GRCG_TILE + 0)]
				streqb	r2, [r0]
				streqb	r2, [r0, #1]
				tst		r1, #(2 << 16)
				add		r0, r0, #(VRAM_R - VRAM_B)
				ldreqb	r2, [r3, #(GRCG_TILE + 2)]
				streqb	r2, [r0]
				streqb	r2, [r0, #1]
				tst		r1, #(4 << 16)
				add		r0, r0, #(VRAM_G - VRAM_R)
				ldreqb	r2, [r3, #(GRCG_TILE + 4)]
				streqb	r2, [r0]
				streqb	r2, [r0, #1]
				tst		r1, #(8 << 16)
				add		r0, r0, #(VRAM_E - VRAM_G)
				ldreqb	r2, [r3, #(GRCG_TILE + 6)]
				streqb	r2, [r0]
				streqb	r2, [r0, #1]
			;;	ldr		r3, grww_vramop
				ldrb	r3, [r9, #MEMWAIT_GRCG]
				CPUWORK	r3
				mov		pc, lr


grcgw_rmw0		add		r2, r1, #1
				cmp		r2, #&10000
				beq		grcgw_tdw0
				cmp		r1, #0
				beq		grcgw_clock
				ldr		r2, grww_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grww_vramupd
				tst		r0, #1
				bne		grcgo_rmw0
				ldrh	r12, [r2, r0]
				orr		r12, r12, #1
				orr		r12, r12, #(1 << 8)
				strh	r12, [r2, r0]
				b		grcge_rmw
grcgw_rmw1		add		r2, r1, #1
				cmp		r2, #&10000
				beq		grcgw_tdw1
				cmp		r1, #0
				beq		grcgw_clock
				ldr		r2, grww_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grww_vramupd
				tst		r0, #1
				bne		grcgo_rmw1
				ldrh	r12, [r2, r0]
				orr		r12, r12, #2
				orr		r12, r12, #(2 << 8)
				strh	r12, [r2, r0]
				add		r0, r0, #VRAM_STEP
grcge_rmw		add		r2, r9, #VRAM_B
				ldr		r3, grww_grcg
				add		r0, r0, r2
				ldrb	r2, [r3, #GRCG_MODEREG]
				orr		r1, r1, r2 lsl #16
				tst		r1, #(1 << 16)
				bne		grmwo_bed
					ldrh	r2, [r3, #(GRCG_TILE + 0)]
					and		r2, r2, r1
					ldrh	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strh	r12, [r0]
grmwe_bed		tst		r1, #(2 << 16)
				add		r0, r0, #(VRAM_R - VRAM_B)
				bne		grmwo_red
					ldrh	r2, [r3, #(GRCG_TILE + 2)]
					and		r2, r2, r1
					ldrh	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strh	r12, [r0]
grmwe_red		tst		r1, #(4 << 16)
				add		r0, r0, #(VRAM_G - VRAM_R)
				bne		grmwo_ged
					ldrh	r2, [r3, #(GRCG_TILE + 4)]
					and		r2, r2, r1
					ldrh	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strh	r12, [r0]
grmwe_ged		tst		r1, #(8 << 16)
				bne		grmwe_eed
				add		r0, r0, #(VRAM_E - VRAM_G)
					ldrh	r2, [r3, #(GRCG_TILE + 6)]
					and		r2, r2, r1
					ldrh	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strh	r12, [r0]
grmwe_eed	;;	ldr		r3, grww_vramop
				ldrb	r3, [r9, #MEMWAIT_GRCG]
				CPUWORK	r3
				mov		pc, lr

grww_gdcs		dcd		gdcs
grww_vramupd	dcd		vramupdate
grww_grcg		dcd		grcg
;; grww_vramop	dcd		vramop

grcgo_rmw0		add		r2, r2, r0
				ldrb	r12, [r2]
				orr		r12, r12, #1
				strb	r12, [r2]
				ldrb	r12, [r2, #1]
				orr		r12, r12, #1
				strb	r12, [r2, #1]
				b		grcgo_rmw

grcgo_rmw1		add		r2, r2, r0
				ldrb	r12, [r2]
				orr		r12, r12, #2
				strb	r12, [r2]
				ldrb	r12, [r2, #1]
				orr		r12, r12, #2
				strb	r12, [r2, #1]
				add		r0, r0, #VRAM_STEP

grcgo_rmw		add		r2, r9, #VRAM_B
				ldr		r3, grww_grcg
				add		r0, r0, r2
				ldrb	r2, [r3, #GRCG_MODEREG]
				orr		r1, r1, r2 lsl #16
				tst		r1, #(1 << 16)
				bne		grmwo_bed
					ldrh	r2, [r3, #(GRCG_TILE + 0)]
					and		r2, r2, r1

					ldrb	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
					ldrb	r12, [r0, #1]
					bic		r12, r12, r1 lsr #8
					orr		r12, r12, r2 lsr #8
					strb	r12, [r0, #1]

grmwo_bed		tst		r1, #(2 << 16)
				add		r0, r0, #(VRAM_R - VRAM_B)
				bne		grmwo_red
					ldrh	r2, [r3, #(GRCG_TILE + 2)]
					and		r2, r2, r1

					ldrb	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
					ldrb	r12, [r0, #1]
					bic		r12, r12, r1 lsr #8
					orr		r12, r12, r2 lsr #8
					strb	r12, [r0, #1]

grmwo_red		tst		r1, #(4 << 16)
				add		r0, r0, #(VRAM_G - VRAM_R)
				bne		grmwo_ged
					ldrh	r2, [r3, #(GRCG_TILE + 4)]
					and		r2, r2, r1

					ldrb	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
					ldrb	r12, [r0, #1]
					bic		r12, r12, r1 lsr #8
					orr		r12, r12, r2 lsr #8
					strb	r12, [r0, #1]

grmwo_ged		tst		r1, #(8 << 16)
				bne		grcgw_clock
				add		r0, r0, #(VRAM_E - VRAM_G)
					ldrh	r2, [r3, #(GRCG_TILE + 6)]
					and		r2, r2, r1

					ldrb	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
					ldrb	r12, [r0, #1]
					bic		r12, r12, r1 lsr #8
					orr		r12, r12, r2 lsr #8
					strb	r12, [r0, #1]

grcgw_clock	;;	ldr		r3, grww_vramop
				ldrb	r3, [r9, #MEMWAIT_GRCG]
				CPUWORK	r3
				mov		pc, lr


; ---- egc

egc_rd			ldrb	r3, [r9, #MEMWAIT_GRCG]
				CPUWORK	r3
				b		egc_readbyte


egcw_rd			ldrb	r3, [r9, #MEMWAIT_GRCG]
				ldrb	r2, egcwrd_egc
				tst		r0, #1
				CPUWORK	r3
				beq		egc_readword
				ldrh	r12, [r2, #EGC_SFT]
				tst		r12, #&1000
				bne		egcwrd_std
				add		r2, r0, #1
				mov		r3, r1 lsr #8
				stmdb	sp!, {r2, r3, lr}
				bl		egc_writebyte
				ldmia	sp!, {r0, r1, lr}
				b		egc_writebyte
egcwrd_std		stmdb	sp!, {r0, r1, lr}
				add		r0, r0, #1
				mov		r1, r1 lsr #8
				bl		egc_writebyte
				ldmia	sp!, {r0, r1, lr}
				b		egc_writebyte
egcwrd_egc		dcd		egc


egc_wt			ldrb	r3, [r9, #MEMWAIT_GRCG]
				CPUWORK	r3
				b		egc_writebyte

egcw_wt			ldrb	r3, [r9, #MEMWAIT_GRCG]
				ldrb	r2, egcwwt_egc
				tst		r0, #1
				CPUWORK	r3
				beq		egc_writeword
				ldrh	r12, [r2, #EGC_SFT]
				stmdb	sp!, {r4, r5, lr}
				tst		r12, #&1000
				bne		egcwwt_std
				add		r4, r0, #1
				mov		r5, r1 lsr #8
				bl		egc_readbyte
				mov		r1, r5
				mov		r5, r0
				mov		r0, r4
				bl		egc_readbyte
				orr		r0, r5, r0 lsl #8
				ldmia	sp!, {r4, r5, lr}
egcwwt_std		mov		r4, r0
				mov		r5, r1
				add		r0, r0, #1
				mov		r1, r1 lsr #8
				bl		egc_readbyte
				mov		r1, r5
				mov		r5, r0
				mov		r0, r4
				bl		egc_readbyte
				orr		r0, r5, r0 lsl #8
				ldmia	sp!, {r4, r5, lr}
egcwwt_egc		dcd		egc


; ---- emmc

emmc_rd			add		r2, r9, #CPU_EMS
				and		r3, r0, #(3 << 14)
				ldr		r2, [r2, r3 lsr #(14 - 2)]
				mov		r0, r0 lsl #(32 - 14)
				ldrb	r0, [r2, r0 lsr #(32 - 14)]
				mov		pc, lr

emmc_wt			add		r2, r9, #CPU_EMS
				and		r3, r0, #(3 << 14)
				ldr		r2, [r2, r3 lsr #(14 - 2)]
				mov		r0, r0 lsl #(32 - 14)
				strb	r1, [r2, r0 lsr #(32 - 14)]
				mov		pc, lr


emmcw_rd		add		r2, r9, #CPU_EMS
				and		r12, r0, #(3 << 14)
				mov		r0, r0 lsl #(32 - 14)
				ldr		r3, [r2, r12 lsr #(14 - 2)]
				tst		r0, #(1 << (32 - 14))
				bne		emmcw_rd_odd
				add		r3, r3, r0 lsr #(32 - 14)
				ldrh	r0, [r3]
				mov		pc, lr
emmcw_rd_odd	ldrb	r1, [r3, r0 lsr #(32 - 14)]
				adds	r0, r0, #(1 << (32 - 14))
				beq		emmcw_rd_3fff
				ldrb	r0, [r3, r0 lsr #(32 - 14)]
				add		r0, r1, r0 lsl #8
				mov		pc, lr
emmcw_rd_3fff	eor		r12, r12, #(1 << 14)				; !
				ldr		r0, [r2, r12 lsr #(14 - 2)]
				ldrb	r0, [r0]
				add		r0, r1, r0 lsl #8
				mov		pc, lr


emmcw_wt		add		r2, r9, #CPU_EMS
				and		r12, r0, #(3 << 14)
				mov		r0, r0 lsl #(32 - 14)
				ldr		r3, [r2, r12 lsr #(14 - 2)]
				tst		r0, #(1 << (32 - 14))
				bne		emmcw_wt_odd
				add		r3, r3, r0 lsr #(32 - 14)
				strh	r1, [r3]
				mov		pc, lr
emmcw_wt_odd	strb	r1, [r3, r0 lsr #(32 - 14)]
				mov		r1, r1 lsr #8
				adds	r0, r0, #(1 << (32 - 14))
				beq		emmcw_wt_3fff
				strb	r1, [r3, r0 lsr #(32 - 14)]
				mov		pc, lr
emmcw_wt_3fff	eor		r12, r12, #(1 << 14)				; !
				ldr		r0, [r2, r12 lsr #(14 - 2)]
				strb	r1, [r0]
				mov		pc, lr


; ---- itf

i286_rb			ldrb	r2, [r9, #CPU_ITFBANK]
				orr		r12, r0, #VRAM_STEP
				cmp		r2, #0
				ldreqb	r0, [r0, r9]
				ldrneb	r0, [r12, r9]
				mov		pc, lr


i286_wb			ldrb	r2, [r9, #CPU_ITFBANK]
				orr		r12, r0, #(&1c8000 - &0e8000)
				cmp		r2, #0
				streqb	r1, [r0, r9]
				strneb	r1, [r12, r9]
				mov		pc, lr


i286w_rb		ldrb	r2, [r9, #CPU_ITFBANK]
				tst		r0, #1
				bne		i286w_rb_odd
				cmp		r2, #0
				orrne	r0, r0, #VRAM_STEP
				ldrh	r0, [r0, r9]
				mov		pc, lr
i286w_rb_odd	cmp		r2, #0
				orrne	r0, r0, #VRAM_STEP
				add		r2, r0, #1
				ldrb	r0, [r0, r9]
				ldrb	r1, [r2, r9]
				orr		r0, r0, r1 lsl #8
				mov		pc, lr


i286w_wb		ldrb	r2, [r9, #CPU_ITFBANK]
				tst		r0, #1
				bne		i286w_wb_odd
				cmp		r2, #0
				addne	r0, r0, #(&1c8000 - &0e8000)
				strh	r1, [r0, r9]
				mov		pc, lr
i286w_wb_odd	cmp		r2, #0
				addne	r0, r0, #(&1c8000 - &0e8000)
				mov		r3, r1 lsr #8
				add		r2, r0, #1
				strb	r1, [r0, r9]
				strb	r3, [r2, r9]
				mov		pc, lr



; ---- other

i286_nonram_rw	orr		r0, r0, #&ff00
i286_nonram_r	mov		r0, #&ff
i286w_wn
i286_wn			mov		pc, lr


; ---- dispatch

i286_memorymap
				ldr		r3, i2mm_memfn
				and		r1, r0, #1
				adr		r2, mmaptbl
				add		r12, r2, r1 lsl #5
				ldr		r1, [r2, r1 lsl #5]
				ldr		r2, [r12, #4]
				str		r1, [r3, #((0 * 32) + (0xe8000 >> (15 - 2)))]
				str		r1, [r3, #((0 * 32) + (0xf0000 >> (15 - 2)))]
				str		r2, [r3, #((0 * 32) + (0xf8000 >> (15 - 2)))]
				ldr		r1, [r12, #8]
				ldr		r2, [r12, #12]
				str		r1, [r3, #((4 * 32) + (0xd0000 >> (15 - 2)))]
				str		r1, [r3, #((4 * 32) + (0xd8000 >> (15 - 2)))]
				str		r2, [r3, #((4 * 32) + (0xe8000 >> (15 - 2)))]
				str		r2, [r3, #((4 * 32) + (0xf0000 >> (15 - 2)))]
				str		r2, [r3, #((4 * 32) + (0xf8000 >> (15 - 2)))]
				ldr		r1, [r12, #16]
				ldr		r2, [r12, #20]
				str		r1, [r3, #((8 * 32) + (0xe8000 >> (15 - 2)))]
				str		r1, [r3, #((8 * 32) + (0xf0000 >> (15 - 2)))]
				str		r2, [r3, #((8 * 32) + (0xf8000 >> (15 - 2)))]
				ldr		r1, [r12, #24]
				ldr		r2, [r12, #28]
				str		r1, [r3, #((12 * 32) + (0xd0000 >> (15 - 2)))]
				str		r1, [r3, #((12 * 32) + (0xd8000 >> (15 - 2)))]
				str		r2, [r3, #((12 * 32) + (0xe8000 >> (15 - 2)))]
				str		r2, [r3, #((12 * 32) + (0xf0000 >> (15 - 2)))]
				str		r2, [r3, #((12 * 32) + (0xf8000 >> (15 - 2)))]
				mov		pc, lr
i2mm_memfn		dcd		memfn
mmaptbl			dcd		i286_rd		; NEC
				dcd		i286_rb
				dcd		i286_wn
				dcd		i286_wn
				dcd		i286w_rd
				dcd		i286w_rb
				dcd		i286_wn
				dcd		i286_wn
				dcd		i286_rb		; EPSON
				dcd		i286_rb
				dcd		i286_wt
				dcd		i286_wb
				dcd		i286w_rb
				dcd		i286w_rb
				dcd		i286w_wt
				dcd		i286w_wb


i286_vram_dispatch
				ldr		r3, i2vd_memfn
				and		r1, r0, #15
				adr		r2, vacctbl
				add		r2, r2, r1 lsl #4
				ldr		r1, [r2]
				ldr		r12, [r2, #4]
				tst		r0, #&10
				str		r1, [r3, #((0 * 32) + (0xa8000 >> (15 - 2)))]
				str		r1, [r3, #((0 * 32) + (0xb0000 >> (15 - 2)))]
				str		r1, [r3, #((0 * 32) + (0xb8000 >> (15 - 2)))]
				strne	r1, [r3, #((0 * 32) + (0xe0000 >> (15 - 2)))]
				str		r12, [r3, #((4 * 32) + (0xa8000 >> (15 - 2)))]
				ldr		r1, [r2, #8]
				str		r12, [r3, #((4 * 32) + (0xb0000 >> (15 - 2)))]
				str		r12, [r3, #((4 * 32) + (0xb8000 >> (15 - 2)))]
				strne	r12, [r3, #((4 * 32) + (0xe0000 >> (15 - 2)))]
				str		r1, [r3, #((8 * 32) + (0xa8000 >> (15 - 2)))]
				ldr		r12, [r2, #12]
				str		r1, [r3, #((8 * 32) + (0xb0000 >> (15 - 2)))]
				str		r1, [r3, #((8 * 32) + (0xb8000 >> (15 - 2)))]
				strne	r1, [r3, #((8 * 32) + (0xe0000 >> (15 - 2)))]
				str		r12, [r3, #((12 * 32) + (0xa8000 >> (15 - 2)))]
				str		r12, [r3, #((12 * 32) + (0xb0000 >> (15 - 2)))]
				str		r12, [r3, #((12 * 32) + (0xb8000 >> (15 - 2)))]
				strne	r12, [r3, #((12 * 32) + (0xe0000 >> (15 - 2)))]
				movne	pc, lr

				adr		r1, i286_nonram_r
				str		r1, [r3, #((0 * 32) + (0xe0000 >> (15 - 2)))]
				adr		r1, i286_wn
				str		r1, [r3, #((4 * 32) + (0xe0000 >> (15 - 2)))]
				adr		r1, i286_nonram_rw
				str		r1, [r3, #((8 * 32) + (0xe0000 >> (15 - 2)))]
				adr		r1, i286_wn
				str		r1, [r3, #((12 * 32) + (0xe0000 >> (15 - 2)))]
				mov		pc, lr

i2vd_memfn		dcd		memfn
vacctbl			dcd		vram_r0			; 00
				dcd		vram_w0
				dcd		vramw_r0
				dcd		vramw_w0
				dcd		vram_r1			; 10
				dcd		vram_w1
				dcd		vramw_r1
				dcd		vramw_w1
				dcd		vram_r0			; 20
				dcd		vram_w0
				dcd		vramw_r0
				dcd		vramw_w0
				dcd		vram_r1			; 30
				dcd		vram_w1
				dcd		vramw_r1
				dcd		vramw_w1
				dcd		vram_r0			; 40
				dcd		vram_w0
				dcd		vramw_r0
				dcd		vramw_w0
				dcd		vram_r1			; 50
				dcd		vram_w1
				dcd		vramw_r1
				dcd		vramw_w1
				dcd		vram_r0			; 60
				dcd		vram_w0
				dcd		vramw_r0
				dcd		vramw_w0
				dcd		vram_r1			; 70
				dcd		vram_w1
				dcd		vramw_r1
				dcd		vramw_w1
				dcd		grcg_tcr0		; 80
				dcd		grcg_tdw0
				dcd		grcgw_tcr0
				dcd		grcgw_tdw0
				dcd		grcg_tcr1		; 90
				dcd		grcg_tdw1
				dcd		grcgw_tcr1
				dcd		grcgw_tdw1
				dcd		egc_rd			; a0
				dcd		egc_wt
				dcd		egcw_rd
				dcd		egcw_wt
				dcd		egc_rd			; b0
				dcd		egc_wt
				dcd		egcw_rd
				dcd		egcw_wt
				dcd		vram_r0			; c0
				dcd		grcg_rmw0
				dcd		vramw_r0
				dcd		grcgw_rmw0
				dcd		vram_r1			; d0
				dcd		grcg_rmw1
				dcd		vramw_r1
				dcd		grcgw_rmw1
				dcd		egc_rd			; e0
				dcd		egc_wt
				dcd		egcw_rd
				dcd		egcw_wt
				dcd		egc_rd			; f0
				dcd		egc_wt
				dcd		egcw_rd
				dcd		egcw_wt

	END

