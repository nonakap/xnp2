/**
 * @file	cmmidi.cpp
 * @brief	Communication port manager (MIDI)
 *
 * @author	$Author: yui $
 * @date	$Date: 2011/03/07 09:54:11 $
 */

#include "compiler.h"
#include "np2.h"
#include "mimpidef.h"
#include "commng.h"
#if defined(VERMOUTH_LIB) || defined(MT32SOUND_DLL)
#include "sound.h"
#endif
#if defined(VERMOUTH_LIB)
#include "vermouth.h"
#endif
#if defined(MT32SOUND_DLL)
#include "mt32snd.h"
#endif
#include "keydisp.h"

#if !defined(__GNUC__)
#pragma comment(lib, "winmm.lib")
#endif	// !defined(__GNUC__)

#if defined(VERMOUTH_LIB)
extern	MIDIMOD	vermouth_module;
#endif


#define MIDIOUTS(a, b, c)	(((c) << 16) + (b << 8) + (a))
#define MIDIOUTS2(a)		(*(UINT16 *)(a))
#define MIDIOUTS3(a)		((*(UINT32 *)(a)) & 0xffffff)


const OEMCHAR cmmidi_midimapper[] = OEMTEXT("MIDI MAPPER");
#if defined(VERMOUTH_LIB)
const OEMCHAR cmmidi_vermouth[] = OEMTEXT("VERMOUTH");
#endif
#if defined(MT32SOUND_DLL)
const OEMCHAR cmmidi_mt32sound[] = OEMTEXT("MT32Sound");
#endif

const OEMCHAR *cmmidi_mdlname[12] = {
		OEMTEXT("MT-32"),	OEMTEXT("CM-32L"),		OEMTEXT("CM-64"),
		OEMTEXT("CM-300"),	OEMTEXT("CM-500(LA)"),	OEMTEXT("CM-500(GS)"),
		OEMTEXT("SC-55"),	OEMTEXT("SC-88"),		OEMTEXT("LA"),
		OEMTEXT("GM"),		OEMTEXT("GS"),			OEMTEXT("XG")};

enum {		MIDI_MT32 = 0,	MIDI_CM32L,		MIDI_CM64,
			MIDI_CM300,		MIDI_CM500LA,	MIDI_CM500GS,
			MIDI_SC55,		MIDI_SC88,		MIDI_LA,
			MIDI_GM,		MIDI_GS,		MIDI_XG,	MIDI_OTHER};

static const UINT8 EXCV_MTRESET[] = {
			0xf0,0x41,0x10,0x16,0x12,0x7f,0x00,0x00,0x00,0x01,0xf7};
static const UINT8 EXCV_GMRESET[] = {
			0xf0,0x7e,0x7f,0x09,0x01,0xf7};
static const UINT8 EXCV_GM2RESET[] = {
			0xf0,0x7e,0x7f,0x09,0x03,0xf7};
static const UINT8 EXCV_GSRESET[] = {
			0xf0,0x41,0x10,0x42,0x12,0x40,0x00,0x7f,0x00,0x41,0xf7};
static const UINT8 EXCV_XGRESET[] = {
			0xf0,0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0xf7};

enum {
	MIDI_EXCLUSIVE		= 0xf0,
	MIDI_TIMECODE		= 0xf1,
	MIDI_SONGPOS		= 0xf2,
	MIDI_SONGSELECT		= 0xf3,
	MIDI_CABLESELECT	= 0xf5,
	MIDI_TUNEREQUEST	= 0xf6,
	MIDI_EOX			= 0xf7,
	MIDI_TIMING			= 0xf8,
	MIDI_START			= 0xfa,
	MIDI_CONTINUE		= 0xfb,
	MIDI_STOP			= 0xfc,
	MIDI_ACTIVESENSE	= 0xfe,
	MIDI_SYSTEMRESET	= 0xff
};

enum {
	MIDI_BUFFER			= (1 << 10),
	MIDIIN_MAX			= 4,

	CMMIDI_MIDIOUT		= 0x01,
	CMMIDI_MIDIIN		= 0x02,
	CMMIDI_MIDIINSTART	= 0x04,
#if defined(VERMOUTH_LIB)
	CMMIDI_VERMOUTH		= 0x08,
#endif
#if defined(MT32SOUND_DLL)
	CMMIDI_MT32SOUND	= 0x10,
#endif

	MIDICTRL_READY		= 0,
	MIDICTRL_2BYTES,
	MIDICTRL_3BYTES,
	MIDICTRL_EXCLUSIVE,
	MIDICTRL_TIMECODE,
	MIDICTRL_SYSTEM
};

struct _cmmidi;
typedef struct _cmmidi	_CMMIDI;
typedef struct _cmmidi	*CMMIDI;

typedef struct {
	UINT8	prog;
	UINT8	press;
	UINT16	bend;
	UINT8	ctrl[28];
} _MIDICH, *MIDICH;

typedef struct {
	HMIDIOUT	hmidiout;
	MIDIHDR		midihdr;
} OUTFNWIN32;

typedef union {
	OUTFNWIN32	win32;
#if defined(VERMOUTH_LIB)
	MIDIHDL		vermouth;
#endif
} HMIDIFNOUT;

struct _cmmidi {
	UINT		opened;
	void		(*shortout)(CMMIDI self, UINT32 msg);
	void		(*longout)(CMMIDI self, const UINT8 *msg, UINT leng);
	HMIDIFNOUT	out;

	HMIDIIN		hmidiin;
	MIDIHDR		hmidiinhdr;
	UINT		midictrl;
	UINT		midisyscnt;
	UINT		mpos;

	UINT8		midilast;
	UINT8		midiexcvwait;
	UINT8		midimodule;

	UINT8		buffer[MIDI_BUFFER];
	_MIDICH		mch[16];
	UINT8		excvbuf[MIDI_BUFFER];

	UINT8		def_en;
	MIMPIDEF	def;

	UINT		recvpos;
	UINT		recvsize;
	UINT8		recvbuf[MIDI_BUFFER];
	UINT8		midiinbuf[MIDI_BUFFER];
};

typedef struct {
	HMIDIIN		hmidiin;
	CMMIDI		midi;
} MIDIINHDL;

static	UINT		midiinhdls;
static	MIDIINHDL	midiinhdl[MIDIIN_MAX];

static const UINT8 midictrltbl[] = { 0, 1, 5, 7, 10, 11, 64,
									65, 66, 67, 84, 91, 93,
									94,						// for SC-88
									71, 72, 73, 74};		// for XG

static	UINT8	midictrlindex[128];


// ----

static BRESULT getmidioutid(const OEMCHAR *midiout, UINT *ret) {

	UINT		num;
	UINT		i;
	MIDIOUTCAPS	moc;

	num = midiOutGetNumDevs();
	for (i=0; i<num; i++) {
		if ((midiOutGetDevCaps(i, &moc, sizeof(moc)) == MMSYSERR_NOERROR) &&
			(!milstr_cmp(midiout, moc.szPname))) {
			*ret = i;
			return(SUCCESS);
		}
	}
	if (!milstr_cmp(midiout, cmmidi_midimapper)) {
		*ret = MIDI_MAPPER;
		return(SUCCESS);
	}
	return(FAILURE);
}

static BRESULT getmidiinid(const OEMCHAR *midiin, UINT *ret) {

	UINT		num;
	UINT		i;
	MIDIINCAPS	moc;

	num = midiInGetNumDevs();
	for (i=0; i<num; i++) {
		if ((midiInGetDevCaps(i, &moc, sizeof(moc)) == MMSYSERR_NOERROR) &&
			(!milstr_cmp(midiin, moc.szPname))) {
			*ret = i;
			return(SUCCESS);
		}
	}
	if (!milstr_cmp(midiin, cmmidi_midimapper)) {
		*ret = MIDI_MAPPER;
		return(SUCCESS);
	}
	return(FAILURE);
}

static UINT module2number(const OEMCHAR *module) {

	UINT	i;

	for (i=0; i<NELEMENTS(cmmidi_mdlname); i++) {
		if (!milstr_extendcmp(module, cmmidi_mdlname[i])) {
			break;
		}
	}
	return(i);
}


// ---- N/C

static void midi_ncshort(CMMIDI midi, UINT32 msg) {

	(void)midi;
	(void)msg;
}

static void midi_nclong(CMMIDI midi, const UINT8 *msg, UINT leng) {

	(void)midi;
	(void)msg;
	(void)leng;
}


// ---- Win32 API

static void waitlastexclusiveout(CMMIDI midi) {

	if (midi->midiexcvwait) {
		midi->midiexcvwait = 0;
		while(midiOutUnprepareHeader(midi->out.win32.hmidiout,
										&midi->out.win32.midihdr,
										sizeof(midi->out.win32.midihdr))
												== MIDIERR_STILLPLAYING) { }
	}
}

static void midi_win32short(CMMIDI midi, UINT32 msg) {

	waitlastexclusiveout(midi);
	midiOutShortMsg(midi->out.win32.hmidiout, (DWORD)msg);
}

static void midi_win32long(CMMIDI midi, const UINT8 *msg, UINT leng) {

	waitlastexclusiveout(midi);
	CopyMemory(midi->excvbuf, msg, leng);
	midi->out.win32.midihdr.lpData = (char *)midi->excvbuf;
	midi->out.win32.midihdr.dwFlags = 0;
	midi->out.win32.midihdr.dwBufferLength = leng;
	midiOutPrepareHeader(midi->out.win32.hmidiout, &midi->out.win32.midihdr,
											sizeof(midi->out.win32.midihdr));
	midiOutLongMsg(midi->out.win32.hmidiout, &midi->out.win32.midihdr,
											sizeof(midi->out.win32.midihdr));
	midi->midiexcvwait = 1;
}


// ---- Vermouth

#if defined(VERMOUTH_LIB)
static void midi_vermouthshort(CMMIDI midi, UINT32 msg) {

	sound_sync();
	midiout_shortmsg(midi->out.vermouth, msg);
}

static void midi_vermouthlong(CMMIDI midi, const UINT8 *msg, UINT leng) {

	sound_sync();
	midiout_longmsg(midi->out.vermouth, msg, leng);
}

static void SOUNDCALL vermouth_getpcm(MIDIHDL hdl, SINT32 *pcm, UINT count) {

const SINT32	*ptr;
	UINT		size;

	while(count) {
		size = count;
		ptr = midiout_get(hdl, &size);
		if (ptr == NULL) {
			break;
		}
		count -= size;
		do {
			pcm[0] += ptr[0];
			pcm[1] += ptr[1];
			ptr += 2;
			pcm += 2;
		} while(--size);
	}
}
#endif


// ---- MT-32

#if defined(MT32SOUND_DLL)
static void midi_mt32short(CMMIDI midi, UINT32 msg) {

	sound_sync();
	mt32sound_shortmsg(msg);
}

static void midi_mt32long(CMMIDI midi, const UINT8 *msg, UINT leng) {

	sound_sync();
	mt32sound_longmsg(msg, leng);
}

static void SOUNDCALL mt32_getpcm(MIDIHDL hdl, SINT32 *pcm, UINT count) {

	mt32sound_mix32(pcm, count);
}
#endif



// ----

static void midiallnoteoff(CMMIDI midi) {

	UINT	i;
	UINT8	msg[4];

	for (i=0; i<0x10; i++) {
		msg[0] = (UINT8)(0xb0 + i);
		msg[1] = 0x7b;
		msg[2] = 0x00;
		keydisp_midi(msg);
		midi->shortout(midi, MIDIOUTS3(msg));
	}
}

static void midireset(CMMIDI midi) {

const UINT8	*excv;
	UINT	excvsize;

	switch(midi->midimodule) {
		case MIDI_GM:
			excv = EXCV_GMRESET;
			excvsize = sizeof(EXCV_GMRESET);
			break;

		case MIDI_CM300:
		case MIDI_CM500GS:
		case MIDI_SC55:
		case MIDI_SC88:
		case MIDI_GS:
			excv = EXCV_GSRESET;
			excvsize = sizeof(EXCV_GSRESET);
			break;

		case MIDI_XG:
			excv = EXCV_XGRESET;
			excvsize = sizeof(EXCV_XGRESET);
			break;

		case MIDI_MT32:
		case MIDI_CM32L:
		case MIDI_CM64:
		case MIDI_CM500LA:
		case MIDI_LA:
			excv = EXCV_MTRESET;
			excvsize = sizeof(EXCV_MTRESET);
			break;

		default:
			excv = NULL;
			excvsize = 0;
			break;
	}
	if (excv) {
		midi->longout(midi, excv, excvsize);
	}
	midiallnoteoff(midi);
}

static void midisetparam(CMMIDI midi) {

	UINT8	i;
	UINT	j;
	MIDICH	mch;

	mch = midi->mch;
	for (i=0; i<16; i++, mch++) {
		if (mch->press != 0xff) {
			midi->shortout(midi, MIDIOUTS(0xa0+i, mch->press, 0));
		}
		if (mch->bend != 0xffff) {
			midi->shortout(midi, (mch->bend << 8) + 0xe0+i);
		}
		for (j=0; j<NELEMENTS(midictrltbl); j++) {
			if (mch->ctrl[j+1] != 0xff) {
				midi->shortout(midi,
							MIDIOUTS(0xb0+i, midictrltbl[j], mch->ctrl[j+1]));
			}
		}
		if (mch->prog != 0xff) {
			midi->shortout(midi, MIDIOUTS(0xc0+i, mch->prog, 0));
		}
	}
}


// ----

static UINT midiread(COMMNG self, UINT8 *data) {

	CMMIDI	midi;

	midi = (CMMIDI)(self + 1);
	if (midi->recvsize) {
		midi->recvsize--;
		*data = midi->recvbuf[midi->recvpos];
		midi->recvpos = (midi->recvpos + 1) & (MIDI_BUFFER - 1);
		return(1);
	}
	return(0);
}

static UINT midiwrite(COMMNG self, UINT8 data) {

	CMMIDI	midi;
	MIDICH	mch;
	int		type;

	midi = (CMMIDI)(self + 1);
	switch(data) {
		case MIDI_TIMING:
		case MIDI_START:
		case MIDI_CONTINUE:
		case MIDI_STOP:
		case MIDI_ACTIVESENSE:
		case MIDI_SYSTEMRESET:
			return(1);
	}
	if (midi->midictrl == MIDICTRL_READY) {
		if (data & 0x80) {
			midi->mpos = 0;
			switch(data & 0xf0) {
				case 0xc0:
				case 0xd0:
					midi->midictrl = MIDICTRL_2BYTES;
					break;

				case 0x80:
				case 0x90:
				case 0xa0:
				case 0xb0:
				case 0xe0:
					midi->midictrl = MIDICTRL_3BYTES;
					midi->midilast = data;
					break;

				default:
					switch(data) {
						case MIDI_EXCLUSIVE:
							midi->midictrl = MIDICTRL_EXCLUSIVE;
							break;

						case MIDI_TIMECODE:
							midi->midictrl = MIDICTRL_TIMECODE;
							break;

						case MIDI_SONGPOS:
							midi->midictrl = MIDICTRL_SYSTEM;
							midi->midisyscnt = 3;
							break;

						case MIDI_SONGSELECT:
							midi->midictrl = MIDICTRL_SYSTEM;
							midi->midisyscnt = 2;
							break;

						case MIDI_CABLESELECT:
							midi->midictrl = MIDICTRL_SYSTEM;
							midi->midisyscnt = 1;
							break;

//						case MIDI_TUNEREQUEST:
//						case MIDI_EOX:
						default:
							return(1);
					}
					break;
			}
		}
		else {						// Key-onのみな気がしたんだけど忘れた…
			// running status
			midi->buffer[0] = midi->midilast;
			midi->mpos = 1;
			midi->midictrl = MIDICTRL_3BYTES;
		}
	}
	midi->buffer[midi->mpos] = data;
	midi->mpos++;

	switch(midi->midictrl) {
		case MIDICTRL_2BYTES:
			if (midi->mpos >= 2) {
				midi->buffer[1] &= 0x7f;
				mch = midi->mch + (midi->buffer[0] & 0xf);
				switch(midi->buffer[0] & 0xf0) {
					case 0xa0:
						mch->press = midi->buffer[1];
						break;

					case 0xc0:
						if (midi->def_en) {
							type = midi->def.ch[midi->buffer[0] & 0x0f];
							if (type < MIMPI_RHYTHM) {
								midi->buffer[1] = 
										midi->def.map[type][midi->buffer[1]];
							}
						}
						mch->prog = midi->buffer[1];
						break;
				}
				keydisp_midi(midi->buffer);
				midi->shortout(midi, MIDIOUTS2(midi->buffer));
				midi->midictrl = MIDICTRL_READY;
				return(2);
			}
			break;

		case MIDICTRL_3BYTES:
			if (midi->mpos >= 3) {
				*(UINT16 *)(midi->buffer + 1) &= 0x7f7f;
				mch = midi->mch + (midi->buffer[0] & 0xf);
				switch(midi->buffer[0] & 0xf0) {
					case 0xb0:
						if (midi->buffer[1] == 123) {
							mch->press = 0;
							mch->bend = 0x4000;
							mch->ctrl[1+1] = 0;			// Modulation
							mch->ctrl[5+1] = 127;		// Explession
							mch->ctrl[6+1] = 0;			// Hold
							mch->ctrl[7+1] = 0;			// Portament
							mch->ctrl[8+1] = 0;			// Sostenute
							mch->ctrl[9+1] = 0;			// Soft
						}
						else {
							mch->ctrl[midictrlindex[midi->buffer[1]]]
															= midi->buffer[2];
						}
						break;

					case 0xe0:
						mch->bend = *(UINT16 *)(midi->buffer + 1);
						break;
				}
				keydisp_midi(midi->buffer);
				midi->shortout(midi, MIDIOUTS3(midi->buffer));
				midi->midictrl = MIDICTRL_READY;
				return(3);
			}
			break;

		case MIDICTRL_EXCLUSIVE:
			if (data == MIDI_EOX) {
				midi->longout(midi, midi->buffer, midi->mpos);
				midi->midictrl = MIDICTRL_READY;
				return(midi->mpos);
			}
			else if (midi->mpos >= MIDI_BUFFER) {		// おーばーふろー
				midi->midictrl = MIDICTRL_READY;
			}
			break;

		case MIDICTRL_TIMECODE:
			if (midi->mpos >= 2) {
				if ((data == 0x7e) || (data == 0x7f)) {
					// exclusiveと同じでいい筈…
					midi->midictrl = MIDICTRL_EXCLUSIVE;
				}
				else {
					midi->midictrl = MIDICTRL_READY;
					return(2);
				}
			}
			break;

		case MIDICTRL_SYSTEM:
			if (midi->mpos >= midi->midisyscnt) {
				midi->midictrl = MIDICTRL_READY;
				return(midi->midisyscnt);
			}
			break;
	}
	return(0);
}

static UINT8 midigetstat(COMMNG self) {

	return(0x00);
}

static long midimsg(COMMNG self, UINT msg, long param) {

	CMMIDI	midi;
	COMFLAG	flag;

	midi = (CMMIDI)(self + 1);
	switch(msg) {
		case COMMSG_MIDIRESET:
			midireset(midi);
			return(1);

		case COMMSG_SETFLAG:
			flag = (COMFLAG)param;
			if ((flag) &&
				(flag->size == sizeof(_COMFLAG) + sizeof(midi->mch)) &&
				(flag->sig == COMSIG_MIDI)) {
				CopyMemory(midi->mch, flag + 1, sizeof(midi->mch));
				midisetparam(midi);
				return(1);
			}
			break;

		case COMMSG_GETFLAG:
			flag = (COMFLAG)_MALLOC(sizeof(_COMFLAG) + sizeof(midi->mch),
																"MIDI FLAG");
			if (flag) {
				flag->size = sizeof(_COMFLAG) + sizeof(midi->mch);
				flag->sig = COMSIG_MIDI;
				flag->ver = 0;
				flag->param = 0;
				CopyMemory(flag + 1, midi->mch, sizeof(midi->mch));
				return((long)flag);
			}
			break;

		case COMMSG_MIMPIDEFFILE:
			mimpidef_load(&midi->def, (OEMCHAR *)param);
			return(1);

		case COMMSG_MIMPIDEFEN:
			midi->def_en = (param)?TRUE:FALSE;
			return(1);
	}
	return(0);
}

static BRESULT midiinhdlreg(CMMIDI midi, HMIDIIN hmidiin) {

	if (midiinhdls < MIDIIN_MAX) {
		midiinhdl[midiinhdls].hmidiin = hmidiin;
		midiinhdl[midiinhdls].midi = midi;
		midiinhdls++;
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}

static void midiinhdlunreg(CMMIDI midi) {

	UINT	i;

	for (i=0; i<midiinhdls; i++) {
		if (midiinhdl[i].midi == midi) {
			break;
		}
	}
	if (i < midiinhdls) {
		midiinhdls--;
		while(i < midiinhdls) {
			midiinhdl[i].hmidiin = midiinhdl[i+1].hmidiin;
			midiinhdl[i].midi = midiinhdl[i+1].midi;
		}
	}
}

static CMMIDI midiinhdlget(HMIDIIN hmidiin) {

	UINT	i;

	for (i=0; i<midiinhdls; i++) {
		if (midiinhdl[i].hmidiin == hmidiin) {
			return(midiinhdl[i].midi);
		}
	}
	return(NULL);
}

static void midirelease(COMMNG self) {

	CMMIDI	midi;

	midi = (CMMIDI)(self + 1);
	midiallnoteoff(midi);
	if (midi->opened & CMMIDI_MIDIOUT) {
		waitlastexclusiveout(midi);
		midiOutReset(midi->out.win32.hmidiout);
		midiOutClose(midi->out.win32.hmidiout);
	}
	if (midi->opened & CMMIDI_MIDIIN) {
		if (midi->opened & CMMIDI_MIDIINSTART) {
			midiInStop(midi->hmidiin);
			midiInUnprepareHeader(midi->hmidiin,
										&midi->hmidiinhdr, sizeof(MIDIHDR));
		}
		midiInReset(midi->hmidiin);
		midiInClose(midi->hmidiin);
		midiinhdlunreg(midi);
	}
#if defined(VERMOUTH_LIB)
	if (midi->opened & CMMIDI_VERMOUTH) {
		midiout_destroy(midi->out.vermouth);
	}
#endif
#if defined(MT32SOUND_DLL)
	if (midi->opened & CMMIDI_MT32SOUND) {
		mt32sound_close();
	}
#endif
	_MFREE(self);
}


// ----

void cmmidi_initailize(void) {

	UINT	i;

	ZeroMemory(midictrlindex, sizeof(midictrlindex));
	for (i=0; i<NELEMENTS(midictrltbl); i++) {
		midictrlindex[midictrltbl[i]] = (UINT8)(i + 1);
	}
	midictrlindex[32] = 1;
}

COMMNG cmmidi_create(const OEMCHAR *midiout, const OEMCHAR *midiin,
													const OEMCHAR *module) {

	UINT		opened;
	UINT		id;
	void		(*shortout)(CMMIDI self, UINT32 msg);
	void		(*longout)(CMMIDI self, const UINT8 *msg, UINT leng);
	HMIDIFNOUT	out;
	HMIDIIN		hmidiin = NULL;
	COMMNG		ret;
	CMMIDI		midi;

	opened = 0;
	ZeroMemory(&out, sizeof(out));
	shortout = midi_ncshort;
	longout = midi_nclong;
	if (getmidioutid(midiout, &id) == SUCCESS) {
		if (midiOutOpen(&out.win32.hmidiout, id, 0, 0, CALLBACK_NULL)
														== MMSYSERR_NOERROR) {
			midiOutReset(out.win32.hmidiout);
			shortout = midi_win32short;
			longout = midi_win32long;
			opened |= CMMIDI_MIDIOUT;
		}
	}
	if (getmidiinid(midiin, &id) == SUCCESS) {
		if (midiInOpen(&hmidiin, id, (DWORD)g_hWndMain, 0, CALLBACK_WINDOW)
														== MMSYSERR_NOERROR) {
			midiInReset(hmidiin);
			opened |= CMMIDI_MIDIIN;
		}
	}
#if defined(VERMOUTH_LIB)
	else if (!milstr_cmp(midiout, cmmidi_vermouth)) {
		out.vermouth = midiout_create(vermouth_module, 512);
		if (out.vermouth != NULL) {
			shortout = midi_vermouthshort;
			longout = midi_vermouthlong;
			opened |= CMMIDI_VERMOUTH;
		}
	}
#endif
#if defined(MT32SOUND_DLL)
	else if (!milstr_cmp(midiout, cmmidi_mt32sound)) {
		if (mt32sound_open() == SUCCESS) {
			shortout = midi_mt32short;
			longout = midi_mt32long;
			opened |= CMMIDI_MT32SOUND;
		}
	}
#endif
	if (!opened) {
		goto cmcre_err1;
	}
	ret = (COMMNG)_MALLOC(sizeof(_COMMNG) + sizeof(_CMMIDI), "MIDI");
	if (ret == NULL) {
		goto cmcre_err2;
	}
	ret->connect = COMCONNECT_MIDI;
	ret->read = midiread;
	ret->write = midiwrite;
	ret->getstat = midigetstat;
	ret->msg = midimsg;
	ret->release = midirelease;
	midi = (CMMIDI)(ret + 1);
	ZeroMemory(midi, sizeof(_CMMIDI));
	midi->opened = opened;
	midi->shortout = shortout;
	midi->longout = longout;
	midi->out = out;
	midi->midictrl = MIDICTRL_READY;
#if 1
	midi->hmidiin = hmidiin;
	if (opened & CMMIDI_MIDIIN) {
		if (midiinhdlreg(midi, hmidiin) == SUCCESS) {
			midi->opened |= CMMIDI_MIDIINSTART;
			midi->hmidiinhdr.lpData = (char *)midi->midiinbuf;
			midi->hmidiinhdr.dwBufferLength = MIDI_BUFFER;
			midiInPrepareHeader(hmidiin, &midi->hmidiinhdr, sizeof(MIDIHDR));
			midiInAddBuffer(hmidiin, &midi->hmidiinhdr, sizeof(MIDIHDR));
			midiInStart(hmidiin);
		}
	}
#endif
#if defined(VERMOUTH_LIB)
	if (opened & CMMIDI_VERMOUTH) {
		sound_streamregist((void *)out.vermouth, (SOUNDCB)vermouth_getpcm);
	}
#endif
#if defined(MT32SOUND_DLL)
	if (opened & CMMIDI_MT32SOUND) {
		sound_streamregist(NULL, (SOUNDCB)mt32_getpcm);
	}
#endif
//	midi->midisyscnt = 0;
//	midi->mpos = 0;

	midi->midilast = 0x80;
//	midi->midiexcvwait = 0;
	midi->midimodule = (UINT8)module2number(module);
	FillMemory(midi->mch, sizeof(midi->mch), 0xff);
	return(ret);

cmcre_err2:
	if (opened & CMMIDI_MIDIOUT) {
		midiOutReset(out.win32.hmidiout);
		midiOutClose(out.win32.hmidiout);
	}
#if defined(VERMOUTH_LIB)
	if (opened & CMMIDI_VERMOUTH) {
		midiout_destroy(out.vermouth);
	}
#endif
#if defined(MT32SOUND_DLL)
	if (opened & CMMIDI_MT32SOUND) {
		mt32sound_close();
	}
#endif

cmcre_err1:
	return(NULL);
}


// ---- midiin callback

static void midiinrecv(CMMIDI midi, const UINT8 *data, UINT size) {

	UINT	wpos;
	UINT	wsize;

	size = min(size, MIDI_BUFFER - midi->recvsize);
	if (size) {
		wpos = (midi->recvpos + midi->recvsize) & (MIDI_BUFFER - 1);
		midi->recvsize += size;
		wsize = min(size, MIDI_BUFFER - wpos);
		CopyMemory(midi->recvbuf + wpos, data, wsize);
		size -= wsize;
		if (size) {
			CopyMemory(midi->recvbuf, data + wsize, size);
		}
	}
}

void cmmidi_recvdata(HMIDIIN hdr, UINT32 data) {

	CMMIDI	midi;
	UINT	databytes;

	midi = midiinhdlget(hdr);
	if (midi) {
		databytes = 0;
		switch(data & 0xf0) {
			case 0xc0:
			case 0xd0:
				databytes = 2;
				break;

			case 0x80:
			case 0x90:
			case 0xa0:
			case 0xb0:
			case 0xe0:
				databytes = 3;
				break;
#if 0
			case 0xf0:
				switch(data & 0xff) {
					case MIDI_TIMING:
					case MIDI_START:
					case MIDI_CONTINUE:
					case MIDI_STOP:
					case MIDI_ACTIVESENSE:
					case MIDI_SYSTEMRESET:
						databytes = 1;
						break;
				}
				break;
#endif
		}
		midiinrecv(midi, (UINT8 *)&data, databytes);
	}
}

void cmmidi_recvexcv(HMIDIIN hdr, MIDIHDR *data) {

	CMMIDI	midi;

	midi = midiinhdlget(hdr);
	if (midi) {
		midiinrecv(midi, (UINT8 *)data->lpData, data->dwBytesRecorded);
		midiInUnprepareHeader(midi->hmidiin,
										&midi->hmidiinhdr, sizeof(MIDIHDR));
		midiInPrepareHeader(midi->hmidiin,
										&midi->hmidiinhdr, sizeof(MIDIHDR));
		midiInAddBuffer(midi->hmidiin, &midi->hmidiinhdr, sizeof(MIDIHDR));
	}
}

