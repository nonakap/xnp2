#include	"compiler.h"

#if defined(MT32SOUND_DLL)

#include	"mt32snd.h"

#define	MT32SAMPLES		512


typedef struct {
	HMODULE	mod;
	BOOL	opened;
	UINT	rate;
	int		(WINAPI * Open)(int rate, int reverb, int def, int revtype, int revtime, int revlvl);
	int		(WINAPI * Close)(void);
	int		(WINAPI * Write)(unsigned char data);
	int		(WINAPI * Mix)(void *buff, unsigned long size);
} MT32SOUND;

static	MT32SOUND	mt32sound;


typedef struct {
const char	*symbol;
	UINT	addr;
} DLLPROC;

static const TCHAR mt32sounddll[] = _T("mt32sound.dll");
static const char fn_mt32soundopen[] = "MT32Sound_Open";
static const char fn_mt32soundclose[] = "MT32Sound_Close";
static const char fn_mt32soundwrite[] = "MT32Sound_Write";
static const char fn_mt32soundmix[] = "MT32Sound_Mix";

static const DLLPROC dllproc[] = {
				{fn_mt32soundopen,	offsetof(MT32SOUND, Open)},
				{fn_mt32soundclose,	offsetof(MT32SOUND, Close)},
				{fn_mt32soundwrite,	offsetof(MT32SOUND, Write)},
				{fn_mt32soundmix,	offsetof(MT32SOUND, Mix)}};


BRESULT mt32sound_initialize(void) {

	HMODULE		mod;
const DLLPROC	*d;
const DLLPROC	*dterm;
	long		proc;

	mt32sound_deinitialize();
	mod = LoadLibrary(mt32sounddll);
	if (mod == NULL) {
		goto m3ini_err1;
	}
	mt32sound.mod = mod;
	d = dllproc;
	dterm = d + NELEMENTS(dllproc);
	while(d < dterm) {
		proc = (long)GetProcAddress(mod, d->symbol);
		if (proc == (long)NULL) {
			goto m3ini_err2;
		}
		*(long *)(((UINT8 *)&mt32sound) + (d->addr)) = proc;
		d++;
	}
	return(SUCCESS);

m3ini_err2:
	mt32sound_deinitialize();

m3ini_err1:
	return(FAILURE);
}

void mt32sound_deinitialize(void) {

	if (mt32sound.mod) {
		FreeLibrary(mt32sound.mod);
	}
	ZeroMemory(&mt32sound, sizeof(mt32sound));
}

BOOL mt32sound_isenable(void) {

	return(mt32sound.mod != NULL);
}

void mt32sound_setrate(UINT rate) {

	if (mt32sound.mod != NULL) {
		mt32sound.rate = rate;
	}
}


// ----

BRESULT mt32sound_open(void) {

	if ((mt32sound.mod != NULL) && (!mt32sound.opened) && (mt32sound.rate)) {
		mt32sound.Open(mt32sound.rate, 0, 0, 0, 0, 0);
		mt32sound.opened = TRUE;
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}

void mt32sound_close(void) {

	if (mt32sound.opened) {
		if (mt32sound.mod != NULL) {
			mt32sound.Close();
		}
		mt32sound.opened = FALSE;
	}
}

void mt32sound_shortmsg(UINT32 msg) {

	int		(WINAPI * Write)(unsigned char data);

	Write = mt32sound.Write;
	switch((msg >> 4) & (0xf0 >> 4)) {
		case 0xc0 >> 4:
		case 0xd0 >> 4:
			Write((UINT8)(msg >> 0));
			Write((UINT8)(msg >> 8));
			break;

		case 0x80 >> 4:
		case 0x90 >> 4:
		case 0xa0 >> 4:
		case 0xb0 >> 4:
		case 0xe0 >> 4:
			Write((UINT8)(msg >> 0));
			Write((UINT8)(msg >> 8));
			Write((UINT8)(msg >> 16));
			break;
	}
}

void mt32sound_longmsg(const UINT8 *ptr, UINT32 leng) {

	int		(WINAPI * Write)(unsigned char data);

	Write = mt32sound.Write;
	while(leng) {
		Write(*ptr);
		ptr++;
		leng--;
	}
}

UINT mt32sound_mix32(SINT32 *buff, UINT leng) {

	UINT	ret;
	UINT	len;
	SINT16	samp[512 * 2];
	UINT	i;

	ret = 0;
	while(leng) {
		len = min(leng, 512);
		mt32sound.Mix(samp, len);
		for (i=0; i<len; i++) {
			buff[i*2+0] += ((SINT32)samp[i*2+0]) * 2;
			buff[i*2+1] += ((SINT32)samp[i*2+1]) * 2;
		}
		buff += len * 2;
		leng -= len;
		ret += len;
	}
	return(ret);
}
#endif

