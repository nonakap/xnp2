
typedef struct {
	int		posx;
	int		posy;

	BYTE	NOWAIT;
	BYTE	DRAW_SKIP;
	BYTE	DISPCLK;
	BYTE	MOUSE_SW;

	BYTE	F11KEY;
	BYTE	F12KEY;

#if defined(SUPPORT_RESUME)
	BYTE	resume;
#endif
#if defined(SUPPORT_STATSAVE)
	BYTE	statsave;
#endif

	BYTE	jastsnd;
	BYTE	I286SAVE;
} NP2OSCFG;


enum {
	SCREEN_WBASE		= 80,
	SCREEN_HBASE		= 50,
	SCREEN_DEFMUL		= 8,
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};


extern	WindowPtr	hWndMain;
extern	BOOL		np2running;
extern	NP2OSCFG	np2oscfg;

