#ifndef	NP2_X11_NP2_H__
#define	NP2_X11_NP2_H__

#include <signal.h>

#include "joymng.h"

G_BEGIN_DECLS

typedef struct {
	BYTE	port;
	BYTE	def_en;
	BYTE	param;
	UINT32	speed;
	char	mout[MAX_PATH];
	char	min[MAX_PATH];
	char	mdl[64];
	char	def[MAX_PATH];
} COMCFG;

typedef struct {
	char	titles[256];

	UINT	paddingx;
	UINT	paddingy;

	BYTE	NOWAIT;
	BYTE	DRAW_SKIP;

	BYTE	DISPCLK;

	BYTE	KEYBOARD;
	BYTE	F12KEY;

	BYTE	MOUSE_SW;
	BYTE	JOYPAD1;
	BYTE	JOYPAD2;
	BYTE	JOY1BTN[JOY_NBUTTON];
	BYTE	JOYAXISMAP[2][JOY_NAXIS];
	BYTE	JOYBTNMAP[2][JOY_NBUTTON];
	char	JOYDEV[2][MAX_PATH];

	COMCFG	mpu;
	COMCFG	com[3];

	BYTE	confirm;

	BYTE	resume;						// ver0.30

	BYTE	statsave;
	BYTE	toolwin;
	BYTE	keydisp;
	BYTE	softkbd;
	BYTE	hostdrv_write;
	BYTE	jastsnd;
	BYTE	I286SAVE;

	BYTE	snddrv;
	char	MIDIDEV[2][MAX_PATH];
	UINT32	MIDIWAIT;

	BYTE	mouse_move_ratio;

	BYTE	disablemmx;
	BYTE	drawinterp;
	BYTE	F11KEY;

	BYTE	cfgreadonly;
} NP2OSCFG;


enum {
	SCREEN_WBASE		= 80,
	SCREEN_HBASE		= 50,
	SCREEN_DEFMUL		= 8,
	FULLSCREEN_WIDTH	= 640,
	FULLSCREEN_HEIGHT	= 480
};

enum {
	MMXFLAG_DISABLE		= 1,
	MMXFLAG_NOTSUPPORT	= 2
};

enum {
	INTERP_NEAREST		= 0,
	INTERP_TILES		= 1,
	INTERP_BILINEAR		= 2,
	INTERP_HYPER		= 3
};

/* np2.c */
extern volatile sig_atomic_t np2running;
extern NP2OSCFG np2oscfg;
extern BYTE scrnmode;
extern int ignore_fullscreen_mode;

extern UINT framecnt;
extern UINT waitcnt;
extern UINT framemax;

extern BOOL s98logging;
extern int s98log_count;

extern int verbose;

extern char hddfolder[MAX_PATH];
extern char fddfolder[MAX_PATH];
extern char bmpfilefolder[MAX_PATH];
extern char modulefile[MAX_PATH];
extern char statpath[MAX_PATH];
extern char fontname[1024];

extern const char np2flagext[];
extern const char np2resumeext[];

int flagload(const char* ext, const char* title, BOOL force);
int flagsave(const char* ext);
void flagdelete(const char* ext);

void changescreen(BYTE newmode);
void framereset(UINT cnt);
void processwait(UINT cnt);
int mainloop(void *);

extern int mmxflag;
int havemmx(void);

G_END_DECLS

#endif	/* NP2_X11_NP2_H__ */
