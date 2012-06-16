#include	"compiler.h"
#include	<mmsystem.h>
#include	"parts.h"
#include	"np2.h"
#include	"soundmng.h"
#include	"sound.h"
#if defined(VERMOUTH_LIB)
#include	"commng.h"
#include	"cmver.h"
#endif


#if !defined(_WIN32_WCE)
#pragma comment(lib, "winmm.lib")
#endif

typedef struct {
	HWAVEOUT	hwave;
	UINT8		*buffer;
	DWORD		samples;
	DWORD		bufalign;
	WAVEHDR		wh[2];
} WAVEMNGT;

static	BOOL		waveopened = FALSE;
static	WAVEMNGT	w_ctrl;
static	BOOL		buffersent = FALSE;
static	DWORD		lastbuffer = 0;
static	DWORD		lasttick = 0;
static	DWORD		retry = 0;
static	UINT		currate = 22050;
static	UINT		curms = 500;
static	UINT		mute;

static const DWORD capsfmt[] = {
			WAVE_FORMAT_1S16, WAVE_FORMAT_2S16, WAVE_FORMAT_4S16};


#if defined(WAVEMNG_CBMAIN)

// wparam -> Handle...
// lparam -> WAVEHDR

extern HWND hWndMain;

void soundmng_cb(UINT msg, HWAVEOUT hwo, WAVEHDR *whd) {

	short		*dst;
const SINT32	*pcm;

	if ((msg == MM_WOM_DONE) && (whd)) {
		waveOutUnprepareHeader(hwo, whd, sizeof(WAVEHDR));
		if (whd->lpData) {
			dst = (short *)whd->lpData;
			pcm = NULL;
			if (!mute) {
				pcm = sound_pcmlock();
			}
			if (pcm) {
				satuation_s16(dst, pcm, w_ctrl.bufalign);
				sound_pcmunlock(pcm);
			}
			else {
				ZeroMemory(dst, w_ctrl.bufalign);
			}
			waveOutPrepareHeader(hwo, whd, sizeof(WAVEHDR));
			waveOutWrite(hwo, whd, sizeof(WAVEHDR));
			buffersent = TRUE;
		}
	}
}
#else
static void CALLBACK hwavecb(HWAVEOUT hwo, UINT uMsg,
						DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {

	short		*dst;
const SINT32	*pcm;

	if ((uMsg == WOM_DONE) && (dwParam1)) {
		waveOutUnprepareHeader(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
		if (((WAVEHDR *)dwParam1)->lpData) {
			dst = (short *)((WAVEHDR *)dwParam1)->lpData;
			pcm = NULL;
			if (!mute) {
				pcm = sound_pcmlock();
			}
			if (pcm) {
				satuation_s16(dst, pcm, w_ctrl.bufalign);
				sound_pcmunlock(pcm);
			}
			else {
				ZeroMemory(dst, w_ctrl.bufalign);
			}
			waveOutPrepareHeader(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
			waveOutWrite(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
			buffersent = TRUE;
		}
	}
}
#endif

UINT soundmng_create(UINT rate, UINT ms) {

	int				devs;
	int				num;
	int				i;
	WAVEOUTCAPS		woc;
	WAVEFORMATEX	wfex;
	DWORD			type;

	if (waveopened) {
		return(0);
	}

	mute = 1 << SNDPROC_NP2;

	switch(rate) {
		case 11025:
			type = 0;
			break;
		case 44100:
			type = 2;
			break;
		default:
			type = 1;
			break;
	}
	type = capsfmt[type];

	devs = waveOutGetNumDevs();
	for (num=0; num<devs; num++) {
		if (waveOutGetDevCaps(num, &woc, sizeof(woc)) == MMSYSERR_NOERROR) {
			if (woc.dwFormats & type) {
				break;
			}
		}
	}
	if (num < devs) {
		ZeroMemory(&w_ctrl, sizeof(w_ctrl));
		ZeroMemory(&wfex, sizeof(wfex));

		ms /= 2;
		if (ms < 50) {
			ms = 50;
		}
		else if (ms > 500) {
			ms = 500;
		}
		w_ctrl.samples = (ms * rate) / 1000;
		w_ctrl.bufalign = w_ctrl.samples * 2 * 2;
		w_ctrl.buffer = (UINT8 *)_MALLOC(w_ctrl.bufalign * 2, "wave mng");
		if (w_ctrl.buffer == NULL) {
			return(0);
		}
		ZeroMemory(w_ctrl.buffer, w_ctrl.bufalign * 2);
		for (i=0; i<2; i++) {
			w_ctrl.wh[i].lpData = (char *)w_ctrl.buffer +
													(w_ctrl.bufalign * i);
			w_ctrl.wh[i].dwBufferLength = w_ctrl.bufalign;
			w_ctrl.wh[i].dwUser = i;
		}
		wfex.wFormatTag = WAVE_FORMAT_PCM;
		wfex.nSamplesPerSec = rate;
		wfex.wBitsPerSample = 16;
		wfex.nChannels = 2;
		wfex.nBlockAlign = wfex.nChannels * (wfex.wBitsPerSample / 8);
		wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
#if defined(WAVEMNG_CBMAIN)
		if (waveOutOpen(&w_ctrl.hwave, num, &wfex, (DWORD)hWndMain, 0,
										CALLBACK_WINDOW) == MMSYSERR_NOERROR)
#else
		if (waveOutOpen(&w_ctrl.hwave, num, &wfex, (DWORD)hwavecb, 0,
									CALLBACK_FUNCTION) == MMSYSERR_NOERROR)
#endif
		{
			_HANDLE_ADD(w_ctrl.hwave, "WaveOut");
			for (i=0; i<2; i++) {
				waveOutPrepareHeader(w_ctrl.hwave, w_ctrl.wh + i,
														sizeof(WAVEHDR));
				waveOutWrite(w_ctrl.hwave, w_ctrl.wh + i, sizeof(WAVEHDR));
			}
#if defined(VERMOUTH_LIB)
			cmvermouth_load(rate);
#endif
			currate = rate;
			curms = ms;
			waveopened = TRUE;
			TRACEOUT(("soundmng success."));
			return(w_ctrl.samples);
		}
		_MFREE(w_ctrl.buffer);
		TRACEOUT(("soundmng failure."));
	}
	return(0);
}

void soundmng_destroy(void) {

#if defined(_WIN32_WCE)
	OSVERSIONINFO	osvi;
#endif
	BOOL			hpc4;
	int				i;
	int				retry = 10;

	if (waveopened) {
#if defined(_WIN32_WCE)
		ZeroMemory(&osvi, sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof(osvi);
		GetVersionEx(&osvi);
		hpc4 = (osvi.dwMajorVersion >= 4);
#else
		hpc4 = FALSE;
#endif
		if (!hpc4) {
			for (i=0; i<2; i++) {
				waveOutUnprepareHeader(w_ctrl.hwave, w_ctrl.wh + i,
															sizeof(WAVEHDR));
				w_ctrl.wh[i].lpData = NULL;
			}
			waveOutPause(w_ctrl.hwave);
			waveOutReset(w_ctrl.hwave);
		}
		else {
		//	誰かシグマリ３でのマトモな開放方法教えてくだちい…
		//	waveOutReset(w_ctrl.hwave);
			for (i=0; i<2; i++) {
				waveOutUnprepareHeader(w_ctrl.hwave, w_ctrl.wh + i,
															sizeof(WAVEHDR));
				w_ctrl.wh[i].lpData = NULL;
			}
		}
		do {
			if (waveOutClose(w_ctrl.hwave) == MMSYSERR_NOERROR) {
				_HANDLE_REM(w_ctrl.hwave);
				break;
			}
			Sleep(500);
		} while(--retry);
		_MFREE(w_ctrl.buffer);
#if defined(VERMOUTH_LIB)
//		cmvermouth_unload();			// 終了時に unload
#endif
		waveopened = FALSE;
	}
}


// ----

// WinCE版 … vermouthのロードに時間掛かるので
void soundmng_initialize(void) {
}

void soundmng_deinitialize(void) {

#if defined(VERMOUTH_LIB)
	cmvermouth_unload();
#endif
}

void soundmng_awake(void) {

	if (waveopened) {
		DWORD now = GetTickCount();
		if ((now - lasttick) >= 1000) {
			lasttick = now;
			if (buffersent) {
				buffersent = FALSE;
			}
			else if (++retry >= 3) {
				soundmng_destroy();
				soundmng_create(currate, curms);
				retry = 0;
			}
		}
	}
}

void soundmng_enable(UINT proc) {

	mute &= ~(1 << proc);
}

void soundmng_disable(UINT proc) {

	mute |= 1 << proc;
}

