/*	$Id: shift_rotate.h,v 1.4 2005/03/12 12:33:48 monaka Exp $	*/

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

#ifndef	IA32_CPU_INSTRUCTION_SHIFT_ROTATE_H__
#define	IA32_CPU_INSTRUCTION_SHIFT_ROTATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SAR
 */
void SAR_Eb(UINT8 *);
void SAR_Ew(UINT16 *);
void SAR_Ed(UINT32 *);
void SAR_Eb_ext(UINT32);
void SAR_Ew_ext(UINT32);
void SAR_Ed_ext(UINT32);
void SAR_EbCL(UINT8 *, UINT32);
void SAR_EbCL_ext(UINT32, UINT32);
void SAR_EwCL(UINT16 *, UINT32);
void SAR_EwCL_ext(UINT32, UINT32);
void SAR_EdCL(UINT32 *, UINT32);
void SAR_EdCL_ext(UINT32, UINT32);

/*
 * SHR
 */
void SHR_Eb(UINT8 *);
void SHR_Ew(UINT16 *);
void SHR_Ed(UINT32 *);
void SHR_Eb_ext(UINT32);
void SHR_Ew_ext(UINT32);
void SHR_Ed_ext(UINT32);
void SHR_EbCL(UINT8 *, UINT32);
void SHR_EbCL_ext(UINT32, UINT32);
void SHR_EwCL(UINT16 *, UINT32);
void SHR_EwCL_ext(UINT32, UINT32);
void SHR_EdCL(UINT32 *, UINT32);
void SHR_EdCL_ext(UINT32, UINT32);

/*
 * SHL
 */
void SHL_Eb(UINT8 *);
void SHL_Ew(UINT16 *);
void SHL_Ed(UINT32 *);
void SHL_Eb_ext(UINT32);
void SHL_Ew_ext(UINT32);
void SHL_Ed_ext(UINT32);
void SHL_EbCL(UINT8 *, UINT32);
void SHL_EbCL_ext(UINT32, UINT32);
void SHL_EwCL(UINT16 *, UINT32);
void SHL_EwCL_ext(UINT32, UINT32);
void SHL_EdCL(UINT32 *, UINT32);
void SHL_EdCL_ext(UINT32, UINT32);

/*
 * SHRD
 */
void SHRD_EwGwIb(void);
void SHRD_EdGdIb(void);
void SHRD_EwGwCL(void);
void SHRD_EdGdCL(void);

/*
 * SHLD
 */
void SHLD_EwGwIb(void);
void SHLD_EdGdIb(void);
void SHLD_EwGwCL(void);
void SHLD_EdGdCL(void);

/*
 * ROR
 */
void ROR_Eb(UINT8 *);
void ROR_Ew(UINT16 *);
void ROR_Ed(UINT32 *);
void ROR_Eb_ext(UINT32);
void ROR_Ew_ext(UINT32);
void ROR_Ed_ext(UINT32);
void ROR_EbCL(UINT8 *, UINT32);
void ROR_EbCL_ext(UINT32, UINT32);
void ROR_EwCL(UINT16 *, UINT32);
void ROR_EwCL_ext(UINT32, UINT32);
void ROR_EdCL(UINT32 *, UINT32);
void ROR_EdCL_ext(UINT32, UINT32);

/*
 * ROL
 */
void ROL_Eb(UINT8 *);
void ROL_Ew(UINT16 *);
void ROL_Ed(UINT32 *);
void ROL_Eb_ext(UINT32);
void ROL_Ew_ext(UINT32);
void ROL_Ed_ext(UINT32);
void ROL_EbCL(UINT8 *, UINT32);
void ROL_EbCL_ext(UINT32, UINT32);
void ROL_EwCL(UINT16 *, UINT32);
void ROL_EwCL_ext(UINT32, UINT32);
void ROL_EdCL(UINT32 *, UINT32);
void ROL_EdCL_ext(UINT32, UINT32);

/*
 * RCR
 */
void RCR_Eb(UINT8 *);
void RCR_Ew(UINT16 *);
void RCR_Ed(UINT32 *);
void RCR_Eb_ext(UINT32);
void RCR_Ew_ext(UINT32);
void RCR_Ed_ext(UINT32);
void RCR_EbCL(UINT8 *, UINT32);
void RCR_EbCL_ext(UINT32, UINT32);
void RCR_EwCL(UINT16 *, UINT32);
void RCR_EwCL_ext(UINT32, UINT32);
void RCR_EdCL(UINT32 *, UINT32);
void RCR_EdCL_ext(UINT32, UINT32);

/*
 * RCL
 */
void RCL_Eb(UINT8 *);
void RCL_Ew(UINT16 *);
void RCL_Ed(UINT32 *);
void RCL_Eb_ext(UINT32);
void RCL_Ew_ext(UINT32);
void RCL_Ed_ext(UINT32);
void RCL_EbCL(UINT8 *, UINT32);
void RCL_EbCL_ext(UINT32, UINT32);
void RCL_EwCL(UINT16 *, UINT32);
void RCL_EwCL_ext(UINT32, UINT32);
void RCL_EdCL(UINT32 *, UINT32);
void RCL_EdCL_ext(UINT32, UINT32);

#ifdef __cplusplus
}
#endif

#endif	/* IA32_CPU_INSTRUCTION_SHIFT_ROTATE_H__ */
