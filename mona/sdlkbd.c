#include	"compiler.h"
#include	"np2.h"
#include	"sdlkbd.h"
#include	"keystat.h"


typedef struct {
	SINT16	sdlkey;
	UINT16	keycode;
} SDLKCNV;

#define		NC		0xff

// 101キーボード
static const SDLKCNV sdlcnv101[] = {
			{SDLK_ESCAPE,		0x00},	{SDLK_1,			0x01},
			{SDLK_2,			0x02},	{SDLK_3,			0x03},
			{SDLK_4,			0x04},	{SDLK_5,			0x05},
			{SDLK_6,			0x06},	{SDLK_7,			0x07},

			{SDLK_8,			0x08},	{SDLK_9,			0x09},
			{SDLK_0,			0x0a},	{SDLK_MINUS,		0x0b},
			{SDLK_EQUALS,		0x0c},	{SDLK_BACKSLASH,	0x0d},
			{SDLK_BACKSPACE,	0x0e},	{SDLK_TAB,			0x0f},

			{SDLK_q,			0x10},	{SDLK_w,			0x11},
			{SDLK_e,			0x12},	{SDLK_r,			0x13},
			{SDLK_t,			0x14},	{SDLK_y,			0x15},
			{SDLK_u,			0x16},	{SDLK_i,			0x17},

			{SDLK_o,			0x18},	{SDLK_p,			0x19},
			{SDLK_RETURN,		0x1c},	{SDLK_a,			0x1d},
			{SDLK_s,			0x1e},	{SDLK_d,			0x1f},

			{SDLK_f,			0x20},	{SDLK_g,			0x21},
			{SDLK_h,			0x22},	{SDLK_j,			0x23},
			{SDLK_k,			0x24},	{SDLK_l,			0x25},

										{SDLK_z,			0x29},
			{SDLK_x,			0x2a},	{SDLK_c,			0x2b},
			{SDLK_v,			0x2c},	{SDLK_b,			0x2d},
			{SDLK_n,			0x2e},	{SDLK_m,			0x2f},

			{SDLK_COMMA,		0x30},	{SDLK_PERIOD,		0x31},
			{SDLK_SLASH,		0x32},
			{SDLK_SPACE,		0x34},
			{SDLK_PAGEUP,		0x36},	{SDLK_PAGEDOWN,		0x37},

			{SDLK_INSERT,		0x38},	{SDLK_DELETE,		0x39},
			{SDLK_UP,			0x3a},	{SDLK_LEFT,			0x3b},
			{SDLK_RIGHT,		0x3c},	{SDLK_DOWN,			0x3d},
			{SDLK_HOME,			0x3e},	{SDLK_END,			0x3f},

			{SDLK_KP_MINUS,		0x40},	{SDLK_KP_DIVIDE,	0x41},
			{SDLK_KP7,			0x42},	{SDLK_KP8,			0x43},
			{SDLK_KP9,			0x44},	{SDLK_KP_MULTIPLY,	0x45},
			{SDLK_KP4,			0x46},	{SDLK_KP5,			0x47},

			{SDLK_KP6,			0x48},	{SDLK_KP_PLUS,		0x49},
			{SDLK_KP1,			0x4a},	{SDLK_KP2,			0x4b},
			{SDLK_KP3,			0x4c},
			{SDLK_KP0,			0x4e},

			{SDLK_KP_PERIOD,	0x50},

			{SDLK_BREAK,		0x60},	{SDLK_PRINT,		0x61},
			{SDLK_F1,			0x62},	{SDLK_F2,			0x63},
			{SDLK_F3,			0x64},	{SDLK_F4,			0x65},
			{SDLK_F5,			0x66},	{SDLK_F6,			0x67},

			{SDLK_F7,			0x68},	{SDLK_F8,			0x69},
			{SDLK_F9,			0x6a},	{SDLK_F10,			0x6b},

			{SDLK_RSHIFT,		0x70},	{SDLK_LSHIFT,		0x70},
			{SDLK_CAPSLOCK,		0x71},
			{SDLK_RALT,			0x73},	{SDLK_LALT,			0x73},
			{SDLK_RCTRL,		0x74},	{SDLK_LCTRL,		0x74}};

//			{SDLK_KP_EQUALS,	0x4d},


static	UINT8	keytbl[SDLK_LAST];

static const UINT8 f12keys[] = {
			0x61, 0x60, 0x4d, 0x4f};


void sdlkbd_initialize(void) {

	int			i;
const SDLKCNV	*key;
const SDLKCNV	*keyterm;

	for (i=0; i<SDLK_LAST; i++) {
		keytbl[i] = NC;
	}
	key = sdlcnv101;
	keyterm = key + NELEMENTS(sdlcnv101);
	while(key < keyterm) {
		keytbl[key->sdlkey] = (UINT8)key->keycode;
		key++;
	}
}

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

void sdlkbd_keydown(UINT key) {

	UINT8	data;

	if (key == SDLK_F12) {
		data = getf12key();
	}
	else if (key < SDLK_LAST) {
	 	data = keytbl[key];
	}
	else {
		data = NC;
	}
	if (data != NC) {
		keystat_senddata(data);
	}
}

void sdlkbd_keyup(UINT key) {

	UINT8	data;

	if (key == SDLK_F12) {
		data = getf12key();
	}
	else if (key < SDLK_LAST) {
	 	data = keytbl[key];
	}
	else {
		data = NC;
	}
	if (data != NC) {
		keystat_senddata((UINT8)(data | 0x80));
	}
}

void sdlkbd_resetf12(void) {

	UINT	i;

	for (i=0; i<NELEMENTS(f12keys); i++) {
		keystat_forcerelease(f12keys[i]);
	}
}








#if 0
	SDLK_NUMLOCK		= 300,
	SDLK_SCROLLOCK		= 302,
	SDLK_CLEAR			= 12,
	SDLK_PAUSE			= 19,
//	SDLK_EXCLAIM		= 33,
//	SDLK_QUOTEDBL		= 34,
//	SDLK_HASH			= 35,
//	SDLK_DOLLAR			= 36,
//	SDLK_AMPERSAND		= 38,
	SDLK_QUOTE			= 39,
//	SDLK_LEFTPAREN		= 40,
//	SDLK_RIGHTPAREN		= 41,
//	SDLK_ASTERISK		= 42,
//	SDLK_PLUS			= 43,
//	SDLK_COLON			= 58,
	SDLK_SEMICOLON		= 59,
//	SDLK_LESS			= 60,
//	SDLK_GREATER		= 62,
//	SDLK_QUESTION		= 63,
//	SDLK_AT				= 64,
	// Skip uppercase letters
	SDLK_LEFTBRACKET	= 91,
	SDLK_RIGHTBRACKET	= 93,
//	SDLK_CARET			= 94,
//	SDLK_UNDERSCORE		= 95,
	SDLK_BACKQUOTE		= 96,

	// Function keys
	SDLK_F11			= 292,
	SDLK_F12			= 293,

	// Key state modifier keys
//	SDLK_RMETA			= 309,
//	SDLK_LMETA			= 310,
	SDLK_LSUPER			= 311,
	SDLK_RSUPER			= 312,
//	SDLK_MODE			= 313,
//	SDLK_COMPOSE		= 314,

	// Miscellaneous function keys
	SDLK_HELP			= 315,
//	SDLK_SYSREQ			= 317,
	SDLK_MENU			= 319,
//	SDLK_POWER			= 320,
//	SDLK_EURO			= 321,
//	SDLK_UNDO			= 322,
#endif

