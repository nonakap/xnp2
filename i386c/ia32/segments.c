/*	$Id: segments.c,v 1.18 2008/01/25 17:53:27 monaka Exp $	*/

/*
 * Copyright (c) 2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"
#include "cpu.h"
#include "ia32.mcr"


void
load_segreg(int idx, UINT16 selector, int exc)
{
	selector_t sel;
	int rv;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		descriptor_t sd;

		/* real-mode or vm86 mode */
		CPU_REGS_SREG(idx) = selector;

		memset(&sd, 0, sizeof(sd));
		if (idx == CPU_CS_INDEX) {
			sd.rpl = CPU_STAT_CPL;
		}
		sd.u.seg.limit = CPU_STAT_SREGLIMIT(idx);
		CPU_SET_SEGDESC_DEFAULT(&sd, idx, selector);
		CPU_STAT_SREG(idx) = sd;
		return;
	}

	/*
	 * protected mode
	 */
	VERBOSE(("load_segreg: EIP = %04x:%08x, idx = %d, selector = %04x, exc = %d", CPU_CS, CPU_PREV_EIP, idx, selector, exc));

	if (idx == CPU_CS_INDEX) {
		ia32_panic("load_segreg: CS");
	}

	rv = parse_selector(&sel, selector);
	if (rv < 0) {
		if ((rv != -2) || (idx == CPU_SS_INDEX)) {
			EXCEPTION(exc, sel.idx);
		}
		CPU_REGS_SREG(idx) = sel.selector;
		CPU_STAT_SREG_CLEAR(idx);
		return;
	}

	switch (idx) {
	case CPU_SS_INDEX:
		if ((CPU_STAT_CPL != sel.rpl)
		 || (CPU_STAT_CPL != sel.desc.dpl)
		 || !sel.desc.s
		 || sel.desc.u.seg.c
		 || !sel.desc.u.seg.wr) {
			EXCEPTION(exc, sel.idx);
		}

		/* not present */
		rv = selector_is_not_present(&sel);
		if (rv < 0) {
			EXCEPTION(SS_EXCEPTION, sel.idx);
		}

		load_ss(sel.selector, &sel.desc, sel.selector & 3);
		break;

	case CPU_ES_INDEX:
	case CPU_DS_INDEX:
	case CPU_FS_INDEX:
	case CPU_GS_INDEX:
		/* !(system segment || non-readable code segment) */
		if (!sel.desc.s
		 || (sel.desc.u.seg.c && !sel.desc.u.seg.wr)) {
			EXCEPTION(exc, sel.idx);
		}
		/* data segment || non-conforming code segment */
		if (!sel.desc.u.seg.c || !sel.desc.u.seg.ec) {
			/* check privilege level */
			if ((sel.rpl > sel.desc.dpl) || (CPU_STAT_CPL > sel.desc.dpl)) {
				EXCEPTION(exc, sel.idx);
			}
		}

		/* not present */
		rv = selector_is_not_present(&sel);
		if (rv < 0) {
			EXCEPTION(NP_EXCEPTION, sel.idx);
		}

		CPU_REGS_SREG(idx) = sel.selector;
		CPU_STAT_SREG(idx) = sel.desc;
		break;
	
	default:
		ia32_panic("load_segreg(): segment register index is invalid");
		break;
	}
}

/*
 * load SS register
 */
void
load_ss(UINT16 selector, const descriptor_t *sd, UINT cpl)
{

	CPU_STAT_SS32 = sd->d;
	CPU_REGS_SREG(CPU_SS_INDEX) = (UINT16)((selector & ~3) | (cpl & 3));
	CPU_STAT_SREG(CPU_SS_INDEX) = *sd;
}

/*
 * load CS register
 */
void
load_cs(UINT16 selector, const descriptor_t *sd, UINT cpl)
{

	CPU_INST_OP32 = CPU_INST_AS32 =
	    CPU_STATSAVE.cpu_inst_default.op_32 =
	    CPU_STATSAVE.cpu_inst_default.as_32 = sd->d;
	CPU_REGS_SREG(CPU_CS_INDEX) = (UINT16)((selector & ~3) | (cpl & 3));
	CPU_STAT_SREG(CPU_CS_INDEX) = *sd;
	CPU_SET_CPL(cpl & 3);
}

/*
 * load LDT register
 */
void
load_ldtr(UINT16 selector, int exc)
{
	selector_t sel;
	int rv;

	rv = parse_selector(&sel, selector);
	if (rv < 0 || sel.ldt) {
		if (rv == -2) {
			/* null segment */
			CPU_LDTR = 0;
			memset(&CPU_LDTR_DESC, 0, sizeof(CPU_LDTR_DESC));
			return;
		}
		EXCEPTION(exc, sel.selector);
	}

	/* check descriptor type */
	if (sel.desc.s || (sel.desc.type != CPU_SYSDESC_TYPE_LDT)) {
		EXCEPTION(exc, sel.selector);
	}

#if 0
	/*
	 * LEMM の挙動より LDT セグメントのリミットチェック処理を無効化
	 *
	 * 症状１:リミット 0 の LDT セレクタを LLDT は駄目っぽい。
	 * 対策１:リミット 0 の LDT セレクタの代わりにヌルセレクタを LLDT。
	 */
	/* check limit */
	if (sel.desc.u.seg.limit < 7) {
		ia32_panic("load_ldtr: LDTR descriptor limit < 7 (limit = %d)", sel.desc.u.seg.limit);
	}
#endif

	/* not present */
	rv = selector_is_not_present(&sel);
	if (rv < 0) {
		EXCEPTION((exc == TS_EXCEPTION) ? TS_EXCEPTION : NP_EXCEPTION, sel.selector);
	}

#if defined(MORE_DEBUG)
	ldtr_dump(sel.desc.u.seg.segbase, sel.desc.u.seg.limit);
#endif

	CPU_LDTR = sel.selector;
	CPU_LDTR_DESC = sel.desc;
}

void
load_descriptor(descriptor_t *descp, UINT32 addr)
{
	UINT32 l, h;

	memset(descp, 0, sizeof(*descp));

	l = cpu_kmemoryread_d(addr);
	h = cpu_kmemoryread_d(addr + 4);
	VERBOSE(("load_descriptor: descriptor address = 0x%08x, h = 0x%08x, l = %08x", addr, h, l));

	descp->flag = 0;

	descp->p = (h & CPU_DESC_H_P) == CPU_DESC_H_P;
	descp->type = (UINT8)((h & CPU_DESC_H_TYPE) >> 8);
	descp->dpl = (UINT8)((h & CPU_DESC_H_DPL) >> 13);
	descp->s = (h & CPU_DESC_H_S) == CPU_DESC_H_S;

	VERBOSE(("load_descriptor: present = %s, type = %d, DPL = %d", descp->p ? "true" : "false", descp->type, descp->dpl));

	if (descp->s) {
		/* code/data */
		descp->valid = 1;
		descp->d = (h & CPU_SEGDESC_H_D) ? 1 : 0;

		descp->u.seg.c = (h & CPU_SEGDESC_H_D_C) ? 1 : 0;
		descp->u.seg.g = (h & CPU_SEGDESC_H_G) ? 1 : 0;
		descp->u.seg.wr = (descp->type & CPU_SEGDESC_TYPE_WR) ? 1 : 0;
		descp->u.seg.ec = (descp->type & CPU_SEGDESC_TYPE_EC) ? 1 : 0;

		descp->u.seg.segbase  = (l >> 16) & 0xffff;
		descp->u.seg.segbase |= (h & 0xff) << 16;
		descp->u.seg.segbase |= h & 0xff000000;
		descp->u.seg.limit = (h & 0xf0000) | (l & 0xffff);
		if (descp->u.seg.g) {
			descp->u.seg.limit <<= 12;
			descp->u.seg.limit |= 0xfff;
		}
		descp->u.seg.segend = descp->u.seg.segbase + descp->u.seg.limit;

		VERBOSE(("load_descriptor: %s segment descriptor", descp->u.seg.c ? "code" : "data"));
		VERBOSE(("load_descriptor: segment base address = 0x%08x, segment limit = 0x%08x", descp->u.seg.segbase, descp->u.seg.limit));
		VERBOSE(("load_descriptor: d = %s, g = %s", descp->d ? "on" : "off", descp->u.seg.g ? "on" : "off"));
		VERBOSE(("load_descriptor: %s, %s", descp->u.seg.c ? (descp->u.seg.wr ? "executable/readable" : "execute-only") : (descp->u.seg.wr ? "writable" : "read-only"), (descp->u.seg.c ? (descp->u.seg.ec ? "conforming" : "non-conforming") : (descp->u.seg.ec ? "expand-down" : "expand-up"))));
	} else {
		/* system */
		switch (descp->type) {
		case CPU_SYSDESC_TYPE_LDT:		/* LDT */
			descp->valid = 1;
			descp->u.seg.g = (h & CPU_SEGDESC_H_G) ? 1 : 0;

			descp->u.seg.segbase  = h & 0xff000000;
			descp->u.seg.segbase |= (h & 0xff) << 16;
			descp->u.seg.segbase |= l >> 16;
			descp->u.seg.limit  = h & 0xf0000;
			descp->u.seg.limit |= l & 0xffff;
			if (descp->u.seg.g) {
				descp->u.seg.limit <<= 12;
				descp->u.seg.limit |= 0xfff;
			}
			descp->u.seg.segend = descp->u.seg.segbase + descp->u.seg.limit;

			VERBOSE(("load_descriptor: LDT descriptor"));
			VERBOSE(("load_descriptor: LDT base address = 0x%08x, limit size = 0x%08x", descp->u.seg.segbase, descp->u.seg.limit));
			break;

		case CPU_SYSDESC_TYPE_TASK:		/* task gate */
			descp->valid = 1;
			descp->u.gate.selector = (UINT16)(l >> 16);

			VERBOSE(("load_descriptor: task descriptor: selector = 0x%04x", descp->u.gate.selector));
			break;

		case CPU_SYSDESC_TYPE_TSS_16:		/* 286 TSS */
		case CPU_SYSDESC_TYPE_TSS_BUSY_16:	/* 286 TSS Busy */
		case CPU_SYSDESC_TYPE_TSS_32:		/* 386 TSS */
		case CPU_SYSDESC_TYPE_TSS_BUSY_32:	/* 386 TSS Busy */
			descp->valid = 1;
			descp->d = (h & CPU_GATEDESC_H_D) ? 1 : 0;
			descp->u.seg.g = (h & CPU_SEGDESC_H_G) ? 1 : 0;

			descp->u.seg.segbase  = h & 0xff000000;
			descp->u.seg.segbase |= (h & 0xff) << 16;
			descp->u.seg.segbase |= l >> 16;
			descp->u.seg.limit  = h & 0xf0000;
			descp->u.seg.limit |= l & 0xffff;
			if (descp->u.seg.g) {
				descp->u.seg.limit <<= 12;
				descp->u.seg.limit |= 0xfff;
			}
			descp->u.seg.segend = descp->u.seg.segbase + descp->u.seg.limit;

			VERBOSE(("load_descriptor: %dbit %sTSS descriptor", descp->d ? 32 : 16, (descp->type & CPU_SYSDESC_TYPE_TSS_BUSY_IND) ? "busy " : ""));
			VERBOSE(("load_descriptor: TSS base address = 0x%08x, limit = 0x%08x", descp->u.seg.segbase, descp->u.seg.limit));
			VERBOSE(("load_descriptor: d = %s, g = %s", descp->d ? "on" : "off", descp->u.seg.g ? "on" : "off"));
			break;

		case CPU_SYSDESC_TYPE_CALL_16:		/* 286 call gate */
		case CPU_SYSDESC_TYPE_INTR_16:		/* 286 interrupt gate */
		case CPU_SYSDESC_TYPE_TRAP_16:		/* 286 trap gate */
		case CPU_SYSDESC_TYPE_CALL_32:		/* 386 call gate */
		case CPU_SYSDESC_TYPE_INTR_32:		/* 386 interrupt gate */
		case CPU_SYSDESC_TYPE_TRAP_32:		/* 386 trap gate */
			if ((h & 0x0000000e0) == 0) {
				descp->valid = 1;
				descp->d = (h & CPU_GATEDESC_H_D) ? 1:0;
				descp->u.gate.selector = (UINT16)(l >> 16);
				descp->u.gate.offset  = h & 0xffff0000;
				descp->u.gate.offset |= l & 0xffff;
				descp->u.gate.count = (BYTE)(h & 0x1f);

				VERBOSE(("load_descriptor: %dbit %s gate descriptor", descp->d ? 32 : 16, ((descp->type & CPU_SYSDESC_TYPE_MASKBIT) == CPU_SYSDESC_TYPE_CALL) ? "call" : (((descp->type & CPU_SYSDESC_TYPE_MASKBIT) == CPU_SYSDESC_TYPE_INTR) ? "interrupt" : "trap")));
				VERBOSE(("load_descriptor: selector = 0x%04x, offset = 0x%08x, count = %d, d = %s", descp->u.gate.selector, descp->u.gate.offset, descp->u.gate.count, descp->d ? "on" : "off"));
			} else {
				ia32_panic("load_descriptor: 386 gate is invalid");
			}
			break;

		case 0: case 8: case 10: case 13: /* reserved */
		default:
			descp->valid = 0;
			break;
		}
	}
}

int
parse_selector(selector_t *ssp, UINT16 selector)
{
	UINT32 base;
	UINT limit;
	UINT idx;

	ssp->selector = selector;
	ssp->idx = selector & ~3;
	ssp->rpl = selector & 3;
	ssp->ldt = (UINT8)(selector & CPU_SEGMENT_TABLE_IND);

	VERBOSE(("parse_selector: selector = %04x, index = %d, RPL = %d, %cDT", ssp->selector, ssp->idx >> 3, ssp->rpl, ssp->ldt ? 'L' : 'G'));

	/* descriptor table */
	idx = selector & CPU_SEGMENT_SELECTOR_INDEX_MASK;
	if (ssp->ldt) {
		/* LDT */
		if (!CPU_LDTR_DESC.valid) {
			VERBOSE(("parse_selector: LDT is invalid"));
			return -1;
		}
		base = CPU_LDTR_BASE;
		limit = CPU_LDTR_LIMIT;
	} else {
		/* check null segment */
		if (idx == 0) {
			VERBOSE(("parse_selector: null segment"));
			return -2;
		}
		base = CPU_GDTR_BASE;
		limit = CPU_GDTR_LIMIT;
	}
	if (idx + 7 > limit) {
		VERBOSE(("parse_selector: segment limit check failed"));
		return -3;
	}

	/* load descriptor */
	ssp->addr = base + idx;
	load_descriptor(&ssp->desc, ssp->addr);
	if (!ssp->desc.valid) {
		VERBOSE(("parse_selector: segment descriptor is invalid"));
		return -4;
	}

	return 0;
}

int
selector_is_not_present(const selector_t *ssp)
{
	UINT32 h;

	/* not present */
	if (!ssp->desc.p) {
		VERBOSE(("selector_is_not_present: not present"));
		return -1;
	}

	/* set access bit if code/data segment descriptor */
	if (ssp->desc.s) {
		h = cpu_kmemoryread_d(ssp->addr + 4);
		if (!(h & CPU_SEGDESC_H_A)) {
			h |= CPU_SEGDESC_H_A;
			cpu_kmemorywrite_d(ssp->addr + 4, h);
		}
	}

	return 0;
}
