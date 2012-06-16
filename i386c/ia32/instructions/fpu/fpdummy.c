/*	$Id: fpdummy.c,v 1.7 2004/03/26 06:33:53 yui Exp $	*/

#include "compiler.h"
#include "cpu.h"
#include "ia32.mcr"
#include "fp.h"


void
FWAIT(void)
{
//	TRACEOUT(("use FPU - FWAIT"));
}

void
ESC0(void)
{
	UINT32 op, madr;

	GET_PCBYTE(op);
//	TRACEOUT(("use FPU d8 %.2x", op));
	if (op >= 0xc0) {
	} else {
		madr = calc_ea_dst(op);
	}
}

void
ESC1(void)
{
	UINT32 op, madr;

	GET_PCBYTE(op);
//	TRACEOUT(("use FPU d9 %.2x", op));
	if (op >= 0xc0) {
	} else {
		madr = calc_ea_dst(op);
		if ((op & 0x38) == 0x38) {
			TRACEOUT(("FSTCW"));
			cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, 0xffff);
		}
	}
}

void
ESC2(void)
{
	UINT32 op, madr;

	GET_PCBYTE(op);
//	TRACEOUT(("use FPU da %.2x", op));
	if (op >= 0xc0) {
	} else {
		madr = calc_ea_dst(op);
	}
}

void
ESC3(void)
{
	UINT32 op, madr;

	GET_PCBYTE(op);
//	TRACEOUT(("use FPU db %.2x", op));
	if (op >= 0xc0) {
	} else {
		madr = calc_ea_dst(op);
	}
}

void
ESC4(void)
{
	UINT32 op, madr;

	GET_PCBYTE(op);
//	TRACEOUT(("use FPU dc %.2x", op));
	if (op >= 0xc0) {
	} else {
		madr = calc_ea_dst(op);
	}
}

void
ESC5(void)
{
	UINT32 op, madr;

	GET_PCBYTE(op);
//	TRACEOUT(("use FPU dd %.2x", op));
	if (op >= 0xc0) {
	} else {
		madr = calc_ea_dst(op);
	}
}

void
ESC6(void)
{
	UINT32 op, madr;

	GET_PCBYTE(op);
//	TRACEOUT(("use FPU de %.2x", op));
	if (op >= 0xc0) {
	} else {
		madr = calc_ea_dst(op);
	}
}

void
ESC7(void)
{
	UINT32 op, madr;

	GET_PCBYTE(op);
//	TRACEOUT(("use FPU df %.2x", op));
	if (op >= 0xc0) {
	} else {
		madr = calc_ea_dst(op);
	}
}
