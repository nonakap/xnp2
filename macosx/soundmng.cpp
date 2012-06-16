#include	"compiler.h"
#ifndef NP2GCC
#include	<fp.h>
#endif
#include	"parts.h"
#include	"soundmng.h"
#include	"sound.h"
#if defined(VERMOUTH_LIB)
#include	"commng.h"
#include	"cmver.h"
#endif

#include	"soundrecording.h"
#include	"np2opening.h"

#define	SOUNDBUFFERS	2

typedef struct {
	SndChannelPtr	hdl;
	SndCallBackUPP	cb;
	UINT			rate;
	UINT			samples;
	UINT			buffersize;
#if !defined(SOUND_CRITICAL)
	SINT16			*indata;
	SINT16			*extendbuffer;
#endif
	ExtSoundHeader	*buf[SOUNDBUFFERS];
	SndCommand		cmd[SOUNDBUFFERS];
	SndCommand		cbcmd[SOUNDBUFFERS];
} _QSOUND, *QSOUND;

static	BOOL		QS_Avail = FALSE;
static	_QSOUND		QSound;
static	BOOL		QSound_Playing = FALSE;

static	void				(PARTSCALL *fnmix)(SINT16 *dst,
												const SINT32 *src, UINT size);

static pascal void QSoundCallback(SndChannelPtr /*inChannel*/,
												SndCommand *inCommand) {

	QSOUND		qs;
	int			nextbuf;
	void		*dst;
#if defined(SOUND_CRITICAL)
const SINT32	*src;
#endif

	if (QS_Avail) {
		qs = &QSound;
		nextbuf = inCommand->param1;
		dst = qs->buf[nextbuf]->sampleArea;
#if !defined(SOUND_CRITICAL)
		if (qs->indata) {
			CopyMemory((SINT16 *)dst, qs->indata, qs->buffersize);
			qs->indata = NULL;
		}
#else
		src = NULL;
		if (QSound_Playing) {
			src = sound_pcmlock();
		}
		if (src) {
			(*fnmix)((SINT16 *)dst, src, qs->buffersize);
            if (QSound_Playing) {
                recOPM((BYTE*)dst, qs->buffersize);
            }
			sound_pcmunlock(src);
		}
#endif
		else {
			ZeroMemory(dst, qs->buffersize);
		}
		SndDoCommand(qs->hdl, &qs->cmd[nextbuf], TRUE);
		SndDoCommand(qs->hdl, &qs->cbcmd[nextbuf], TRUE);
	}
}

static BOOL SoundChannel_Init(void) {

volatile QSOUND		qs;

	qs = &QSound;
	ZeroMemory(qs, sizeof(QSOUND));
#if TARGET_API_MAC_CARBON
	qs->cb = NewSndCallBackUPP(QSoundCallback);
#else
	qs->cb = NewSndCallBackProc(QSoundCallback);
#endif

	if (SndNewChannel(&qs->hdl, sampledSynth,
				initStereo + initNoInterp + initNoDrop, qs->cb) != noErr) {
		return(FAILURE);
	}
	return(SUCCESS);
}

static void SoundChannel_Term(void) {

volatile QSOUND		qs;
	SndChannelPtr	hdl;
	SndCallBackUPP	cb;

	qs = &QSound;
	hdl = qs->hdl;
	qs->hdl = NULL;
	cb = qs->cb;
	qs->cb = NULL;

#if TARGET_API_MAC_CARBON
	if (cb) {
		DisposeSndCallBackUPP(cb);
	}
#endif
	if (hdl) {
		SndDisposeChannel(hdl, TRUE);
	}
}

static BOOL SoundBuffer_Init(UINT rate, UINT samples) {

	QSOUND			qs;
	double			drate;
	extended80		extFreq;
	UINT			buffersize;
	int				i;
	ExtSoundHeader	*buf;

	qs = &QSound;
	qs->rate = rate;
	qs->samples = samples;
	buffersize = samples * 4;
	qs->buffersize = buffersize;
	drate = rate;
	dtox80(&drate, &extFreq);

#if !defined(SOUND_CRITICAL)
	qs->extendbuffer = (SINT16 *)_MALLOC(buffersize, "Extend buffer");
	if (qs->extendbuffer == NULL) {
		goto sbinit_err;
	}
#endif

	buffersize += sizeof(ExtSoundHeader);
	for (i=0; i<SOUNDBUFFERS; i++) {
		buf = (ExtSoundHeader *)_MALLOC(buffersize, "ExtSoundHeader");
		qs->buf[i] = buf;
		if (buf == NULL) {
			goto sbinit_err;
		}
		ZeroMemory(buf, buffersize);
		buf->numChannels = 2;
		buf->sampleRate = (UInt32)rate << 16;
		buf->encode = extSH;
		buf->numFrames = samples;
		buf->AIFFSampleRate = extFreq;
		buf->sampleSize = 16;

		qs->cmd[i].cmd = bufferCmd;
		qs->cmd[i].param2 = (SInt32)buf;
		qs->cbcmd[i].cmd = callBackCmd;
		qs->cbcmd[i].param1 = (i + 1) % SOUNDBUFFERS;
	}

	QS_Avail = TRUE;
	SndDoCommand(qs->hdl, &qs->cmd[0], TRUE);
	SndDoCommand(qs->hdl, &qs->cbcmd[0], TRUE);
	return(SUCCESS);

sbinit_err:
	return(FAILURE);
}

static void SoundBuffer_Term(void) {

	QSOUND			qs;
	ExtSoundHeader	**buf;
	int				i;

	qs = &QSound;
	buf = qs->buf;
	for (i=0; i<SOUNDBUFFERS; i++) {
		if (buf[i]) {
			_MFREE(buf[i]);
			buf[i] = NULL;
		}
	}
#if !defined(SOUND_CRITICAL)
	qs->indata = NULL;
	if (qs->extendbuffer) {
		_MFREE(qs->extendbuffer);
		qs->extendbuffer = NULL;
	}
#endif
}

UINT soundmng_create(UINT rate, UINT ms) {

	UINT	samples;

	QSound_Playing = FALSE;

	if (rate < 11025) {
		rate = 11025;
	}
	else if (rate > 44100) {
		rate = 44100;
	}

	if (SoundChannel_Init()) {
		goto qsinit_err;
	}
#if !defined(SOUND_CRITICAL)
	samples = rate * ms / (SOUNDBUFFERS * 1000);
	samples = (samples + 3) & (~3);
#else
	samples = rate * ms / 1000;
	samples = (samples + 255) & (~255);
#endif
	if (SoundBuffer_Init(rate, samples)) {
		goto qsinit_err;
	}
#if defined(VERMOUTH_LIB)
	cmvermouth_load(rate);
#endif
	return(samples);

qsinit_err:
	soundmng_destroy();
	return(0);
}

void soundmng_destroy(void) {

	if (QS_Avail) {
		QS_Avail = FALSE;
		SoundBuffer_Term();
		SoundChannel_Term();
#if defined(VERMOUTH_LIB)
		cmvermouth_unload();
#endif
	}
}

void soundmng_play(void) {

    if (!QSound_Playing) QSound_Playing = TRUE;
}

void soundmng_stop(void) {

	if (QSound_Playing) QSound_Playing = FALSE;
}

void soundmng_setreverse(BOOL reverse) {

	if (!reverse) {
        fnmix = satuation_s16;
	}
	else {
		fnmix = satuation_s16x;
	}
}

#if !defined(SOUND_CRITICAL)
void soundmng_sync(void) {

	QSOUND		qs;
const SINT32	*src;

	qs = &QSound;

	if ((QSound_Playing) && (qs->indata == NULL)) {
		src = sound_pcmlock();
		if (src) {
			(*fnmix)(qs->extendbuffer, src, qs->buffersize);
			recOPM((BYTE *)qs->extendbuffer, qs->buffersize);
			sound_pcmunlock(src);
			qs->indata = qs->extendbuffer;
		}
	}
}
#endif

#ifdef SUPPORT_HWSEEKSND
// --------------------------------------------------------------------------
#include <QuickTime/Movies.h>
#include "np2.h"
#include "dosio.h"

static	Movie	seekWAV[2];

static	Movie	setupWAV(const char* name) {
    FSSpec	fs;
    short	movieRefNum;
    short	resID = 0;
    Movie	wav = NULL;    

    if (!getResourceFile(name, &fs)) {
        char	path[MAX_PATH];
        Str255	fname;

        file_cpyname(path, file_getcd(name), MAX_PATH);
        mkstr255(fname, path);
        FSMakeFSSpec(0, 0, fname, &fs);
    }

    if (OpenMovieFile( &fs, &movieRefNum, fsRdPerm ) == noErr) {
        if (NewMovieFromFile(&wav,movieRefNum, &resID, NULL, newMovieActive, NULL) != noErr) {
            return NULL;
        }
    }
    return  wav;
}

void soundmng_deinitialize(void) {
    StopMovie(seekWAV[0]);
    StopMovie(seekWAV[1]);
    seekWAV[0]  = NULL;
    seekWAV[1] = NULL;
    ExitMovies();
}

BOOL soundmng_initialize(void) {
    EnterMovies();
    seekWAV[0] = setupWAV("fddseek.wav");
    seekWAV[1] = setupWAV("fddseek1.wav");
    if (seekWAV[0] == NULL || seekWAV[1] == NULL) {
        return  false;
    }
    return(true);
}

void soundmng_pcmvolume(UINT num, int volume) {
    if (seekWAV[num]) {
        SetMovieVolume(seekWAV[num], kFullVolume*volume/100);
    }
}

BOOL soundmng_pcmplay(UINT num, BOOL loop) {
    if (seekWAV[num]) {
        GoToBeginningOfMovie(seekWAV[num]);
        StartMovie(seekWAV[num]);
        return SUCCESS;
    }
	return(FAILURE);
}

void soundmng_pcmstop(UINT num) {
    if (seekWAV[num]) {
        StopMovie(seekWAV[num]);
    }
}

#endif
