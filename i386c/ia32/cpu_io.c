/*	$Id: cpu_io.c,v 1.8 2007/02/06 14:20:57 monaka Exp $	*/

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
#include "pccore.h"
#include "iocore.h"
#include "memory.h"

static void IOOUTCALL check_io(UINT port, UINT len);

static void IOOUTCALL
check_io(UINT port, UINT len) 
{
	UINT off;
	UINT8 bit;
	UINT8 map;

	if (CPU_STAT_IOLIMIT == 0) {
		VERBOSE(("check_io: CPU_STAT_IOLIMIT == 0 (port = %04x, len = %d)", port, len));
		EXCEPTION(GP_EXCEPTION, 0);
	}

	off = port / 8;
	bit = 1 << (port % 8);
	for (; len > 0; ++off, bit = 0x01) {
		if (off >= CPU_STAT_IOLIMIT) {
			VERBOSE(("check_io: off(%08x) >= CPU_STAT_IOLIMIT(%08x) (port = %04x, len = %d)", off, CPU_STAT_IOLIMIT, port, len));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		map = cpu_kmemoryread(CPU_STAT_IOADDR + off);
		for (; (len > 0) && (bit != 0x00); bit <<= 1, --len) {
			if (map & bit) {
				VERBOSE(("check_io: (bitmap(0x%02x) & bit(0x%02x)) != 0 (port = %04x, len = %d)", map, bit, port, len));
				EXCEPTION(GP_EXCEPTION, 0);
			}
		}
	}
}

#if defined(IA32_SUPPORT_DEBUG_REGISTER) && CPU_FAMILY >= 5
INLINE static void IOOUTCALL
check_ioport_break_point(UINT port, UINT length)
{
	int i;

	if (CPU_STAT_BP && !(CPU_EFLAG & RF_FLAG)) {
		for (i = 0; i < CPU_DEBUG_REG_INDEX_NUM; i++) {
			if ((CPU_STAT_BP & (1 << i))
			 && (CPU_DR7_GET_RW(i) == CPU_DR7_RW_IO)

			 && ((port <= CPU_DR(i) && port + length > CPU_DR(i))
			  || (port > CPU_DR(i) && port <= CPU_DR(i) + CPU_DR7_GET_LEN(i)))) {
				CPU_STAT_BP_EVENT |= CPU_STAT_BP_EVENT_B(i);
			}
		}
	}
}
#else
#define	check_ioport_break_point(port, length)
#endif

UINT8
cpu_in(UINT port)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 1);
	}
	check_ioport_break_point(port, 1);
	return iocore_inp8(port);
}

UINT16
cpu_in_w(UINT port)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 2);
	}
	check_ioport_break_point(port, 2);
	return iocore_inp16(port);
}

UINT32
cpu_in_d(UINT port)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 4);
	}
	check_ioport_break_point(port, 4);
	return iocore_inp32(port);
}

void
cpu_out(UINT port, UINT8 data)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 1);
	}
	check_ioport_break_point(port, 1);
	iocore_out8(port, data);
}

void
cpu_out_w(UINT port, UINT16 data)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 2);
	}
	check_ioport_break_point(port, 2);
	iocore_out16(port, data);
}

void
cpu_out_d(UINT port, UINT32 data)
{

	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL > CPU_STAT_IOPL))) {
		check_io(port, 4);
	}
	check_ioport_break_point(port, 4);
	iocore_out32(port, data);
}
