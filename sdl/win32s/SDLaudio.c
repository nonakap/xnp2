#include	"compiler.h"


#if !defined(__GNUC__) && !defined(_WIN32_WCE)
#pragma comment(lib, "winmm.lib")
#endif

typedef struct {
	BOOL		opened;
	HWAVEOUT	hwave;
	DWORD		slice;
	DWORD		samples;
	DWORD		bufsize;
	void		(*usercb)(void *userdata, BYTE *stream, int len);
	void		*userdata;
	WAVEHDR		*wh;
	int			pause_on;
} WCTRL;

static	WCTRL			w_ctrl;
// static	SDL_AudioSpec	sdlaudio;

extern	HWND	__sdl_hWnd;
extern	void	__sdl_seterror(const char *error);

static const DWORD capsfmt[] = {
			WAVE_FORMAT_1S16, WAVE_FORMAT_1M16,
			WAVE_FORMAT_2S16, WAVE_FORMAT_2M16,
			WAVE_FORMAT_4S16, WAVE_FORMAT_4M16};

UINT soundmix_getpcm(SINT16 *pcm, UINT samples);

#if 1
void __sdl_audio_cb(UINT msg, HWAVEOUT hwo, WAVEHDR *whd) {

	if ((msg == MM_WOM_DONE) && (whd)) {
		waveOutUnprepareHeader(hwo, whd, sizeof(WAVEHDR));
		if (whd->lpData) {
			if (!w_ctrl.pause_on) {
				w_ctrl.usercb(w_ctrl.userdata,
										(BYTE *)whd->lpData, w_ctrl.bufsize);
			}
			else {
				ZeroMemory((BYTE *)whd->lpData, w_ctrl.bufsize);
			}
			waveOutPrepareHeader(hwo, whd, sizeof(WAVEHDR));
			waveOutWrite(hwo, whd, sizeof(WAVEHDR));
		}
	}
}
#else
static void CALLBACK hwavecb(HWAVEOUT hwo, UINT uMsg,
						DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {

	if ((uMsg == WOM_DONE) && (dwParam1)) {
		waveOutUnprepareHeader(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
		if (((WAVEHDR *)dwParam1)->lpData) {
			if (!w_ctrl.pause_on) {
				w_ctrl.usercb(w_ctrl.userdata,
										(BYTE *)whd->lpData, w_ctrl.bufsize);
			}
			else {
				ZeroMemory((BYTE *)whd->lpData, w_ctrl.bufsize);
			}
			waveOutPrepareHeader(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
			waveOutWrite(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
		}
	}
}
#endif

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {

	char			*errorstr;
	DWORD			type;
	int				devs;
	int				num;
	DWORD			i;
	WAVEOUTCAPS		woc;
	WAVEFORMATEX	wfex;
	DWORD			slice;
	DWORD			bufsize;
	DWORD			allocsize;
	WAVEHDR			*wh;
	char			*buf;
	HWAVEOUT		hwave;

	errorstr = NULL;
	if (w_ctrl.opened) {
		errorstr = "Audio device is already opened";
		goto soa_err1;
	}
	if (desired == NULL) {
		errorstr = "SDL_OpenAudio() disired = (NULL)";
		goto soa_err1;
	}
	if (desired->callback == NULL) {
		errorstr = "SDL_OpenAudio() passed a NULL callback";
		goto soa_err1;
	}
	i = desired->samples;
	if (i) {
		while(!(i & 1)) {
			i >>= 1;
		}
	}
	if (i != 1) {
		errorstr = "buffer size must be power of 2";
		goto soa_err1;
	}

	switch(desired->freq) {
		case 11025:
			type = 0;
			break;

		case 22050:
			type = 2;
			break;

		case 44100:
			type = 4;
			break;

		default:
			errorstr = "unsupport out sampling rate";
			goto soa_err1;
	}
	switch(desired->channels) {
		case 1:
			type++;
			break;

		case 2:
			break;

		default:
			errorstr = "1 (mono) and 2 (stereo) channels supported";
			goto soa_err1;
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
	if ((num >= devs) && (type != WAVE_FORMAT_2S16)) {
		for (num=0; num<devs; num++) {
			if (waveOutGetDevCaps(num, &woc, sizeof(woc))
												== MMSYSERR_NOERROR) {
				if (woc.dwFormats & WAVE_FORMAT_2S16) {
					break;
				}
			}
		}
	}
	if (num >= devs) {
		errorstr = "unsupport sound device";
		goto soa_err1;
	}

	slice = 2;
	bufsize = desired->samples * 2 * desired->channels;
	allocsize = (sizeof(WAVEHDR) + bufsize) * slice;
	wh = (WAVEHDR *)_MALLOC(allocsize, "waveout");
	if (wh == NULL) {
		errorstr = "memory allocate error";
		goto soa_err1;
	}
	ZeroMemory(wh, allocsize);
	buf = (char *)(wh + slice);
	for (i=0; i<slice; i++) {
		wh[i].lpData = buf;
		wh[i].dwBufferLength = bufsize;
		wh[i].dwUser = i;
		buf += bufsize;
	}

	w_ctrl.slice = slice;
	w_ctrl.samples = desired->samples;
	w_ctrl.bufsize = bufsize;
	w_ctrl.usercb = desired->callback;
	w_ctrl.userdata = desired->userdata;
	w_ctrl.wh = wh;

	ZeroMemory(&wfex, sizeof(wfex));
	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nSamplesPerSec = desired->freq;
	wfex.wBitsPerSample = 16;
	wfex.nChannels = desired->channels;
	wfex.nBlockAlign = wfex.nChannels * (wfex.wBitsPerSample / 8);
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
#if 1
	if (waveOutOpen(&hwave, num, &wfex, (DWORD)__sdl_hWnd, 0,
									CALLBACK_WINDOW) != MMSYSERR_NOERROR)
#else
	if (waveOutOpen(&hwave, num, &wfex, (DWORD)hwavecb, 0,
									CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
#endif
	{
		errorstr = "waveOutOpen error";
		goto soa_err2;
	}
	w_ctrl.opened = TRUE;
	w_ctrl.pause_on = 0;
	w_ctrl.hwave = hwave;
	for (i=0; i<slice; i++) {
		waveOutPrepareHeader(hwave, wh + i, sizeof(WAVEHDR));
		waveOutWrite(hwave, wh + i, sizeof(WAVEHDR));
	}
	return(0);

soa_err2:
	_MFREE(wh);

soa_err1:
	if (errorstr) {
		__sdl_seterror(errorstr);
	}
	return(-1);
}

void SDL_CloseAudio(void) {

	DWORD	i;
	int		retry = 10;

	if (w_ctrl.opened) {
		for (i=0; i<w_ctrl.slice; i++) {
			waveOutUnprepareHeader(w_ctrl.hwave, w_ctrl.wh + i,
															sizeof(WAVEHDR));
			w_ctrl.wh[i].lpData = NULL;
		}
		waveOutPause(w_ctrl.hwave);
		waveOutReset(w_ctrl.hwave);
		do {
			if (waveOutClose(w_ctrl.hwave) == MMSYSERR_NOERROR) {
				_HANDLE_REM(w_ctrl.hwave);
				break;
			}
			Sleep(500);
		} while(--retry);
		_MFREE(w_ctrl.wh);
		w_ctrl.opened = FALSE;
	}
}

void SDL_PauseAudio(int pause_on) {

	w_ctrl.pause_on = pause_on;
}

void SDL_MixAudio(BYTE *dst, const BYTE *src, DWORD len, int volume) {

	CopyMemory(dst, src, len);
}

