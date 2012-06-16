/*	$Id: cpu_mem.c,v 1.21 2005/03/12 12:32:54 monaka Exp $	*/

/*
 * Copyright (c) 2002-2004 NONAKA Kimihiro
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
#include "memory.h"


/*
 * memory access check
 */
void
cpu_memoryread_check(descriptor_t *sd, UINT32 offset, UINT length, int e)
{
	UINT32 uplimit;

	if (CPU_STAT_PM) {
		/* invalid */
		if (!sd->valid) {
			VERBOSE(("cpu_memoryread_check: invalid"));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		/* not present */
		if (!sd->p) {
			VERBOSE(("cpu_memoryread_check: not present"));
			EXCEPTION(e, 0);
		}
	}

	switch (sd->type) {
	case 0:	 case 1:	/* ro */
	case 2:  case 3:	/* rw */
	case 10: case 11:	/* rx */
	case 14: case 15:	/* rxc */
		if (offset > sd->u.seg.limit - length + 1) {
			VERBOSE(("cpu_memoryread_check: offset(%08x) > sd->u.seg.limit(%08x) - length(%08x) + 1", offset, sd->u.seg.limit, length));
			EXCEPTION(e, 0);
		}
		if (length - 1 > sd->u.seg.limit) {
			VERBOSE(("cpu_memoryread_check: length(%08x) - 1 > sd->u.seg.limit(%08x)", length, sd->u.seg.limit));
			EXCEPTION(e, 0);
		}
		break;

	case 4:  case 5:	/* ro (expand down) */
	case 6:  case 7:	/* rw (expand down) */
		uplimit = sd->d ? 0xffffffff : 0x0000ffff;
		if (offset <= sd->u.seg.limit) {
			VERBOSE(("cpu_memoryread_check: offset(%08x) <= sd->u.seg.limit(%08x)", offset, sd->u.seg.limit));
			EXCEPTION(e, 0);
		}
		if (offset > uplimit) {
			VERBOSE(("cpu_memoryread_check: offset(%08x) > uplimit(%08x)", offset, uplimit));
			EXCEPTION(e, 0);
		}
		if (uplimit - offset < length - 1) {
			VERBOSE(("cpu_memoryread_check: uplimit(%08x) - offset(%08x) < length(%08x) - 1", uplimit, offset, length));
			EXCEPTION(e, 0);
		}
		break;

	default:
		VERBOSE(("cpu_memoryread_check: invalid type (type = %d)", sd->type));
		EXCEPTION(e, 0);
		break;
	}
	sd->flag |= CPU_DESC_FLAG_READABLE;
}

void
cpu_memorywrite_check(descriptor_t *sd, UINT32 offset, UINT length, int e)
{
	UINT32 uplimit;

	if (CPU_STAT_PM) {
		/* invalid */
		if (!sd->valid) {
			VERBOSE(("cpu_memorywrite_check: invalid"));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		/* not present */
		if (!sd->p) {
			VERBOSE(("cpu_memorywrite_check: not present"));
			EXCEPTION(e, 0);
		}

		if (!sd->s) {
			VERBOSE(("cpu_memorywrite_check: system segment"));
			EXCEPTION(e, 0);
		}
	}

	switch (sd->type) {
	case 2: case 3:	/* rw */
		if (offset > sd->u.seg.limit - length + 1) {
			VERBOSE(("cpu_memorywrite_check: offset(%08x) > sd->u.seg.limit(%08x) - length(%08x) + 1", offset, sd->u.seg.limit, length));
			EXCEPTION(e, 0);
		}
		if (length - 1 > sd->u.seg.limit) {
			VERBOSE(("cpu_memorywrite_check: length(%08x) - 1 > sd->u.seg.limit(%08x)", length, sd->u.seg.limit));
			EXCEPTION(e, 0);
		}
		break;

	case 6: case 7:	/* rw (expand down) */
		uplimit = sd->d ? 0xffffffff : 0x0000ffff;
		if (offset <= sd->u.seg.limit) {
			VERBOSE(("cpu_memorywrite_check: offset(%08x) <= sd->u.seg.limit(%08x)", offset, sd->u.seg.limit));
			EXCEPTION(e, 0);
		}
		if (offset > uplimit) {
			VERBOSE(("cpu_memorywrite_check: offset(%08x) > uplimit(%08x)", offset, uplimit));
			EXCEPTION(e, 0);
		}
		if (uplimit - offset < length - 1) {
			VERBOSE(("cpu_memorywrite_check: uplimit(%08x) - offset(%08x) < length(%08x) - 1", uplimit, offset, length));
			EXCEPTION(e, 0);
		}
		break;

	default:
		VERBOSE(("cpu_memorywrite_check: invalid type (type = %d)", sd->type));
		EXCEPTION(e, 0);
		break;
	}
	sd->flag |= CPU_DESC_FLAG_WRITABLE;
}

void
cpu_stack_push_check(UINT16 s, descriptor_t *sd, UINT32 esp, UINT length)
{
	UINT32 limit;

	if (CPU_STAT_PM) {
		if (!sd->valid || !sd->p) {
			VERBOSE(("cpu_stack_push_check: valid = %d, present = %d", sd->valid, sd->p));
			EXCEPTION(SS_EXCEPTION, s & 0xfffc);
		}
		if (!sd->s || sd->u.seg.c || !sd->u.seg.wr) {
			VERBOSE(("cpu_stack_push_check: s = %d, c = %d, wr", sd->s, sd->u.seg.c, sd->u.seg.wr));
			EXCEPTION(SS_EXCEPTION, s & 0xfffc);
		}

		if (!sd->d) {
			limit = 0xffff;
		} else {
			limit = 0xffffffff;
		}
		if (sd->u.seg.ec) {
			/* expand-down stack */
			if ((esp == 0)
			 || (esp < length)
			 || (esp - length <= sd->u.seg.limit)
			 || (esp > limit)) {
				VERBOSE(("cpu_stack_push_check: expand-down, esp = %08x, length = %08x", esp, length));
				VERBOSE(("cpu_stack_push_check: limit = %08x, seglimit = %08x", limit, sd->u.seg.limit));
				VERBOSE(("cpu_stack_push_check: segbase = %08x, segend = %08x", sd->u.seg.segbase, sd->u.seg.segend));
				EXCEPTION(SS_EXCEPTION, s & 0xfffc);
			}
		} else {
			/* expand-up stack */
			if (esp == 0) {
				if ((sd->d && (sd->u.seg.segend != 0xffffffff))
				 || (!sd->d && (sd->u.seg.segend != 0xffff))) {
					VERBOSE(("cpu_stack_push_check: expand-up, esp = %08x, length = %08x", esp, length));
					VERBOSE(("cpu_stack_push_check: limit = %08x, seglimit = %08x", limit, sd->u.seg.limit));
					VERBOSE(("cpu_stack_push_check: segbase = %08x, segend = %08x", sd->u.seg.segbase, sd->u.seg.segend));
					EXCEPTION(SS_EXCEPTION, s & 0xfffc);
				}
			} else {
				if ((esp < length)
				 || (esp - 1 > sd->u.seg.limit)) {
					VERBOSE(("cpu_stack_push_check: expand-up, esp = %08x, length = %08x", esp, length));
					VERBOSE(("cpu_stack_push_check: limit = %08x, seglimit = %08x", limit, sd->u.seg.limit));
					VERBOSE(("cpu_stack_push_check: segbase = %08x, segend = %08x", sd->u.seg.segbase, sd->u.seg.segend));
					EXCEPTION(SS_EXCEPTION, s & 0xfffc);
				}
			}
		}
	}
}

void
cpu_stack_pop_check(UINT16 s, descriptor_t *sd, UINT32 esp, UINT length)
{
	UINT32 limit;

	if (CPU_STAT_PM) {
		if (!sd->valid || !sd->p) {
			VERBOSE(("cpu_stack_pop_check: valid = %d, present = %d", sd->valid, sd->p));
			EXCEPTION(SS_EXCEPTION, s & 0xfffc);
		}
		if (!sd->s || sd->u.seg.c || !sd->u.seg.wr) {
			VERBOSE(("cpu_stack_pop_check: s = %d, c = %d, wr", sd->s, sd->u.seg.c, sd->u.seg.wr));
			EXCEPTION(SS_EXCEPTION, s & 0xfffc);
		}

		if (!sd->d) {
			limit = 0xffff;
		} else {
			limit = 0xffffffff;
		}
		if (sd->u.seg.ec) {
			/* expand-down stack */
			if ((esp == limit)
			 || ((limit - esp) + 1 < length)) {
				VERBOSE(("cpu_stack_pop_check: expand-up, esp = %08x, length = %08x", esp, length));
				VERBOSE(("cpu_stack_pop_check: limit = %08x, seglimit = %08x", limit, sd->u.seg.limit));
				VERBOSE(("cpu_stack_pop_check: segbase = %08x, segend = %08x", sd->u.seg.segbase, sd->u.seg.segend));
				EXCEPTION(SS_EXCEPTION, s & 0xfffc);
			}
		} else {
			/* expand-up stack */
			if ((esp == limit)
			 || (sd->u.seg.segend == 0)
			 || (esp > sd->u.seg.limit)
			 || ((sd->u.seg.limit - esp) + 1 < length)) {
				VERBOSE(("cpu_stack_pop_check: expand-up, esp = %08x, length = %08x", esp, length));
				VERBOSE(("cpu_stack_pop_check: limit = %08x, seglimit = %08x", limit, sd->u.seg.limit));
				VERBOSE(("cpu_stack_pop_check: segbase = %08x, segend = %08x", sd->u.seg.segbase, sd->u.seg.segend));
				EXCEPTION(SS_EXCEPTION, s & 0xfffc);
			}
		}
	}
}

#if defined(IA32_SUPPORT_DEBUG_REGISTER)
INLINE static void
check_memory_break_point(UINT32 address, UINT length, UINT rw)
{
	int i;

	if (CPU_STAT_BP && !(CPU_EFLAG & RF_FLAG)) {
		for (i = 0; i < CPU_DEBUG_REG_INDEX_NUM; i++) {
			if ((CPU_STAT_BP & (1 << i))
			 && (CPU_DR7_GET_RW(i) & rw)

			 && ((address <= CPU_DR(i) && address + length > CPU_DR(i))
			  || (address > CPU_DR(i) && address < CPU_DR(i) + CPU_DR7_GET_LEN(i)))) {
				CPU_STAT_BP_EVENT |= CPU_STAT_BP_EVENT_B(i);
			}
		}
	}
}
#else
#define	check_memory_break_point(address, length, rw)
#endif


/*
 * code fetch
 */
#define	ucrw	(CPU_PAGE_READ_CODE | CPU_STAT_USER_MODE)

UINT8 MEMCALL
cpu_codefetch(UINT32 offset)
{
	descriptor_t *sd;
	UINT32 addr;
#if defined(IA32_SUPPORT_TLB)
	TLB_ENTRY_T *ep;
#endif

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
	if (offset <= sd->u.seg.limit) {
		addr = sd->u.seg.segbase + offset;
		if (!CPU_STAT_PAGING)
			return cpu_memoryread(addr);
#if defined(IA32_SUPPORT_TLB)
		ep = tlb_lookup(addr, ucrw);
		if (ep != NULL && ep->memp != NULL) {
			return ep->memp[addr & 0xfff];
		}
#endif
		return cpu_linear_memory_read_b(addr, ucrw);
	}
	EXCEPTION(GP_EXCEPTION, 0);
	return 0;	/* compiler happy */
}

UINT16 MEMCALL
cpu_codefetch_w(UINT32 offset)
{
	descriptor_t *sd;
	UINT32 addr;
#if defined(IA32_SUPPORT_TLB)
	TLB_ENTRY_T *ep;
	UINT16 value;
#endif

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
	if (offset <= sd->u.seg.limit - 1) {
		addr = sd->u.seg.segbase + offset;
		if (!CPU_STAT_PAGING)
			return cpu_memoryread_w(addr);
#if defined(IA32_SUPPORT_TLB)
		ep = tlb_lookup(addr, ucrw);
		if (ep != NULL && ep->memp != NULL) {
			if ((addr + 1) & 0x00000fff) {
				return LOADINTELWORD(ep->memp + (addr & 0xfff));
			}
			value = ep->memp[0xfff];
			ep = tlb_lookup(addr + 1, ucrw);
			if (ep != NULL && ep->memp != NULL) {
				value += (UINT16)ep->memp[0] << 8;
				return value;
			}
		}
#endif
		return cpu_linear_memory_read_w(addr, ucrw);
	}
	EXCEPTION(GP_EXCEPTION, 0);
	return 0;	/* compiler happy */
}

UINT32 MEMCALL
cpu_codefetch_d(UINT32 offset)
{
	descriptor_t *sd;
	UINT32 addr;
#if defined(IA32_SUPPORT_TLB)
	TLB_ENTRY_T *ep[2];
	UINT32 value;
	UINT remain;
#endif

	sd = &CPU_STAT_SREG(CPU_CS_INDEX);
	if (offset <= sd->u.seg.limit - 3) {
		addr = sd->u.seg.segbase + offset;
		if (!CPU_STAT_PAGING)
			return cpu_memoryread_d(addr);
#if defined(IA32_SUPPORT_TLB)
		ep[0] = tlb_lookup(addr, ucrw);
		if (ep[0] != NULL && ep[0]->memp != NULL) {
			remain = 0x1000 - (addr & 0xfff);
			if (remain >= 4) {
				return LOADINTELDWORD(ep[0]->memp + (addr & 0xfff));
			}
			ep[1] = tlb_lookup(addr + remain, ucrw);
			if (ep[1] != NULL && ep[1]->memp != NULL) {
				switch (remain) {
				case 3:
					value = ep[0]->memp[0xffd];
					value += (UINT32)LOADINTELWORD(ep[0]->memp + 0xffe) << 8;
					value += (UINT32)ep[1]->memp[0] << 24;
					break;

				case 2:
					value = LOADINTELWORD(ep[0]->memp + 0xffe);
					value += (UINT32)LOADINTELWORD(ep[1]->memp + 0) << 16;
					break;

				case 1:
					value = ep[0]->memp[0xfff];
					value += (UINT32)LOADINTELWORD(ep[1]->memp + 0) << 8;
					value += (UINT32)ep[1]->memp[2] << 24;
					break;

				default:
					ia32_panic("cpu_codefetch_d(): out of range. (remain = %d)\n", remain);
					return (UINT32)-1;
				}
				return value;
			}
		}
#endif
		return cpu_linear_memory_read_d(addr, ucrw);
	}
	EXCEPTION(GP_EXCEPTION, 0);
	return 0;	/* compiler happy */
}

/*
 * additional physical address memory access functions
 */
UINT64 MEMCALL
cpu_memoryread_q(UINT32 address)
{
	UINT64 value;

	value = cpu_memoryread_d(address);
	value += (UINT64)cpu_memoryread_d(address + 4) << 32;

	return value;
}

REG80 MEMCALL
cpu_memoryread_f(UINT32 address)
{
	REG80 value;
	UINT i;

	for (i = 0; i < sizeof(REG80); ++i) {
		value.b[i] = cpu_memoryread(address + i);
	}
	return value;
}

void MEMCALL
cpu_memorywrite_q(UINT32 address, UINT64 value)
{

	cpu_memorywrite_d(address, (UINT32)value);
	cpu_memorywrite_d(address + 4, (UINT32)(value >> 32));
}

void MEMCALL
cpu_memorywrite_f(UINT32 address, const REG80 *value)
{
	UINT i;

	for (i = 0; i < sizeof(REG80); ++i) {
		cpu_memorywrite(address + i, value->b[i]);
	}
}

/*
 * virtual address memory access functions
 */
#include "cpu_mem.mcr"

VIRTUAL_ADDRESS_MEMORY_ACCESS_FUNCTION(b, UINT8, 1)
VIRTUAL_ADDRESS_MEMORY_ACCESS_FUNCTION(w, UINT16, 2)
VIRTUAL_ADDRESS_MEMORY_ACCESS_FUNCTION(d, UINT32, 4)

UINT64 MEMCALL
cpu_vmemoryread_q(int idx, UINT32 offset)
{
	descriptor_t *sd;
	UINT32 addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

	if (!(sd->flag & CPU_DESC_FLAG_READABLE)) {
		cpu_memoryread_check(sd, offset, 8,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 4: case 5: case 6: case 7:
			if (offset - (8 - 1) <= sd->u.seg.limit)
				goto range_failure;
			break;

		default:
			if (offset > sd->u.seg.limit - (8 - 1))
				goto range_failure;
			break;
		}
	} 
	addr = sd->u.seg.segbase + offset;
	check_memory_break_point(addr, 8, CPU_DR7_RW_RO);
	if (!CPU_STAT_PAGING)
		return cpu_memoryread_q(addr);
	return cpu_linear_memory_read_q(addr, CPU_PAGE_READ_DATA | CPU_STAT_USER_MODE);

range_failure:
	if (idx == CPU_SS_INDEX) {
		exc = SS_EXCEPTION;
	} else {
		exc = GP_EXCEPTION;
	}
	VERBOSE(("cpu_vmemoryread_q: type = %d, offset = %08x, limit = %08x", sd->type, offset, sd->u.seg.limit));
err:
	EXCEPTION(exc, 0);
	return 0;	/* compiler happy */
}

void MEMCALL
cpu_vmemorywrite_q(int idx, UINT32 offset, UINT64 value)
{
	descriptor_t *sd;
	UINT32 addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

	if (!(sd->flag & CPU_DESC_FLAG_WRITABLE)) {
		cpu_memorywrite_check(sd, offset, 8,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 6: case 7:
			if (offset - (8 - 1) <= sd->u.seg.limit)
				goto range_failure;
			break;

		default:
			if (offset > sd->u.seg.limit - (8 - 1))
				goto range_failure;
			break;
		}
	}
	addr = sd->u.seg.segbase + offset;
	check_memory_break_point(addr, 8, CPU_DR7_RW_RW);
	if (!CPU_STAT_PAGING) {
		cpu_memorywrite_q(addr, value);
	} else {
		cpu_linear_memory_write_q(addr, value, CPU_PAGE_WRITE_DATA | CPU_STAT_USER_MODE);
	}
	return;

range_failure:
	if (idx == CPU_SS_INDEX) {
		exc = SS_EXCEPTION;
	} else {
		exc = GP_EXCEPTION;
	}
	VERBOSE(("cpu_vmemorywrite_q: type = %d, offset = %08x, limit = %08x", sd->type, offset, sd->u.seg.limit));
err:
	EXCEPTION(exc, 0);
}

REG80 MEMCALL
cpu_vmemoryread_f(int idx, UINT32 offset)
{
	descriptor_t *sd;
	UINT32 addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

	if (!(sd->flag & CPU_DESC_FLAG_READABLE)) {
		cpu_memoryread_check(sd, offset, 10,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 4: case 5: case 6: case 7:
			if (offset - (10 - 1) <= sd->u.seg.limit)
				goto range_failure;
			break;

		default:
			if (offset > sd->u.seg.limit - (10 - 1))
				goto range_failure;
			break;
		}
	} 
	addr = sd->u.seg.segbase + offset;
	check_memory_break_point(addr, 10, CPU_DR7_RW_RO);
	if (!CPU_STAT_PAGING)
		return cpu_memoryread_f(addr);
	return cpu_linear_memory_read_f(addr, CPU_PAGE_READ_DATA | CPU_STAT_USER_MODE);

range_failure:
	if (idx == CPU_SS_INDEX) {
		exc = SS_EXCEPTION;
	} else {
		exc = GP_EXCEPTION;
	}
	VERBOSE(("cpu_vmemoryread_f: type = %d, offset = %08x, limit = %08x", sd->type, offset, sd->u.seg.limit));
err:
	EXCEPTION(exc, 0);
	{
		REG80 dummy;
		memset(&dummy, 0, sizeof(dummy));
		return dummy;	/* compiler happy */
	}
}

void MEMCALL
cpu_vmemorywrite_f(int idx, UINT32 offset, const REG80 *value)
{
	descriptor_t *sd;
	UINT32 addr;
	int exc;

	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM);

	sd = &CPU_STAT_SREG(idx);
	if (!sd->valid) {
		exc = GP_EXCEPTION;
		goto err;
	}

	if (!(sd->flag & CPU_DESC_FLAG_WRITABLE)) {
		cpu_memorywrite_check(sd, offset, 10,
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION);
	} else {
		switch (sd->type) {
		case 6: case 7:
			if (offset - (10 - 1) <= sd->u.seg.limit)
				goto range_failure;
			break;

		default:
			if (offset > sd->u.seg.limit - (10 - 1))
				goto range_failure;
			break;
		}
	}
	addr = sd->u.seg.segbase + offset;
	check_memory_break_point(addr, 10, CPU_DR7_RW_RW);
	if (!CPU_STAT_PAGING) {
		cpu_memorywrite_f(addr, value);
	} else {
		cpu_linear_memory_write_f(addr, value, CPU_PAGE_WRITE_DATA | CPU_STAT_USER_MODE);
	}
	return;

range_failure:
	if (idx == CPU_SS_INDEX) {
		exc = SS_EXCEPTION;
	} else {
		exc = GP_EXCEPTION;
	}
	VERBOSE(("cpu_vmemorywrite_f: type = %d, offset = %08x, limit = %08x", sd->type, offset, sd->u.seg.limit));
err:
	EXCEPTION(exc, 0);
}
