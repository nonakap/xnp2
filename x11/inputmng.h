#ifndef	NP2_X11_INPUTMNG_H__
#define	NP2_X11_INPUTMNG_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USE_SYSMENU)

enum {
	LBUTTON_BIT		= (1 << 0),
	RBUTTON_BIT		= (1 << 1),
	LBUTTON_DOWNBIT		= (1 << 2),
	RBUTTON_DOWNBIT		= (1 << 3),
	LBUTTON_UPBIT		= (1 << 4),
	RBUTTON_UPBIT		= (1 << 5),
	MOUSE_MOVEBIT		= (1 << 6),

	KEY_ENTER		= 0x01,
	KEY_MENU		= 0x02,
	KEY_SKIP		= 0x04,
	KEY_EXT			= 0x08,
	KEY_UP			= 0x10,
	KEY_DOWN		= 0x20,
	KEY_LEFT		= 0x40,
	KEY_RIGHT		= 0x80
};

#define	MAX_KEYBIND	32

typedef struct {
	short	key;
	UINT	bit;
} KEYBIND;

typedef struct {
	UINT	kbs;
	KEYBIND	kb[MAX_KEYBIND];
} INPMNG;

extern const KEYBIND keybind[];
extern const UINT nkeybind;

void inputmng_init(void);
void inputmng_keybind(short key, UINT bit);
UINT inputmng_getkey(short key);

#else

#define	inputmng_init()

#endif

#ifdef __cplusplus
}
#endif

#endif	/* NP2_X11_INPUTMNG_H__ */
