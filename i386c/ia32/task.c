/*	$Id: task.c,v 1.22 2008/01/25 18:12:13 monaka Exp $	*/

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
load_tr(UINT16 selector)
{
	selector_t task_sel;
	int rv;
#if defined(IA32_SUPPORT_DEBUG_REGISTER)
	int i;
#endif
	UINT16 iobase;

	rv = parse_selector(&task_sel, selector);
	if (rv < 0 || task_sel.ldt || task_sel.desc.s) {
		EXCEPTION(GP_EXCEPTION, task_sel.idx);
	}

	/* check descriptor type & stack room size */
	switch (task_sel.desc.type) {
	case CPU_SYSDESC_TYPE_TSS_16:
		if (task_sel.desc.u.seg.limit < 0x2b) {
			EXCEPTION(TS_EXCEPTION, task_sel.idx);
		}
		iobase = 0;
		break;

	case CPU_SYSDESC_TYPE_TSS_32:
		if (task_sel.desc.u.seg.limit < 0x67) {
			EXCEPTION(TS_EXCEPTION, task_sel.idx);
		}
		iobase = cpu_kmemoryread_w(task_sel.desc.u.seg.segbase + 102);
		break;

	default:
		EXCEPTION(GP_EXCEPTION, task_sel.idx);
		return;
	}

	/* not present */
	rv = selector_is_not_present(&task_sel);
	if (rv < 0) {
		EXCEPTION(NP_EXCEPTION, task_sel.idx);
	}

#if defined(MORE_DEBUG)
	tr_dump(task_sel.selector, task_sel.desc.u.seg.segbase, task_sel.desc.u.seg.limit);
#endif

	CPU_SET_TASK_BUSY(task_sel.selector, &task_sel.desc);
	CPU_TR = task_sel.selector;
	CPU_TR_DESC = task_sel.desc;

	/* I/O deny bitmap */
	if (task_sel.desc.type == CPU_SYSDESC_TYPE_TSS_BUSY_32) {
		if (iobase != 0 && iobase < task_sel.desc.u.seg.limit) {
			CPU_STAT_IOLIMIT = (UINT16)(task_sel.desc.u.seg.limit - iobase);
			CPU_STAT_IOADDR = task_sel.desc.u.seg.segbase + iobase;
		} else {
			CPU_STAT_IOLIMIT = 0;
		}
	} else {
		CPU_STAT_IOLIMIT = 0;
	}

#if defined(IA32_SUPPORT_DEBUG_REGISTER)
	/* clear local break point flags */
	CPU_DR7 &= ~(CPU_DR7_L(0)|CPU_DR7_L(1)|CPU_DR7_L(2)|CPU_DR7_L(3)|CPU_DR7_LE);
	CPU_STAT_BP = 0;
	for (i = 0; i < CPU_DEBUG_REG_INDEX_NUM; i++) {
		if (CPU_DR7 & CPU_DR7_G(i)) {
			CPU_STAT_BP |= (1 << i);
		}
	}
#endif
}

void
get_stack_pointer_from_tss(UINT pl, UINT16 *new_ss, UINT32 *new_esp)
{
	UINT32 tss_stack_addr;

	__ASSERT(pl < 3);

	if (CPU_TR_DESC.type == CPU_SYSDESC_TYPE_TSS_BUSY_32) {
		tss_stack_addr = pl * 8 + 4;
		if (tss_stack_addr + 7 > CPU_TR_DESC.u.seg.limit) {
			EXCEPTION(TS_EXCEPTION, CPU_TR & ~3);
		}
		tss_stack_addr += CPU_TR_DESC.u.seg.segbase;
		*new_esp = cpu_kmemoryread_d(tss_stack_addr);
		*new_ss = cpu_kmemoryread_w(tss_stack_addr + 4);
	} else if (CPU_TR_DESC.type == CPU_SYSDESC_TYPE_TSS_BUSY_16) {
		tss_stack_addr = pl * 4 + 2;
		if (tss_stack_addr + 3 > CPU_TR_DESC.u.seg.limit) {
			EXCEPTION(TS_EXCEPTION, CPU_TR & ~3);
		}
		tss_stack_addr += CPU_TR_DESC.u.seg.segbase;
		*new_esp = cpu_kmemoryread_w(tss_stack_addr);
		*new_ss = cpu_kmemoryread_w(tss_stack_addr + 2);
	} else {
		ia32_panic("get_stack_pointer_from_tss: task register is invalid (%d)\n", CPU_TR_DESC.type);
	}

	VERBOSE(("get_stack_pointer_from_tss: pl = %d, new_esp = 0x%08x, new_ss = 0x%04x", pl, *new_esp, *new_ss));
}

UINT16
get_backlink_selector_from_tss(void)
{
	UINT16 backlink;

	if (CPU_TR_DESC.type == CPU_SYSDESC_TYPE_TSS_BUSY_32) {
		if (4 > CPU_TR_DESC.u.seg.limit) {
			EXCEPTION(TS_EXCEPTION, CPU_TR & ~3);
		}
	} else if (CPU_TR_DESC.type == CPU_SYSDESC_TYPE_TSS_BUSY_16) {
		if (2 > CPU_TR_DESC.u.seg.limit) {
			EXCEPTION(TS_EXCEPTION, CPU_TR & ~3);
		}
	} else {
		ia32_panic("get_backlink_selector_from_tss: task register is invalid (%d)\n", CPU_TR_DESC.type);
	}

	backlink = cpu_kmemoryread_w(CPU_TR_DESC.u.seg.segbase);
	VERBOSE(("get_backlink_selector_from_tss: backlink selector = 0x%04x", backlink));
	return backlink;
}

void
task_switch(selector_t *task_sel, task_switch_type_t type)
{
	UINT32 regs[CPU_REG_NUM];
	UINT32 eip;
	UINT32 new_flags;
	UINT32 cr3 = 0;
	UINT16 sreg[CPU_SEGREG_NUM];
	UINT16 ldtr;
	UINT16 iobase;
	UINT16 t;

	selector_t cs_sel;
	int rv;

	UINT32 cur_base;	/* current task state */
	UINT32 task_base;	/* new task state */
	UINT32 old_flags = REAL_EFLAGREG;
	BOOL task16;
	UINT i;

	VERBOSE(("task_switch: start"));

	/* limit check */
	switch (task_sel->desc.type) {
	case CPU_SYSDESC_TYPE_TSS_32:
	case CPU_SYSDESC_TYPE_TSS_BUSY_32:
		if (task_sel->desc.u.seg.limit < 0x67) {
			EXCEPTION(TS_EXCEPTION, task_sel->idx);
		}
		task16 = FALSE;
		break;

	case CPU_SYSDESC_TYPE_TSS_16:
	case CPU_SYSDESC_TYPE_TSS_BUSY_16:
		if (task_sel->desc.u.seg.limit < 0x2b) {
			EXCEPTION(TS_EXCEPTION, task_sel->idx);
		}
		task16 = TRUE;
		break;

	default:
		ia32_panic("task_switch: descriptor type is invalid.");
		task16 = FALSE;		/* compiler happy */
		break;
	}

	cur_base = CPU_TR_DESC.u.seg.segbase;
	task_base = task_sel->desc.u.seg.segbase;
	VERBOSE(("task_switch: cur task (%04x) = 0x%08x:%08x", CPU_TR, cur_base, CPU_TR_DESC.u.seg.limit));
	VERBOSE(("task_switch: new task (%04x) = 0x%08x:%08x", task_sel->selector, task_base, task_sel->desc.u.seg.limit));
	VERBOSE(("task_switch: %dbit task switch", task16 ? 16 : 32));

#if defined(MORE_DEBUG)
	{
		UINT32 v;

		VERBOSE(("task_switch: new task"));
		for (i = 0; i < task_sel->desc.u.seg.limit; i += 4) {
			v = cpu_kmemoryread_d(task_base + i);
			VERBOSE(("task_switch: 0x%08x: %08x", task_base + i,v));
		}
	}
#endif

	if (CPU_STAT_PAGING) {
		/* task state paging check */
		paging_check(cur_base, CPU_TR_DESC.u.seg.limit, CPU_PAGE_WRITE_DATA|CPU_MODE_SUPERVISER);
		paging_check(task_base, task_sel->desc.u.seg.limit, CPU_PAGE_WRITE_DATA|CPU_MODE_SUPERVISER);
	}

	/* load task state */
	memset(sreg, 0, sizeof(sreg));
	if (!task16) {
		if (CPU_STAT_PAGING) {
			cr3 = cpu_kmemoryread_d(task_base + 28);
		}
		eip = cpu_kmemoryread_d(task_base + 32);
		new_flags = cpu_kmemoryread_d(task_base + 36);
		for (i = 0; i < CPU_REG_NUM; i++) {
			regs[i] = cpu_kmemoryread_d(task_base + 40 + i * 4);
		}
		for (i = 0; i < CPU_SEGREG_NUM; i++) {
			sreg[i] = cpu_kmemoryread_w(task_base + 72 + i * 4);
		}
		ldtr = cpu_kmemoryread_w(task_base + 96);
		t = cpu_kmemoryread_w(task_base + 100);
		if (t & 1) {
			CPU_STAT_BP_EVENT |= CPU_STAT_BP_EVENT_TASK;
		}
		iobase = cpu_kmemoryread_w(task_base + 102);
	} else {
		eip = cpu_kmemoryread_w(task_base + 14);
		new_flags = cpu_kmemoryread_w(task_base + 16);
		for (i = 0; i < CPU_REG_NUM; i++) {
			regs[i] = cpu_kmemoryread_w(task_base + 18 + i * 2);
		}
		for (i = 0; i < CPU_SEGREG286_NUM; i++) {
			sreg[i] = cpu_kmemoryread_w(task_base + 34 + i * 2);
		}
		ldtr = cpu_kmemoryread_w(task_base + 42);
		iobase = 0;
		t = 0;
	}

#if defined(DEBUG)
	VERBOSE(("task_switch: current task"));
	if (!task16) {
		VERBOSE(("task_switch: CR3     = 0x%08x", CPU_CR3));
	}
	VERBOSE(("task_switch: eip     = 0x%08x", CPU_EIP));
	VERBOSE(("task_switch: eflags  = 0x%08x", old_flags));
	for (i = 0; i < CPU_REG_NUM; i++) {
		VERBOSE(("task_switch: regs[%d] = 0x%08x", i, CPU_REGS_DWORD(i)));
	}
	for (i = 0; i < CPU_SEGREG_NUM; i++) {
		VERBOSE(("task_switch: sreg[%d] = 0x%04x", i, CPU_REGS_SREG(i)));
	}
	VERBOSE(("task_switch: ldtr    = 0x%04x", CPU_LDTR));

	VERBOSE(("task_switch: new task"));
	if (!task16) {
		VERBOSE(("task_switch: CR3     = 0x%08x", cr3));
	}
	VERBOSE(("task_switch: eip     = 0x%08x", eip));
	VERBOSE(("task_switch: eflags  = 0x%08x", new_flags));
	for (i = 0; i < CPU_REG_NUM; i++) {
		VERBOSE(("task_switch: regs[%d] = 0x%08x", i, regs[i]));
	}
	for (i = 0; i < CPU_SEGREG_NUM; i++) {
		VERBOSE(("task_switch: sreg[%d] = 0x%04x", i, sreg[i]));
	}
	VERBOSE(("task_switch: ldtr    = 0x%04x", ldtr));
	if (!task16) {
		VERBOSE(("task_switch: t       = 0x%04x", t));
		VERBOSE(("task_switch: iobase  = 0x%04x", iobase));
	}
#endif

	/* if IRET or JMP, clear busy flag in this task: need */
	/* if IRET, clear NT_FLAG in current EFLAG: need */
	switch (type) {
	case TASK_SWITCH_IRET:
		/* clear NT_FLAG */
		old_flags &= ~NT_FLAG;
		/*FALLTHROUGH*/
	case TASK_SWITCH_JMP:
		/* clear busy flags in current task */
		CPU_SET_TASK_FREE(CPU_TR, &CPU_TR_DESC);
		break;

	case TASK_SWITCH_CALL:
	case TASK_SWITCH_INTR:
		/* Nothing to do */
		break;
	
	default:
		ia32_panic("task_switch(): task switch type is invalid");
		break;
	}

	/* save this task state in this task state segment */
	if (!task16) {
		cpu_kmemorywrite_d(cur_base + 32, CPU_EIP);
		cpu_kmemorywrite_d(cur_base + 36, old_flags);
		for (i = 0; i < CPU_REG_NUM; i++) {
			cpu_kmemorywrite_d(cur_base + 40 + i * 4, CPU_REGS_DWORD(i));
		}
		for (i = 0; i < CPU_SEGREG_NUM; i++) {
			cpu_kmemorywrite_w(cur_base + 72 + i * 4, CPU_REGS_SREG(i));
		}
	} else {
		cpu_kmemorywrite_w(cur_base + 14, CPU_IP);
		cpu_kmemorywrite_w(cur_base + 16, (UINT16)old_flags);
		for (i = 0; i < CPU_REG_NUM; i++) {
			cpu_kmemorywrite_w(cur_base + 18 + i * 2, CPU_REGS_WORD(i));
		}
		for (i = 0; i < CPU_SEGREG286_NUM; i++) {
			cpu_kmemorywrite_w(cur_base + 34 + i * 2, CPU_REGS_SREG(i));
		}
	}

#if defined(MORE_DEBUG)
	{
		UINT32 v;

		VERBOSE(("task_switch: current task"));
		for (i = 0; i < CPU_TR_DESC.u.seg.limit; i += 4) {
			v = cpu_kmemoryread_d(cur_base + i);
			VERBOSE(("task_switch: 0x%08x: %08x", cur_base + i, v));
		}
	}
#endif

	/* set back link selector */
	switch (type) {
	case TASK_SWITCH_CALL:
	case TASK_SWITCH_INTR:
		/* set back link selector */
		cpu_kmemorywrite_w(task_base, CPU_TR);
		break;
	
	case TASK_SWITCH_IRET:
	case TASK_SWITCH_JMP:
		/* Nothing to do */
		break;

	default:
		ia32_panic("task_switch(): task switch type is invalid");
		break;
	}

	/* Now task switching! */

	/* if CALL, INTR, set EFLAGS image NT_FLAG */
	/* if CALL, INTR, JMP set busy flag */
	switch (type) {
	case TASK_SWITCH_CALL:
	case TASK_SWITCH_INTR:
		/* set back link selector */
		new_flags |= NT_FLAG;
		/*FALLTHROUGH*/
	case TASK_SWITCH_JMP:
		CPU_SET_TASK_BUSY(task_sel->selector, &task_sel->desc);
		break;
	
	case TASK_SWITCH_IRET:
		/* check busy flag is active */
		if (task_sel->desc.valid) {
			UINT32 h;
			h = cpu_kmemoryread_d(task_sel->addr + 4);
			if ((h & CPU_TSS_H_BUSY) == 0) {
				ia32_panic("task_switch: new task is not busy");
			}
		}
		break;

	default:
		ia32_panic("task_switch(): task switch type is invalid");
		break;
	}

	/* set CR0 image CPU_CR0_TS */
	CPU_CR0 |= CPU_CR0_TS;

	/* load task selector to CPU_TR */
	CPU_TR = task_sel->selector;
	CPU_TR_DESC = task_sel->desc;

	/* load task state (CR3, EFLAG, EIP, GPR, segreg, LDTR) */

	/* set new CR3 */
	if (!task16 && CPU_STAT_PAGING) {
		set_CR3(cr3);
	}

	/* set new EIP, GPR */
	CPU_PREV_EIP = CPU_EIP = eip;
	for (i = 0; i < CPU_REG_NUM; i++) {
		CPU_REGS_DWORD(i) = regs[i];
	}
	for (i = 0; i < CPU_SEGREG_NUM; i++) {
		CPU_REGS_SREG(i) = sreg[i];
		CPU_STAT_SREG_INIT(i);
	}

	/* set new EFLAGS */
	set_eflags(new_flags, I_FLAG|IOPL_FLAG|RF_FLAG|VM_FLAG|VIF_FLAG|VIP_FLAG);

	/* I/O deny bitmap */
	if (!task16) {
		if (iobase != 0 && iobase < task_sel->desc.u.seg.limit) {
			CPU_STAT_IOLIMIT = (UINT16)(task_sel->desc.u.seg.limit - iobase);
			CPU_STAT_IOADDR = task_sel->desc.u.seg.segbase + iobase;
		} else {
			CPU_STAT_IOLIMIT = 0;
		}
	} else {
		CPU_STAT_IOLIMIT = 0;
	}
	VERBOSE(("task_switch: ioaddr = %08x, limit = %08x", CPU_STAT_IOADDR, CPU_STAT_IOLIMIT));

#if defined(IA32_SUPPORT_DEBUG_REGISTER)
	/* check resume flag */
	if (CPU_EFLAG & RF_FLAG) {
		CPU_STAT_BP_EVENT |= CPU_STAT_BP_EVENT_RF;
	}

	/* clear local break point flags */
	CPU_DR7 &= ~(CPU_DR7_L(0)|CPU_DR7_L(1)|CPU_DR7_L(2)|CPU_DR7_L(3)|CPU_DR7_LE);
	CPU_STAT_BP = 0;
	for (i = 0; i < CPU_DEBUG_REG_INDEX_NUM; i++) {
		if (CPU_DR7 & CPU_DR7_G(i)) {
			CPU_STAT_BP |= (1 << i);
		}
	}
#endif

	/* load new LDTR */
	load_ldtr(ldtr, TS_EXCEPTION);

	/* set new segment register */
	if (!CPU_STAT_VM86) {
		/* clear segment descriptor cache */
		for (i = 0; i < CPU_SEGREG_NUM; i++) {
			CPU_STAT_SREG_CLEAR(i);
		}

		/* load CS */
		rv = parse_selector(&cs_sel, sreg[CPU_CS_INDEX]);
		if (rv < 0) {
			VERBOSE(("task_switch: load CS failure (sel = 0x%04x, rv = %d)", sreg[CPU_CS_INDEX], rv));
			EXCEPTION(TS_EXCEPTION, cs_sel.idx);
		}

		/* CS register must be code segment */
		if (!cs_sel.desc.s || !cs_sel.desc.u.seg.c) {
			EXCEPTION(TS_EXCEPTION, cs_sel.idx);
		}

		/* check privilege level */
		if (!cs_sel.desc.u.seg.ec) {
			/* non-confirming code segment */
			if (cs_sel.desc.dpl != cs_sel.rpl) {
				EXCEPTION(TS_EXCEPTION, cs_sel.idx);
			}
		} else {
			/* confirming code segment */
			if (cs_sel.desc.dpl < cs_sel.rpl) {
				EXCEPTION(TS_EXCEPTION, cs_sel.idx);
			}
		}

		/* code segment is not present */
		rv = selector_is_not_present(&cs_sel);
		if (rv < 0) {
			EXCEPTION(NP_EXCEPTION, cs_sel.idx);
		}

		/* Now loading CS register */
		load_cs(cs_sel.selector, &cs_sel.desc, cs_sel.desc.dpl);

		/* load ES, SS, DS, FS, GS segment register */
		for (i = 0; i < CPU_SEGREG_NUM; i++) {
			if (i != CPU_CS_INDEX) {
				load_segreg(i, sreg[i], TS_EXCEPTION);
			}
		}
	}

	/* out of range */
	if (CPU_EIP > CPU_STAT_CS_LIMIT) {
		VERBOSE(("task_switch: new_ip is out of range. new_ip = %08x, limit = %08x", CPU_EIP, CPU_STAT_CS_LIMIT));
		EXCEPTION(GP_EXCEPTION, 0);
	}

	VERBOSE(("task_switch: done."));
}
