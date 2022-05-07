/**
 * @file	pcm86c.c
 * @brief	Implementation of the 86-PCM
 */

#include "compiler.h"
#include "pcm86.h"
#include "pccore.h"
#include "cpucore.h"
#include "iocore.h"
#include "fmboard.h"

/* サンプリングレートに8掛けた物 */
const UINT pcm86rate8[] = {352800, 264600, 176400, 132300,
							88200,  66150,  44010,  33075};

/* 32,24,16,12, 8, 6, 4, 3 - 最少公倍数: 96 */
/*  3, 4, 6, 8,12,16,24,32 */

static const UINT clk25_128[] = {
					0x00001bde, 0x00002527, 0x000037bb, 0x00004a4e,
					0x00006f75, 0x0000949c, 0x0000df5f, 0x00012938};
static const UINT clk20_128[] = {
					0x000016a4, 0x00001e30, 0x00002d48, 0x00003c60,
					0x00005a8f, 0x000078bf, 0x0000b57d, 0x0000f17d};


	PCM86CFG	pcm86cfg;


void pcm86gen_initialize(UINT rate)
{
	pcm86cfg.rate = rate;
}

void pcm86gen_setvol(UINT vol)
{
	pcm86cfg.vol = vol;
	pcm86gen_update();
}

void pcm86_reset(void)
{
	PCM86 pcm86 = &g_pcm86;

	memset(pcm86, 0, sizeof(*pcm86));
	pcm86->nFifoIntrSize = 0x80;
	pcm86->cDacCtrl = 0x32;
	pcm86->nStepMask = (1 << 2) - 1;
	pcm86->cStepBits = 2;
	pcm86->nStepClock = (pccore.baseclock << 6);
	pcm86->nStepClock /= 44100;
	pcm86->nStepClock *= pccore.multiple;
	pcm86->nExtendBufferSize = (PCM86_RESCUE * 32) << 2;
	pcm86->cIrqLevel = 0xff;
}

void pcm86gen_update(void)
{
	PCM86 pcm86 = &g_pcm86;

	pcm86->volume = pcm86cfg.vol * pcm86->vol5;
	pcm86_setpcmrate(pcm86->cFifoCtrl);
}

void pcm86_setpcmrate(REG8 val)
{
	PCM86 pcm86 = &g_pcm86;
	SINT32	rate;

	rate = pcm86rate8[val & 7];
	pcm86->nStepClock = (pccore.baseclock << 6);
	pcm86->nStepClock /= rate;
	pcm86->nStepClock *= (pccore.multiple << 3);
	if (pcm86cfg.rate)
	{
		pcm86->div = (rate << (PCM86_DIVBIT - 3)) / pcm86cfg.rate;
		pcm86->div2 = (pcm86cfg.rate << (PCM86_DIVBIT + 3)) / rate;
	}
}

void pcm86_cb(NEVENTITEM item)
{
	PCM86 pcm86 = &g_pcm86;

	if (((pcm86->cFifoCtrl & 0xa0) == 0xa0) && (pcm86->cIrqFlag == 0))
	{
		sound_sync();
		TRACEOUT(("pcm intr? %d < %d", pcm86->nFifoRemain, pcm86->nFifoIntrSize));
		if (pcm86->nFifoRemain <= pcm86->nFifoIntrSize)
		{
			pcm86->cIrqFlag = 1;
			if (pcm86->cIrqLevel != 0xff)
			{
				pic_setirq(pcm86->cIrqLevel);
			}
		}
		else
		{
			pcm86_setnextintr();
		}
	}
	(void)item;
}

/**
 * 次の割り込みタイミングを計算する
 */
void pcm86_setnextintr(void)
{
	PCM86 pcm86 = &g_pcm86;
	SINT32 clk;
	SINT32 cnt;

	if (((pcm86->cFifoCtrl & 0xa0) == 0xa0) && (pcm86->cIrqFlag == 0))
	{
		if (pccore.cpumode & CPUMODE_8MHZ)
		{
			clk = clk20_128[pcm86->cFifoCtrl & 7];
		}
		else
		{
			clk = clk25_128[pcm86->cFifoCtrl & 7];
		}

		cnt = pcm86->nFifoRemain - pcm86->nFifoIntrSize;
		if (cnt > 0)
		{
			cnt += pcm86->nStepMask;
			cnt >>= pcm86->cStepBits;
			clk *= cnt;
		}
		else if (pcm86->nFifoRemain == 0)
		{
			clk *= (32768 >> pcm86->cStepBits);
		}
		clk >>= 7;
		clk *= pccore.multiple;
		nevent_set(NEVENT_86PCM, clk, pcm86_cb, NEVENT_ABSOLUTE);
	}
}

void SOUNDCALL pcm86gen_checkbuf(PCM86 pcm86)
{
	SINT nDiff;
	UINT32 nPast;

	nPast = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	nPast <<= 6;
	nPast -= pcm86->nLastClock;
	if (nPast >= pcm86->nStepClock)
	{
		nPast = nPast / pcm86->nStepClock;
		pcm86->nLastClock += (nPast * pcm86->nStepClock);
		RECALC_NOWCLKWAIT(pcm86, nPast);
	}

	nDiff = pcm86->nBufferCount - pcm86->nFifoRemain;
	if (nDiff < 0)									/* 処理落ちてる… */
	{
		nDiff &= ~3;
		pcm86->nFifoRemain += nDiff;
#if 0
		if (pcm86->nFifoRemain <= pcm86->nFifoIntrSize)
		{
			pcm86->cReqIrq = 0;
			pcm86->cIrqFlag = 1;
			if (pcm86->cIrqLevel != 0xff)
			{
				pic_setirq(pcm86->cIrqLevel);
			}
		}
		else
		{
			pcm86_setnextintr();
		}
#endif
	}
	else
	{
		nDiff -= pcm86->nExtendBufferSize;
		if (nDiff > 0)
		{
			nDiff &= ~3;
			pcm86->nBufferCount -= nDiff;
			pcm86->nReadPos += nDiff;
		}
	}
}

BOOL pcm86gen_intrq(void)
{
	PCM86 pcm86 = &g_pcm86;

	if (pcm86->cIrqFlag)
	{
		return TRUE;
	}
#if 0
	if (pcm86->cFifoCtrl & 0x20)
	{
		sound_sync();
		if ((pcm86->cReqIrq) && (pcm86->nFifoRemain <= pcm86->nFifoIntrSize))
		{
			pcm86->cReqIrq = 0;
			pcm86->cIrqFlag = 1;
			return TRUE;
		}
	}
#endif
	return FALSE;
}
