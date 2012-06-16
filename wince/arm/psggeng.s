
PSGFREQPADBIT	equ		12
PSGADDEDBIT		equ		3
PSGADDEDCNT		equ		(1 << PSGADDEDBIT)

PSGENV_INC		equ		15
PSGENV_ONESHOT	equ		16
PSGENV_LASTON	equ		32
PSGENV_ONECYCLE	equ		64

T_FREQ			equ		0
T_COUNT			equ		4
T_PVOL			equ		8
T_PUCHI			equ		12
T_PAN			equ		14
T_SIZE			equ		16

P_TONE			equ		0
P_NOISE			equ		48
PN_FREQ			equ		48
PN_COUNT		equ		52
PN_BASE			equ		56
P_REG			equ		60
P_ENVCNT		equ		76
P_ENVMAX		equ		78
P_MIXER			equ		80
P_ENVMODE		equ		81
P_ENVVOL		equ		82
P_ENVVOLCNT		equ		83
P_EVOL			equ		84
P_PUCHICOUNT	equ		88

C_VOLUME		equ		0
C_VOLTBL		equ		64
C_RATE			equ		128
C_BASE			equ		132
C_PUCHIDEC		equ		136

CD_BIT31		equ		&80000000

; r0	psggen
; r1	Offset
; r2	Counter
; r3	Temporary Register
; r4	Temporary Register
; r5	Temporary Register
; r6	Temporary Register
; r7	L
; r8	R
; r9	noise
; r10	mixer
; r11	psgcfg Fix
; r12	Temporary Register
; lr	envcnt?

	IMPORT	__randseed
	IMPORT	psggencfg

	EXPORT	psggen_getpcm

	AREA	.text, CODE, READONLY


	MACRO
$label	PSGCALC	$o, $t, $n
$label		ldr		r12, [r0, #($o + T_PVOL)]
			tst		r10, $t
			mov		r3, #0
			ldr		r12, [r12]
			beq		$label.n
			cmp		r12, #0
			beq		$label.ed
			ldr		r4, [r0, #($o + T_COUNT)]
			ldr		r5, [r0, #($o + T_FREQ)]
			tst		r10, $n
			bne		$label.tn
			mov		r6, #PSGADDEDCNT
$label.tlp	adds	r4, r4, r5
			addpl	r3, r3, r12
			submi	r3, r3, r12
			subs	r6, r6, #1
			bne		$label.tlp
			str		r4, [r0, #($o + T_COUNT)]
			ldrb	r6, [r0, #($o + T_PAN)]
			b		$label.pan
$label.tn	add		r6, r9, #1
$label.tnlp	add		r4, r4, r5
			tst		r4, r6
			addpl	r3, r3, r12
			submi	r3, r3, r12
			mov		r6, r6 lsl #1
			tst		r6, #(1 << PSGADDEDCNT)
			beq		$label.tnlp
			str		r4, [r0, #($o + T_COUNT)]
			ldrb	r6, [r0, #($o + T_PAN)]
			b		$label.pan
$label.n	cmp		r12, #0
			beq		$label.ed
			tst		r10, $n
			bne		$label.nmn
			ldrb	r4, [r0, #($o + T_PUCHI)]
			ldrb	r6, [r0, #($o + T_PAN)]
			subs	r4, r4, #1
			strcsb	r4, [r0, #($o + T_PUCHI)]
			addcs	r3, r3, r12 lsl #PSGADDEDBIT
			b		$label.pan
$label.nmn	mov		r4, #(1 << (32 - PSGADDEDCNT))
			ldrb	r6, [r0, #($o + T_PAN)]
$label.nlp	tst		r4, r9
			addeq	r3, r3, r12
			subne	r3, r3, r12
			movs	r4, r4 lsl #1
			bne		$label.nlp
$label.pan	tst		r6, #1
			addeq	r7, r7, r3
			tst		r6, #2
			addeq	r8, r8, r3
$label.ed
	MEND


psggen_getpcm
				ldrb	r12, [r0, #P_MIXER]
				tst		r12, #&3f
				bne		countcheck
				ldr		r3, [r0, #P_PUCHICOUNT]
				cmp		r2, r3
				movcs	r2, r3
				sub		r3, r3, r2
				str		r3, [r0, #P_PUCHICOUNT]
countcheck		cmp		r2, #0
				moveq	pc, lr

				stmdb	sp!, {r4 - r11, lr}
				ldr		r11, psgvoltbl
				ldrh	lr, [r0, #P_ENVCNT]

psgmake_lp		ldr		r10, [r0, #P_MIXER]
				cmp		lr, #0
				beq		makenoise
				subs	lr, lr, #1
				bne		makenoise
				bic		r10, r10, #(255 << 16)
				subs	r10, r10, #(1 << 24)
				bcs		calcenvnext
				tst		r10, #(PSGENV_ONESHOT << 8)
				beq		calcenvcyc
				tst		r10, #(PSGENV_LASTON << 8)
				ldreq	r6, [r11]
				ldrne	r6, [r11, #(15 * 4)]
				orrne	r10, r10, #(15 << 16)
				b		calcenvvstr
calcenvcyc		bic		r10, r10, #(240 << 24)
				tst		r10, #(PSGENV_ONECYCLE << 8)
				eoreq	r10, r10, #(PSGENV_INC << 8)
calcenvnext		ldrh	lr, [r0, #P_ENVMAX]
				eor		r3, r10, r10 lsr #16
				and		r3, r3, #(15 << 8)
				ldr		r6, [r11, r3 lsr #(8 - 2)]
				orr		r10, r10, r3 lsl #8
calcenvvstr		str		r6, [r0, #P_EVOL]
calcenvstr		str		r10, [r0, #P_MIXER]

makenoise		tst		r10, #&38
				beq		makesamp
				ldr		r6, [r0, #PN_FREQ]
				ldr		r7, [r0, #PN_COUNT]
				ldr		r8, [r0, #PN_BASE]
				mov		r9, #0
				mov		r3, #PSGADDEDCNT
mknoise_lp		subs	r7, r7, r6
				bcc		updatenoise
updatenoiseret	add		r9, r8, r9 lsl #1
				subs	r3, r3, #1
				bne		mknoise_lp
				str		r7, [r0, #PN_COUNT]

makesamp		mov		r7, #0
				mov		r8, #0

psgcalc0		PSGCALC	(T_SIZE * 0), #&01, #&08
psgcalc1		PSGCALC	(T_SIZE * 1), #&02, #&10
psgcalc2		PSGCALC	(T_SIZE * 2), #&04, #&20

				ldr		r4, [r1]
				ldr		r3, [r1, #4]
				subs	r2, r2, #1
				add		r4, r4, r7
				str		r4, [r1], #4
				add		r3, r3, r8
				str		r3, [r1], #4
				bne		psgmake_lp
				strh	lr, [r0, #P_ENVCNT]
				ldmia	sp!, {r4 - r11, pc}
psgvoltbl		dcd		psggencfg + C_VOLUME

updatenoise		ldr		r4, randdcd
				ldr		r8, [r4]
				ldr		r12, randdcd1
				mul		r12, r8, r12
				ldr		r8, randdcd2
				add		r8, r8, r12
				str		r8, [r4]
				and		r8, r8, #(1 << (32 - PSGADDEDCNT))
				str		r8, [r0, #PN_BASE]
				b		updatenoiseret
randdcd			dcd		__randseed
randdcd1		dcd		&343fd
randdcd2		dcd		&269ec3

	END

