#include	"compiler.h"
#if defined(WIN32_PLATFORM_PSPC)
#include	<gx.h>
#endif
#include	"np2.h"
#include	"winkbd.h"
#include	"keystat.h"


#define		NC		0xff

#if defined(WIN32_PLATFORM_PSPC)
static UINT8 key106[256] =
#else
static const UINT8 key106[256] =
#endif
			//	    ,    ,    ,STOP,    ,    ,    ,    		; 0x00
		{		  NC,  NC,  NC,0x60,  NC,  NC,  NC,  NC,
			//	  BS, TAB,    ,    , CLR, ENT,    ,    		; 0x08
				0x0e,0x0f,  NC,  NC,  NC,0x1c,  NC,  NC,
			//	 SFT,CTRL, ALT,PAUS,CAPS,KANA,    ,    		; 0x10
				0x70,0x74,0x73,0x60,0x71,0x72,  NC,  NC,
			//	 FIN, KAN,    , ESC,XFER,NFER,    ,  MD		; 0x18
				  NC,  NC,  NC,0x00,0x35,0x51,  NC,  NC,
			//	 SPC,RLUP,RLDN, END,HOME,  Å©,  Å™,  Å®		; 0x20
				0x34,0x37,0x36,0x3f,0x3e,0x3b,0x3a,0x3c,
			//	  Å´, SEL, PNT, EXE,COPY, INS, DEL, HLP		; 0x28
				0x3d,  NC,  NC,  NC,  NC,0x38,0x39,  NC,
			//	  ÇO,  ÇP,  ÇQ,  ÇR,  ÇS,  ÇT,  ÇU,  ÇV		; 0x30
				0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
			//	  ÇW,  ÇX,    ,    ,    ,    ,    ,    		; 0x38
				0x08,0x09,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,  Ç`,  Ça,  Çb,  Çc,  Çd,  Çe,  Çf		; 0x40
				  NC,0x1d,0x2d,0x2b,0x1f,0x12,0x20,0x21,
			//	  Çg,  Çh,  Çi,  Çj,  Çk,  Çl,  Çm,  Çn		; 0x48
				0x22,0x17,0x23,0x24,0x25,0x2f,0x2e,0x18,
			//	  Ço,  Çp,  Çq,  Çr,  Çs,  Çt,  Çu,  Çv		; 0x50
				0x19,0x10,0x13,0x1e,0x14,0x16,0x2c,0x11,
			//	  Çw,  Çx,  Çy,LWIN,RWIN, APP,    ,    		; 0x58
				0x2a,0x15,0x29,  NC,  NC,  NC,  NC,  NC,
			//	<ÇO>,<ÇP>,<ÇQ>,<ÇR>,<ÇS>,<ÇT>,<ÇU>,<ÇV>		; 0x60
				0x4e,0x4a,0x4b,0x4c,0x46,0x47,0x48,0x42,
			//	<ÇW>,<ÇX>,<Åñ>,<Å{>,<ÅC>,<Å|>,<ÅD>,<Å^>		; 0x68
				0x43,0x44,0x45,0x49,  NC,0x40,0x50,0x41,
			//	 f.1, f.2, f.3, f.4, f.5, f.6, f.7, f.8		; 0x70
				0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
			//	 f.9, f10, f11, f12, f13, f14, f15, f16		; 0x78
				0x6a,0x6b,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0x80
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0x88
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	HELP, ALT,<ÅÅ>,    ,    ,    ,    ,    		; 0x90
				  NC,0x73,0x4d,  NC,  NC,  NC,  NC,  NC,			// ver0.28
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0x98
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa8
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xb0
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,  ÅF,  ÅG,  ÅC,  Å|,  ÅD,  Å^		; 0xb8
				  NC,  NC,0x27,0x26,0x30,0x0b,0x31,0x32,
			//	  Åó,    ,    ,    ,    ,    ,    ,    		; 0xc0
				0x1a,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xc8
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd0
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,  Åm,  Åè,  Ån,  ÅO,    		; 0xd8
				  NC,  NC,  NC,0x1b,0x0d,0x28,0x0c,  NC,
			//	    ,    ,  ÅQ,    ,    ,    ,    ,    		; 0xe0
				  NC,  NC,0x33,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xe8
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	CAPS,    ,KANA,    ,    ,    ,    ,    		; 0xf0
				0x71,  NC,0x72,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf8
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC};

#if defined(WIN32_PLATFORM_PSPC)
static UINT8 key106ext[256] =
#else
static const UINT8 key106ext[256] =
#endif
			//	    ,    ,    ,STOP,    ,    ,    ,    		; 0x00
		{		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	  BS, TAB,    ,    , CLR, ENT,    ,    		; 0x08
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	 SFT,CTRL, ALT,PAUS,CAPS,KANA,    ,    		; 0x10
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	 FIN, KAN,    , ESC,XFER,NFER,    ,  MD		; 0x18
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	 SPC,RLUP,RLDN, END,HOME,  Å©,  Å™,  Å®		; 0x20
				  NC,0x44,0x4c,0x4a,0x42,0x46,0x43,0x48,
			//	  Å´, SEL, PNT, EXE,COPY, INS, DEL, HLP		; 0x28
				0x4b,  NC,  NC,  NC,  NC,0x4e,0x50,  NC,
			//	  ÇO,  ÇP,  ÇQ,  ÇR,  ÇS,  ÇT,  ÇU,  ÇV		; 0x30
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	  ÇW,  ÇX,    ,    ,    ,    ,    ,    		; 0x38
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,  Ç`,  Ça,  Çb,  Çc,  Çd,  Çe,  Çf		; 0x40
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	  Çg,  Çh,  Çi,  Çj,  Çk,  Çl,  Çm,  Çn		; 0x48
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	  Ço,  Çp,  Çq,  Çr,  Çs,  Çt,  Çu,  Çv		; 0x50
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	  Çw,  Çx,  Çy,LWIN,RWIN, APP,    ,    		; 0x58
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	<ÇO>,<ÇP>,<ÇQ>,<ÇR>,<ÇS>,<ÇT>,<ÇU>,<ÇV>		; 0x60
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	<ÇW>,<ÇX>,<Åñ>,<Å{>,<ÅC>,<Å|>,<ÅD>,<Å^>		; 0x68
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	 f.1, f.2, f.3, f.4, f.5, f.6, f.7, f.8		; 0x70
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	 f.9, f10, f11, f12, f13, f14, f15, f16		; 0x78
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0x80
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0x88
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	HELP, ALT,<ÅÅ>,    ,    ,    ,    ,    		; 0x90
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0x98
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa8
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xb0
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,  ÅF,  ÅG,  ÅC,  Å|,  ÅD,  Å^		; 0xb8
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	  Åó,    ,    ,    ,    ,    ,    ,    		; 0xc0
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xc8
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd0
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,  Åm,  Åè,  Ån,  ÅO,    		; 0xd8
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,  ÅQ,    ,    ,    ,    ,    		; 0xe0
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xe8
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	CAPS,    ,KANA,    ,    ,    ,    ,    		; 0xf0
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf8
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC};

static const UINT8 f12keys[] = {
			0x61, 0x60, 0x4d, 0x4f};


static UINT8 getf12key(void) {

	UINT	key;

	key = np2oscfg.F12KEY - 1;
	if (key < NELEMENTS(f12keys)) {
		return(f12keys[key]);
	}
	else {
		return(NC);
	}
}

void winkbd_keydown(WPARAM wParam, LPARAM lParam) {

	UINT8	data;

	if (wParam != VK_F12) {
		data = key106[wParam & 0xff];
	}
	else {
		data = getf12key();
	}
	if (data != NC) {
		if ((!(lParam & 0x01000000)) &&
				(key106ext[wParam & 0xff] != NC)) {			// ver0.28
			keystat_senddata(0x70);							// PC/AT only!
			data = key106ext[wParam & 0xff];
		}
		keystat_senddata(data);
	}
	else {													// ver0.28
		if (wParam == 0x0c) {
			keystat_senddata(0x70);							// PC/AT only
			keystat_senddata(0x47);
		}
	}
}

void winkbd_keyup(WPARAM wParam, LPARAM lParam) {

	UINT8	data;

	if (wParam != VK_F12) {
		data = key106[wParam & 0xff];
	}
	else {
		data = getf12key();
	}
	if (data != NC) {
		if ((!(lParam & 0x01000000)) &&
				(key106ext[wParam & 0xff] != NC)) {		// ver0.28
			keystat_senddata(0x70 | 0x80);				// PC/AT only
			data = key106ext[wParam & 0xff];
		}
		keystat_senddata((UINT8)(data | 0x80));
	}
	else {												// ver0.28
		if (wParam == 0x0c) {
			keystat_senddata(0x70 | 0x80);				// PC/AT only
			keystat_senddata(0x47 | 0x80);
		}
	}
}

void winkbd_resetf12(void) {

	UINT	i;

	for (i=0; i<NELEMENTS(f12keys); i++) {
		keystat_forcerelease(f12keys[i]);
	}
}


// ---- PocketPC keys

#if defined(WIN32_PLATFORM_PSPC)

extern	GXKeyList	gx_keylist;

typedef struct {
	short	*ptr[4];
} KEYADRS;

typedef struct {
	UINT8	key[4];
} KEYSET;

typedef struct {
	KEYADRS	curadrs;
	KEYADRS	btnadrs;
	KEYSET	curset[2];
	KEYSET	btnset[2];
} PPCBTNTBL;

typedef struct {
	KEYSET	cur;
	KEYSET	btn;
} PPCBTNDEF;


static const PPCBTNTBL ppcbtntbl = {
			{&gx_keylist.vkUp, &gx_keylist.vkDown,
			 &gx_keylist.vkLeft, &gx_keylist.vkRight},

			{&gx_keylist.vkA, &gx_keylist.vkB,
			 &gx_keylist.vkC, &gx_keylist.vkStart},

			{{0x3a, 0x3d, 0x3b, 0x3c},			// cur
			 {0x43, 0x4b, 0x46, 0x48}},			// tenkey

			{{0x1c, 0x34,   NC,   NC},		// RET/SP
			 {0x29, 0x2a,   NC,   NC}}};	// ZX

static	PPCBTNDEF	ppcbtndef;

static void getbind(KEYSET *bind, const UINT8 *tbl, const KEYADRS *adrs) {

	int		i;
	int		key;

	for (i=0; i<4; i++) {
		key = (*adrs->ptr[i]) & 0xff;
		bind->key[i] = tbl[key];
	}
}

static void setbind(UINT8 *tbl, const KEYSET *bind, const KEYADRS *adrs) {

	int		i;
	int		key;

	for (i=0; i<4; i++) {
		key = (*adrs->ptr[i]) & 0xff;
		if (tbl[key] != NC) {
			keystat_forcerelease(tbl[key]);
		}
		tbl[key] = bind->key[i];
	}
}

void winkbd_bindinit(void) {

	getbind(&ppcbtndef.cur, key106ext, &ppcbtntbl.curadrs);
	getbind(&ppcbtndef.btn, key106, &ppcbtntbl.btnadrs);
}

void winkbd_bindcur(UINT type) {

const KEYSET	*bind;

	switch(type) {
		case 0:
		default:
			bind = &ppcbtndef.cur;
			break;

		case 1:
			bind = ppcbtntbl.curset + 0;
			break;

		case 2:
			bind = ppcbtntbl.curset + 1;
			break;
	}
	setbind(key106ext, bind, &ppcbtntbl.curadrs);
}

void winkbd_bindbtn(UINT type) {

const KEYSET	*bind;

	switch(type) {
		case 0:
		default:
			bind = &ppcbtndef.btn;
			break;

		case 1:
			bind = ppcbtntbl.btnset + 0;
			break;

		case 2:
			bind = ppcbtntbl.btnset + 1;
			break;
	}
	setbind(key106, bind, &ppcbtntbl.btnadrs);
}
#endif

