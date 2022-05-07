
typedef struct {
	UINT8	NOWAIT;
	UINT8	DRAW_SKIP;
	UINT8	F12KEY;
	UINT8	resume;
	UINT8	jastsnd;
} NP2OSCFG;


#if defined(SIZE_QVGA)
enum {
	FULLSCREEN_WIDTH	= 320,
	FULLSCREEN_HEIGHT	= 240
};
#else
enum {
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 400
};
#endif

extern	NP2OSCFG	np2oscfg;

extern int np2_main(int argc, char *argv[]);
