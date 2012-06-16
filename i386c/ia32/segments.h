/*	$Id: segments.h,v 1.10 2005/03/12 12:32:54 monaka Exp $	*/

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

#ifndef	IA32_CPU_SEGMENTS_H__
#define	IA32_CPU_SEGMENTS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * segment descriptor
 */
typedef struct {
	union {
		struct {
			UINT32	segbase;
			UINT32	segend;
			UINT32	limit;

			UINT8	c;	/* 0 = data, 1 = code */
			UINT8	g;	/* 4k base */
			UINT8	wr;	/* readable/writable */
			UINT8	ec;	/* expand down/conforming */
		} seg;

		struct {
			UINT16	selector;
			UINT16	w_pad;
			UINT32	offset;

			UINT8	count;		/* parameter count:call gate */

			UINT8	b_pad[7];
		} gate;
	} u;

	UINT8	valid;	/* descriptor valid flag */
	UINT8	p;	/* avail flag */

	UINT8	type;	/* descriptor type */
	UINT8	dpl;	/* DPL */
	UINT8	rpl;	/* RPL */
	UINT8	s;	/* 0 = system, 1 = code/data */
	UINT8	d;	/* 0 = 16bit, 1 = 32bit */

	UINT8	flag;
#define	CPU_DESC_FLAG_READABLE	(1 << 0)
#define	CPU_DESC_FLAG_WRITABLE	(1 << 1)
} descriptor_t;


/*
 * セグメント・ディスクリプタ
 *
 *  31            24 23 22 21 20 19   16 15 14 13 12 11    8 7             0
 * +----------------+--+--+--+--+-------+--+-----+--+-------+---------------+
 * |  Base  31..16  | G|DB| 0| A|limit_h| P| DPL | S|  type |  Base  23:16  | 4
 * +----------------+--+--+--+--+-------+--+-----+--+-------+---------------+
 *  31                                16 15                                0
 * +------------------------------------+-----------------------------------+
 * |           Base  15..00             |            limit  15..0           | 0
 * +------------------------------------+-----------------------------------+
 */

/* descriptor common */
#define	CPU_DESC_H_TYPE		(0xf <<  8)
#define	CPU_DESC_H_S		(  1 << 12)	/* 0 = system, 1 = code/data */
#define	CPU_DESC_H_DPL		(  3 << 13)
#define	CPU_DESC_H_P		(  1 << 15)	/* exist */

/* for segment descriptor */
#define	CPU_SEGDESC_H_A		(  1 <<  8)
#define	CPU_SEGDESC_H_D_C	(  1 << 11)	/* 0 = data, 1 = code */
#define	CPU_SEGDESC_H_D		(  1 << 22)
#define	CPU_SEGDESC_H_G		(  1 << 23)

/* for gate descriptor */
#define	CPU_GATEDESC_H_D	(  1 << 11)

/* for tss descriptor */
#define	CPU_TSS_H_BUSY		(  1 <<  9)

/*
 * descriptor type
 */
#define	CPU_SEGDESC_TYPE_A		0x01
#define	CPU_SEGDESC_TYPE_WR		0x02
#define	CPU_SEGDESC_TYPE_EC		0x04

#define	CPU_SYSDESC_TYPE_TSS_16		0x01
#define	CPU_SYSDESC_TYPE_LDT		0x02
#define	CPU_SYSDESC_TYPE_TSS_BUSY_16	0x03
#define	CPU_SYSDESC_TYPE_CALL_16	0x04	/* call gate */
#define	CPU_SYSDESC_TYPE_TASK		0x05	/* task gate */
#define	CPU_SYSDESC_TYPE_INTR_16	0x06	/* hardware interrupt */
#define	CPU_SYSDESC_TYPE_TRAP_16	0x07	/* software interrupt */
#define	CPU_SYSDESC_TYPE_TSS_32		0x09
#define	CPU_SYSDESC_TYPE_TSS_BUSY_32	0x0b
#define	CPU_SYSDESC_TYPE_CALL_32	0x0c	/* call gate */
#define	CPU_SYSDESC_TYPE_INTR_32	0x0e	/* hardware interrupt */
#define	CPU_SYSDESC_TYPE_TRAP_32	0x0f	/* software interrupt */

#define	CPU_SYSDESC_TYPE_TSS		0x01
/*	CPU_SYSDESC_TYPE_LDT		0x02	*/
#define	CPU_SYSDESC_TYPE_TSS_BUSY	0x03
#define	CPU_SYSDESC_TYPE_CALL		0x04
/*	CPU_SYSDESC_TYPE_TASK		0x05	*/
#define	CPU_SYSDESC_TYPE_INTR		0x06
#define	CPU_SYSDESC_TYPE_TRAP		0x07
#define	CPU_SYSDESC_TYPE_MASKBIT	0x07

#define	CPU_SYSDESC_TYPE_TSS_BUSY_IND	0x02

#define	CPU_SET_SEGDESC_DEFAULT(dscp, idx, selector) \
do { \
	(dscp)->u.seg.segbase = (UINT32)(selector) << 4; \
	(dscp)->u.seg.segend = (dscp)->u.seg.segbase + (dscp)->u.seg.limit; \
	(dscp)->u.seg.c = ((idx) == CPU_CS_INDEX) ? 1 : 0; \
	(dscp)->u.seg.g = 0; \
	(dscp)->u.seg.wr = 1; \
	(dscp)->valid = 1; \
	(dscp)->p = 1; \
	(dscp)->type = 0x02; /* writable */ \
	(dscp)->dpl = 0; \
	(dscp)->s = 1;	/* code/data */ \
	(dscp)->d = 0; /* 16bit */ \
	(dscp)->flag = CPU_DESC_FLAG_READABLE|CPU_DESC_FLAG_WRITABLE; \
} while (/*CONSTCOND*/ 0)

#define	CPU_SET_TASK_BUSY(selector, dscp) \
do { \
	UINT32 addr; \
	UINT32 h; \
	addr = CPU_GDTR_BASE + ((selector) & CPU_SEGMENT_SELECTOR_INDEX_MASK); \
	h = cpu_kmemoryread_d(addr + 4); \
	if (!(h & CPU_TSS_H_BUSY)) { \
		(dscp)->type |= CPU_SYSDESC_TYPE_TSS_BUSY_IND; \
		h |= CPU_TSS_H_BUSY; \
		cpu_kmemorywrite_d(addr + 4, h); \
	} else { \
		ia32_panic("CPU_SET_TASK_BUSY: already busy (%04x:%08x)", selector, h); \
	} \
} while (/*CONSTCOND*/ 0)

#define	CPU_SET_TASK_FREE(selector, dscp) \
do { \
	UINT32 addr; \
	UINT32 h; \
	addr = CPU_GDTR_BASE + ((selector) & CPU_SEGMENT_SELECTOR_INDEX_MASK); \
	h = cpu_kmemoryread_d(addr + 4); \
	if (h & CPU_TSS_H_BUSY) { \
		(dscp)->type &= ~CPU_SYSDESC_TYPE_TSS_BUSY_IND; \
		h &= ~CPU_TSS_H_BUSY; \
		cpu_kmemorywrite_d(addr + 4, h); \
	} else { \
		ia32_panic("CPU_SET_TASK_FREE: already free (%04x:%08x)", selector, h); \
	} \
} while (/*CONSTCOND*/ 0)

void load_descriptor(descriptor_t *descp, UINT32 addr);

#define	CPU_SET_SEGREG(idx, selector)	load_segreg(idx, selector, GP_EXCEPTION)
void load_segreg(int idx, UINT16 selector, int exc);
void load_ss(UINT16 selector, const descriptor_t *sd, UINT cpl);
void load_cs(UINT16 selector, const descriptor_t *sd, UINT cpl);
void load_ldtr(UINT16 selector, int exc);


/*
 * segment selector
 */
#define	CPU_SEGMENT_SELECTOR_INDEX_MASK	(~7)
#define	CPU_SEGMENT_SELECTOR_RPL_MASK	(3)
#define	CPU_SEGMENT_TABLE_IND		(1 << 2)	/* 0 = GDT, 1 = LDT */

typedef struct {
	UINT16		selector;
	UINT16		idx;
	UINT16		rpl;
	UINT8		ldt;
	UINT8		pad;

	UINT32		addr;		/* descriptor linear address */

	descriptor_t	desc;
} selector_t;

int parse_selector(selector_t *ssp, UINT16 selector);
int selector_is_not_present(const selector_t *ssp);

#ifdef __cplusplus
}
#endif

#endif	/* !IA32_CPU_SEGMENTS_H__ */
