/**
 * @file	pcm86io.c
 * @brief	Implementation of the 86-PCM I/O
 */

#include "compiler.h"
#include "cpucore.h"
#include "pccore.h"
#include "iocore.h"
#include "pcm86io.h"
#include "sound/fmboard.h"
#include "sound/sound.h"

extern	PCM86CFG	pcm86cfg;

static const UINT8 pcm86bits[] = {1, 1, 1, 2, 0, 0, 0, 1};
static const SINT32 pcm86rescue[] = {PCM86_RESCUE * 32, PCM86_RESCUE * 24,
									 PCM86_RESCUE * 16, PCM86_RESCUE * 12,
									 PCM86_RESCUE *  8, PCM86_RESCUE *  6,
									 PCM86_RESCUE *  4, PCM86_RESCUE *  3};

static const UINT8 s_irqtable[8] = {0xff, 0xff, 0xff, 0xff, 0x03, 0x0a, 0x0d, 0x0c};

static void IOOUTCALL pcm86_oa460(UINT port, REG8 val)
{
//	TRACEOUT(("86pcm out %.4x %.2x", port, val));
	g_pcm86.cSoundFlags = (g_pcm86.cSoundFlags & 0xfe) | (val & 1);
	fmboard_extenable((REG8)(val & 1));
	(void)port;
}

static void IOOUTCALL pcm86_oa466(UINT port, REG8 val)
{
//	TRACEOUT(("86pcm out %.4x %.2x", port, val));
	if ((val & 0xe0) == 0xa0)
	{
		sound_sync();
		g_pcm86.vol5 = (~val) & 15;
		g_pcm86.volume = pcm86cfg.vol * g_pcm86.vol5;
	}
	(void)port;
}

static void IOOUTCALL pcm86_oa468(UINT port, REG8 val)
{
	REG8	xchgbit;

//	TRACEOUT(("86pcm out %.4x %.2x", port, val));
	sound_sync();
	xchgbit = g_pcm86.cFifoCtrl ^ val;

	/* バッファリセット */
	if ((xchgbit & 8) && (val & 8))
	{
		g_pcm86.nReadPos = 0;
		g_pcm86.nWritePos = 0;
		g_pcm86.nBufferCount = 0;
		g_pcm86.nFifoRemain = 0;
		g_pcm86.nLastClock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
		g_pcm86.nLastClock <<= 6;
	}

	/* 割り込みクリア */
	if ((xchgbit & 0x10) && (!(val & 0x10)))
	{
		g_pcm86.cIrqFlag = 0;
//		g_pcm86.cReqIrq = 0;
	}

	/* サンプリングレート変更 */
	if (xchgbit & 7)
	{
		g_pcm86.nExtendBufferSize = pcm86rescue[val & 7] << g_pcm86.cStepBits;
		pcm86_setpcmrate(val);
	}
	g_pcm86.cFifoCtrl = val;

	/* 再生フラグ */
	if ((xchgbit & 0x80) && (val & 0x80))
	{
		g_pcm86.nLastClock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
		g_pcm86.nLastClock <<= 6;
	}
	pcm86_setnextintr();
	(void)port;
}

static void IOOUTCALL pcm86_oa46a(UINT port, REG8 val)
{
//	TRACEOUT(("86pcm out %.4x %.2x", port, val));
	sound_sync();
	if (g_pcm86.cFifoCtrl & 0x20)
	{
		if (val != 0xff)
		{
			g_pcm86.nFifoIntrSize = (UINT16)((val + 1) << 7);
		}
		else
		{
			g_pcm86.nFifoIntrSize = 0x7ffc;
		}
	}
	else
	{
		g_pcm86.cDacCtrl = val;
		g_pcm86.cStepBits = pcm86bits[(val >> 4) & 7];
		g_pcm86.nStepMask = (1 << g_pcm86.cStepBits) - 1;
		g_pcm86.nExtendBufferSize = pcm86rescue[g_pcm86.cFifoCtrl & 7] << g_pcm86.cStepBits;
	}
	pcm86_setnextintr();
	(void)port;
}

static void IOOUTCALL pcm86_oa46c(UINT port, REG8 val)
{
//	TRACEOUT(("86pcm out %.4x %.2x", port, val));
	if (g_pcm86.nFifoRemain < PCM86_LOGICALBUF)
	{
		g_pcm86.nFifoRemain++;
	}
	g_pcm86.buffer[g_pcm86.nWritePos] = val;
	g_pcm86.nWritePos = (g_pcm86.nWritePos + 1) & PCM86_BUFMSK;
	g_pcm86.nBufferCount++;
	/* バッファオーバーフローの監視 */
	if (g_pcm86.nBufferCount >= (PCM86_LOGICALBUF + g_pcm86.nExtendBufferSize))
	{
		g_pcm86.nBufferCount -= 4;
		g_pcm86.nReadPos = (g_pcm86.nReadPos + 4) & PCM86_BUFMSK;
	}
	(void)port;
}

static REG8 IOINPCALL pcm86_ia460(UINT port)
{
	(void)port;
	return g_pcm86.cSoundFlags;
}

static REG8 IOINPCALL pcm86_ia466(UINT port)
{
	UINT32	nPast;
	UINT32	nStepClock;
	UINT32	cnt;
	REG8	ret;

	nPast = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	nPast <<= 6;
	nPast -= g_pcm86.nLastClock;
	nStepClock = g_pcm86.nStepClock;
	if (nPast >= nStepClock)
	{
		cnt = nPast / nStepClock;
		g_pcm86.nLastClock += (cnt * nStepClock);
		nPast -= cnt * nStepClock;
		if (g_pcm86.cFifoCtrl & 0x80)
		{
			sound_sync();
			RECALC_NOWCLKWAIT(&g_pcm86, cnt);
		}
	}
	ret = ((nPast << 1) >= nStepClock) ? 1 : 0;
	if (g_pcm86.nFifoRemain >= PCM86_LOGICALBUF)		/* バッファフル */
	{
		ret |= 0x80;
	}
	else if (!g_pcm86.nFifoRemain)						/* バッファ０ */
	{
		ret |= 0x40;
	}
	(void)port;
//	TRACEOUT(("86pcm in %.4x %.2x", port, ret));
	return ret;
}

static REG8 IOINPCALL pcm86_ia468(UINT port)
{
	REG8 ret;

	ret = g_pcm86.cFifoCtrl & (~0x10);
	if (pcm86gen_intrq())
	{
		ret |= 0x10;
	}
	(void)port;
//	TRACEOUT(("86pcm in %.4x %.2x", port, ret));
	return ret;
}

static REG8 IOINPCALL pcm86_ia46a(UINT port)
{
	(void)port;
//	TRACEOUT(("86pcm in %.4x %.2x", port, g_pcm86.cDacCtrl));
	return g_pcm86.cDacCtrl;
}

static REG8 IOINPCALL pcm86_ia46c(UINT port)
{
	REG8 ret = 0;

	if (g_pcm86.nFifoRemain)
	{
		g_pcm86.nFifoRemain--;
	}

	if (g_pcm86.nBufferCount > 0)
	{
		g_pcm86.nBufferCount--;
		ret = g_pcm86.buffer[g_pcm86.nReadPos & PCM86_BUFMSK];
		g_pcm86.nReadPos++;
	}
	return ret;
}

static REG8 IOINPCALL pcm86_inpdummy(UINT port)
{
	(void)port;
	return 0;
}


/* ---- */

/**
 * Reset
 * @param[in] cDipSw Dip switch
 */
void pcm86io_setopt(REG8 cDipSw)
{
	g_pcm86.cSoundFlags = ((~cDipSw) >> 1) & 0x70;
	g_pcm86.cIrqLevel = s_irqtable[(cDipSw >> 2) & 7];
}

void pcm86io_bind(void)
{
	sound_streamregist(&g_pcm86, (SOUNDCB)pcm86gen_getpcm);

	iocore_attachout(0xa460, pcm86_oa460);
	iocore_attachout(0xa466, pcm86_oa466);
	iocore_attachout(0xa468, pcm86_oa468);
	iocore_attachout(0xa46a, pcm86_oa46a);
	iocore_attachout(0xa46c, pcm86_oa46c);

	iocore_attachinp(0xa460, pcm86_ia460);
	iocore_attachinp(0xa462, pcm86_inpdummy);
	iocore_attachinp(0xa464, pcm86_inpdummy);
	iocore_attachinp(0xa466, pcm86_ia466);
	iocore_attachinp(0xa468, pcm86_ia468);
	iocore_attachinp(0xa46a, pcm86_ia46a);
	iocore_attachinp(0xa46c, pcm86_ia46c);
	iocore_attachinp(0xa46e, pcm86_inpdummy);
}
