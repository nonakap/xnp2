/*	$Id: cpu_mem.mcr,v 1.4 2005/03/12 12:32:54 monaka Exp $	*/

/*
 * Copyright (c) 2004 NONAKA Kimihiro
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

#define	VIRTUAL_ADDRESS_MEMORY_ACCESS_FUNCTION(width, valtype, length) \
valtype MEMCALL \
cpu_vmemoryread_##width(int idx, UINT32 offset) \
{ \
	descriptor_t *sd; \
	UINT32 addr; \
	int exc; \
\
	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM); \
\
	sd = &CPU_STAT_SREG(idx); \
	if (!sd->valid) { \
		exc = GP_EXCEPTION; \
		goto err; \
	} \
\
	if (!(sd->flag & CPU_DESC_FLAG_READABLE)) { \
		cpu_memoryread_check(sd, offset, (length), \
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION); \
	} else { \
		switch (sd->type) { \
		case 4: case 5: case 6: case 7: \
			if (offset - ((length) - 1) <= sd->u.seg.limit) \
				goto range_failure; \
			break; \
\
		default: \
			if (offset > sd->u.seg.limit - ((length) - 1)) \
				goto range_failure; \
			break; \
		} \
	}  \
	addr = sd->u.seg.segbase + offset; \
	check_memory_break_point(addr, (length), CPU_DR7_RW_RO); \
	if (!CPU_STAT_PAGING) \
		return cpu_memoryread_##width(addr); \
	return cpu_linear_memory_read_##width(addr, CPU_PAGE_READ_DATA | CPU_STAT_USER_MODE); \
\
range_failure: \
	if (idx == CPU_SS_INDEX) { \
		exc = SS_EXCEPTION; \
	} else { \
		exc = GP_EXCEPTION; \
	} \
	VERBOSE(("cpu_vmemoryread: type = %d, offset = %08x, length = %d, limit = %08x", sd->type, offset, length, sd->u.seg.limit)); \
err: \
	EXCEPTION(exc, 0); \
	return 0;	/* compiler happy */ \
} \
\
void MEMCALL \
cpu_vmemorywrite_##width(int idx, UINT32 offset, valtype value) \
{ \
	descriptor_t *sd; \
	UINT32 addr; \
	int exc; \
\
	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM); \
\
	sd = &CPU_STAT_SREG(idx); \
	if (!sd->valid) { \
		exc = GP_EXCEPTION; \
		goto err; \
	} \
\
	if (!(sd->flag & CPU_DESC_FLAG_WRITABLE)) { \
		cpu_memorywrite_check(sd, offset, (length), \
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION); \
	} else { \
		switch (sd->type) { \
		case 6: case 7: \
			if (offset - ((length) - 1) <= sd->u.seg.limit) \
				goto range_failure; \
			break; \
\
		default: \
			if (offset > sd->u.seg.limit - ((length) - 1)) \
				goto range_failure; \
			break; \
		} \
	} \
	addr = sd->u.seg.segbase + offset; \
	check_memory_break_point(addr, (length), CPU_DR7_RW_RW); \
	if (!CPU_STAT_PAGING) { \
		cpu_memorywrite_##width(addr, value); \
	} else { \
		cpu_linear_memory_write_##width(addr, value, CPU_PAGE_WRITE_DATA | CPU_STAT_USER_MODE); \
	} \
	return; \
\
range_failure: \
	if (idx == CPU_SS_INDEX) { \
		exc = SS_EXCEPTION; \
	} else { \
		exc = GP_EXCEPTION; \
	} \
	VERBOSE(("cpu_vmemorywrite: type = %d, offset = %08x, length = %d, limit = %08x", sd->type, offset, length, sd->u.seg.limit)); \
err: \
	EXCEPTION(exc, 0); \
} \
\
UINT32 MEMCALL \
cpu_memory_access_va_RMW_##width(int idx, UINT32 offset, UINT32 (*func)(UINT32, void *), void *arg) \
{ \
	descriptor_t *sd; \
	UINT32 addr; \
	UINT32 res, dst; \
	int exc; \
\
	__ASSERT((unsigned int)idx < CPU_SEGREG_NUM); \
\
	sd = &CPU_STAT_SREG(idx); \
	if (!sd->valid) { \
		exc = GP_EXCEPTION; \
		goto err; \
	} \
\
	if (!(sd->flag & CPU_DESC_FLAG_WRITABLE)) { \
		cpu_memorywrite_check(sd, offset, (length), \
		    (idx == CPU_SS_INDEX) ? SS_EXCEPTION : GP_EXCEPTION); \
	} else { \
		switch (sd->type) { \
		case 6: case 7: \
			if (offset - ((length) - 1) <= sd->u.seg.limit) \
				goto range_failure; \
			break; \
\
		default: \
			if (offset > sd->u.seg.limit - ((length) - 1)) \
				goto range_failure; \
			break; \
		} \
	} \
	addr = sd->u.seg.segbase + offset; \
	check_memory_break_point(addr, (length), CPU_DR7_RW_RW); \
	if (!CPU_STAT_PAGING) { \
		dst = cpu_memoryread_##width(addr); \
		res = (*func)(dst, arg); \
		cpu_memorywrite_##width(addr, res); \
	} else { \
		dst = cpu_memory_access_la_RMW_##width(addr, func, arg); \
	} \
	return dst; \
\
range_failure: \
	if (idx == CPU_SS_INDEX) { \
		exc = SS_EXCEPTION; \
	} else { \
		exc = GP_EXCEPTION; \
	} \
	VERBOSE(("cpu_memory_access_va_RMW: type = %d, offset = %08x, length = %d, limit = %08x", sd->type, offset, length, sd->u.seg.limit)); \
err: \
	EXCEPTION(exc, 0); \
	return 0;	/* compiler happy */ \
}
