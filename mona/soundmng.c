#include	"compiler.h"
#include	"parts.h"
#include	"soundmng.h"
#include	"sound.h"

#if !defined(DISABLE_SOUND)

#define	NSNDBUF				2

typedef struct {
	BOOL	opened;
	int		nsndbuf;
	int		samples;
	SINT16	*buf[NSNDBUF];
} SOUNDMNG;

static	SOUNDMNG	soundmng;


static void sound_play_cb(void *userdata, BYTE *stream, int len) {

	int			length;
	SINT16		*dst;
const SINT32	*src;

	length = min(len, (int)(soundmng.samples * 2 * sizeof(SINT16)));
	dst = soundmng.buf[soundmng.nsndbuf];
	src = sound_pcmlock();
	if (src) {
		satuation_s16(dst, src, length);
		sound_pcmunlock(src);
	}
	else {
		ZeroMemory(dst, length);
	}
	SDL_MixAudio(stream, (BYTE *)dst, length, SDL_MIX_MAXVOLUME);
	soundmng.nsndbuf = (soundmng.nsndbuf + 1) % NSNDBUF;
	(void)userdata;
}

UINT soundmng_create(UINT rate, UINT ms) {

	SDL_AudioSpec	fmt;
	UINT			s;
	UINT			samples;
	SINT16			*tmp;

	if (soundmng.opened) {
		goto smcre_err1;
	}
	if (SDL_InitSubSystem(SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "Error: SDL_Init: %s\n", SDL_GetError());
		goto smcre_err1;
	}

	s = rate * ms / (NSNDBUF * 1000);
	samples = 1;
	while(s > samples) {
		samples <<= 1;
	}
	soundmng.nsndbuf = 0;
	soundmng.samples = samples;
	for (s=0; s<NSNDBUF; s++) {
		tmp = (SINT16 *)_MALLOC(samples * 2 * sizeof(SINT16), "buf");
		if (tmp == NULL) {
			goto smcre_err2;
		}
		soundmng.buf[s] = tmp;
		ZeroMemory(tmp, samples * 2 * sizeof(SINT16));
	}

	ZeroMemory(&fmt, sizeof(fmt));
	fmt.freq = rate;
	fmt.format = AUDIO_S16SYS;
	fmt.channels = 2;
	fmt.samples = samples;
	fmt.callback = sound_play_cb;
	if (SDL_OpenAudio(&fmt, NULL) < 0) {
		fprintf(stderr, "Error: SDL_OpenAudio: %s\n", SDL_GetError());
		return(FAILURE);
	}
	soundmng.opened = TRUE;
	return(samples);

smcre_err2:
	for (s=0; s<NSNDBUF; s++) {
		tmp = soundmng.buf[s];
		soundmng.buf[s] = NULL;
		if (tmp) {
			_MFREE(tmp);
		}
	}

smcre_err1:
	return(0);
}

void soundmng_destroy(void) {

	int		i;
	SINT16	*tmp;

	if (soundmng.opened) {
		soundmng.opened = FALSE;
		SDL_PauseAudio(1);
		SDL_CloseAudio();
		for (i=0; i<NSNDBUF; i++) {
			tmp = soundmng.buf[i];
			soundmng.buf[i] = NULL;
			_MFREE(tmp);
		}
	}
}

void soundmng_play(void) {

	if (soundmng.opened) {
		SDL_PauseAudio(0);
	}
}

void soundmng_stop(void) {

	if (soundmng.opened) {
		SDL_PauseAudio(1);
	}
}

#endif

