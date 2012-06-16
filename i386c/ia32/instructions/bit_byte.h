/*	$Id: bit_byte.h,v 1.3 2005/03/12 12:33:47 monaka Exp $	*/

/*
 * Copyright (c) 2002-2003 NONAKA Kimihiro
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

#ifndef	IA32_CPU_INSTRUCTION_BIT_BYTE_H__
#define	IA32_CPU_INSTRUCTION_BIT_BYTE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * BTx
 */
void BT_EwGw(void);
void BT_EdGd(void);
void BT_EwIb(UINT32);
void BT_EdIb(UINT32);
void BTS_EwGw(void);
void BTS_EdGd(void);
void BTS_EwIb(UINT32);
void BTS_EdIb(UINT32);
void BTR_EwGw(void);
void BTR_EdGd(void);
void BTR_EwIb(UINT32);
void BTR_EdIb(UINT32);
void BTC_EwGw(void);
void BTC_EdGd(void);
void BTC_EwIb(UINT32);
void BTC_EdIb(UINT32);

/*
 * BSx
 */
void BSF_GwEw(void);
void BSF_GdEd(void);
void BSR_GwEw(void);
void BSR_GdEd(void);

/*
 * SETcc
 */
void SETO_Eb(void);
void SETNO_Eb(void);
void SETC_Eb(void);
void SETNC_Eb(void);
void SETZ_Eb(void);
void SETNZ_Eb(void);
void SETA_Eb(void);
void SETNA_Eb(void);
void SETS_Eb(void);
void SETNS_Eb(void);
void SETP_Eb(void);
void SETNP_Eb(void);
void SETL_Eb(void);
void SETNL_Eb(void);
void SETLE_Eb(void);
void SETNLE_Eb(void);

/*
 * TEST
 */
void TEST_EbGb(void);
void TEST_EwGw(void);
void TEST_EdGd(void);
void TEST_ALIb(void);
void TEST_AXIw(void);
void TEST_EAXId(void);

void TEST_EbIb(UINT32);
void TEST_EwIw(UINT32);
void TEST_EdId(UINT32);

#ifdef __cplusplus
}
#endif

#endif	/* IA32_CPU_INSTRUCTION_BIT_BYTE_H__ */
