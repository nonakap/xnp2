/**
 * @file	pcm86.h
 * @brief	Interface of the 86-PCM
 */

#pragma once

#include "sound.h"
#include "nevent.h"

enum {
	PCM86_LOGICALBUF	= 0x8000,
	PCM86_BUFSIZE		= (1 << 16),
	PCM86_BUFMSK		= ((1 << 16) - 1),

	PCM86_DIVBIT		= 10,
	PCM86_DIVENV		= (1 << PCM86_DIVBIT),

	PCM86_RESCUE		= 20
};

#define RECALC_NOWCLKWAIT(p, cnt)										\
	do																	\
	{																	\
		(p)->nFifoRemain -= (cnt << (p)->cStepBits);					\
		if ((p)->nFifoRemain < 0)										\
		{																\
			(p)->nFifoRemain &= (p)->nStepMask;							\
		}																\
	} while (0 /*CONSTCOND*/)

/**
 * @brief PCM86
 */
struct tagPcm86
{
	SINT32	divremain;
	SINT32	div;
	SINT32	div2;
	SINT32	smp;
	SINT32	lastsmp;
	SINT32	smp_l;
	SINT32	lastsmp_l;
	SINT32	smp_r;
	SINT32	lastsmp_r;

	UINT nReadPos;				/*!< DSOUND再生位置 */
	UINT nWritePos;				/*!< 書込み位置 */
	SINT nBufferCount;			/*!< DSOUND用のデータ数 */
	SINT nFifoRemain;			/*!< 86PCM(bufsize:0x8000)のデータ数 */
	SINT nExtendBufferSize;		/*!< バッファの追加サイズ */

	SINT nFifoIntrSize;			/*!< 割り込み要求サイズ */
	SINT32	volume;
	SINT32	vol5;

	UINT32 nLastClock;			/*!< 最後に処理したクロック */
	UINT32 nStepClock;			/*!< サンプル1つのクロック */
	UINT nStepMask;				/*!< ステップ マスク */

	UINT8 cFifoCtrl;			/*!< FIFO コントロール */
	UINT8 cSoundFlags;			/*!< サウンド フラグ (A460) */
	UINT8 cDacCtrl;				/*!< DAC 設定 */
	UINT8 __write;
	UINT8 cStepBits;			/*!< PCM アライメント */
	UINT8 cIrqLevel;			/*!< 割り込みレベル */
	UINT8 __cReqIrq;
	UINT8 cIrqFlag;				/*!< 割り込みフラグ */

	UINT8	buffer[PCM86_BUFSIZE];
};
typedef struct tagPcm86	_PCM86;		/*!< define */
typedef struct tagPcm86	*PCM86;		/*!< define */

typedef struct {
	UINT	rate;
	UINT	vol;
} PCM86CFG;


#ifdef __cplusplus
extern "C"
{
#endif

extern const UINT pcm86rate8[];

void pcm86_cb(NEVENTITEM item);

void pcm86gen_initialize(UINT rate);
void pcm86gen_setvol(UINT vol);

void pcm86_reset(void);
void pcm86gen_update(void);
void pcm86_setpcmrate(REG8 val);
void pcm86_setnextintr(void);

void SOUNDCALL pcm86gen_checkbuf(PCM86 pcm86);
void SOUNDCALL pcm86gen_getpcm(PCM86 pcm86, SINT32 *lpBuffer, UINT nCount);

BOOL pcm86gen_intrq(void);

#ifdef __cplusplus
}
#endif
