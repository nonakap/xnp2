
	MACRO
$label	GETPC8
$label		ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r0, r8 lsr #16
			bl		i286a_memoryread
			add		r8, r8, #(1 << 16)
	MEND

	MACRO
$label	GETPC16
$label		ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r0, r8 lsr #16
			bl		i286a_memoryread_w
			add		r8, r8, #(2 << 16)
	MEND


	MACRO
$label	GETR0
$label		cmp		r0, #I286_MEMWRITEMAX
			ldrccb	r0, [r0, r9]
			blcs	i286a_memoryread
	MEND

	MACRO
$label	GETPCF8
$label		add		r0, r5, r8 lsr #16
			cmp		r0, #I286_MEMWRITEMAX
			ldrccb	r0, [r0, r9]
			blcs	i286a_memoryread
			add		r8, r8, #(1 << 16)
	MEND

	MACRO
$label	GETPCF16
$label		add		r0, r5, r8 lsr #16
			bl		i286a_memoryread_w
			add		r8, r8, #(2 << 16)
	MEND


	MACRO
$label	R8SRC	$op, $out
$label		and		$out, $op, #3
			tst		$op, #(1 << 2)
			add		$out, r9, $out lsl #1
			addne	$out, $out, #1
	MEND

	MACRO
$label	R8DST	$op, $out
$label		and		$out, $op, #(6 << 2)
			tst		$op, #(1 << 5)
			add		$out, r9, $out lsr #2
			addne	$out, $out, #1
	MEND


	MACRO
$label	EAREG8	$src
$label		add		r0, r5, r8 lsr #16
			GETR0
			and		$src, r0, #(6 << 2)
			add		r8, r8, #(1 << 16)
			add		$src, r9, $src lsr #2
			tst		r0, #(1 << 5)
			addne	$src, $src, #1
	MEND

	MACRO
$label	REG8EA	$dst, $regclk, $memclk
$label		add		r0, r5, r8 lsr #16
			GETR0
			add		r8, r8, #(1 << 16)
			R8DST	r0, $dst
			cmp		r0, #&c0
			bcc		$label.1
			CPUWORK	$regclk
			R8SRC	r0, r12
			ldrb	r0, [r12, #CPU_REG]
			b		$label.2
$label.1	CPUWORK	$memclk
			bl		i286a_ea
			bl		i286a_memoryread
$label.2
	MEND


	MACRO
$label	R16SRC	$op, $out
$label		and		$out, $op, #7
			add		$out, r9, $out lsl #1
	MEND

	MACRO
$label	R16DST	$op, $out
$label		and		$out, $op, #(7 << 3)
			add		$out, r9, $out lsr #2
	MEND

	MACRO
$label	EAREG16	$src
$label		add		r0, r5, r8 lsr #16
			GETR0
			and		$src, r0, #(7 << 3)
			add		r8, r8, #(1 << 16)
			add		$src, r9, $src lsr #2
	MEND

	MACRO
$label	REG16EA	$dst, $regclk, $memclk
$label		add		r0, r5, r8 lsr #16
			GETR0
			and		$dst, r0, #(7 << 3)
			add		r8, r8, #(1 << 16)
			add		$dst, r9, $dst lsr #2
			cmp		r0, #&c0
			bcc		$label.1
			CPUWORK	$regclk
			R16SRC	r0, r12
			ldrh	r0, [r12, #CPU_REG]
			b		$label.2
$label.1	CPUWORK	$memclk
			bl		i286a_ea
			bl		i286a_memoryread_w
$label.2
	MEND

	END

