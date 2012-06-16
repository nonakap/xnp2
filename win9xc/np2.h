
#ifndef __cplusplus
#error why called from C ?
#endif

typedef struct {
	int		winx;
	int		winy;
	UINT8	NOWAIT;
	UINT8	DRAW_SKIP;

	UINT8	KEYBOARD;
	UINT8	F12COPY;

	UINT8	MOUSE_SW;
	UINT8	JOYPAD1;
	UINT8	JOYPAD2;
	UINT8	JOY1BTN[4];

	UINT8	resume;
	UINT8	jastsnd;
	UINT8	I286SAVE;
} NP2OSCFG;


enum {
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};


enum {
	IDM_MEMORYDUMP		= 20000,

	IDM_FLAGSAVE		= 20100,
	IDM_FLAGLOAD		= 20150,

	WM_NP2CMD			= (WM_USER + 200)
};

enum {
	NP2CMD_EXIT			= 0,
	NP2CMD_RESET		= 1,
	NP2CMD_EXIT2		= 0x0100,
	NP2CMD_DUMMY		= 0xffff
};

// #define	STATSAVEMAX		10

extern	const TCHAR	szAppCaption[];
extern	NP2OSCFG	np2oscfg;
extern	HWND		hWndMain;
extern	HINSTANCE	hInst;
extern	HINSTANCE	hPrev;

extern	TCHAR		modulefile[MAX_PATH];
extern	TCHAR		fddfolder[MAX_PATH];
extern	TCHAR		hddfolder[MAX_PATH];
extern	TCHAR		bmpfilefolder[MAX_PATH];
extern	TCHAR		mimpideffile[MAX_PATH];

