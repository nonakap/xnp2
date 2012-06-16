#include	"compiler.h"
#include	"np2.h"
#include	"oemtext.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"fddfile.h"

	UINT	sys_updates;


// ----

static	OEMCHAR	title[512];
static	OEMCHAR	clock[64];

static struct {
	UINT32	tick;
	UINT32	clock;
	UINT32	draws;
	SINT32	fps;
	SINT32	khz;
} workclock;

void sysmng_workclockreset(void) {

	workclock.tick = GETTICK();
	workclock.clock = CPU_CLOCK;
	workclock.draws = drawcount;
}

BOOL sysmng_workclockrenewal(void) {

	SINT32	tick;

	tick = GETTICK() - workclock.tick;
	if (tick < 2000) {
		return(FALSE);
	}
	workclock.tick += tick;
	workclock.fps = ((drawcount - workclock.draws) * 10000) / tick;
	workclock.draws = drawcount;
	workclock.khz = (CPU_CLOCK - workclock.clock) / tick;
	workclock.clock = CPU_CLOCK;
	return(TRUE);
}

void sysmng_updatecaption(UINT8 flag) {

	OEMCHAR	work[512];

	if (flag & 1) {
		title[0] = '\0';
		if (fdd_diskready(0)) {
			milstr_ncat(title, OEMTEXT("  FDD1:"), NELEMENTS(title));
			milstr_ncat(title, file_getname(fdd_diskname(0)),
															NELEMENTS(title));
		}
		if (fdd_diskready(1)) {
			milstr_ncat(title, OEMTEXT("  FDD2:"), NELEMENTS(title));
			milstr_ncat(title, file_getname(fdd_diskname(1)),
															NELEMENTS(title));
		}
	}
	if (flag & 2) {
		clock[0] = '\0';
		if (np2oscfg.DISPCLK & 2) {
			if (workclock.fps) {
				OEMSPRINTF(clock, OEMTEXT(" - %u.%1uFPS"),
									workclock.fps / 10, workclock.fps % 10);
			}
			else {
				milstr_ncpy(clock, OEMTEXT(" - 0FPS"), NELEMENTS(clock));
			}
		}
		if (np2oscfg.DISPCLK & 1) {
			OEMSPRINTF(work, OEMTEXT(" %2u.%03uMHz"),
								workclock.khz / 1000, workclock.khz % 1000);
			if (clock[0] == '\0') {
				milstr_ncpy(clock, OEMTEXT(" -"), NELEMENTS(clock));
			}
			milstr_ncat(clock, work, sizeof(clock));
#if 0
			OEMSPRINTF(work, OEMTEXT(" (debug: OPN %d / PSG %s)"),
							opngen.playing,
							(psg1.mixer & 0x3f)?OEMTEXT("ON"):OEMTEXT("OFF"));
			milstr_ncat(clock, work, NELEMENTS(clock));
#endif
		}
	}
	milstr_ncpy(work, np2oscfg.titles, NELEMENTS(work));
	milstr_ncat(work, title, NELEMENTS(work));
	milstr_ncat(work, clock, NELEMENTS(work));
#if defined(OSLANG_UTF8)
	TCHAR tchr[512];
	oemtotchar(tchr, NELEMENTS(tchr), work, -1);
	SetWindowText(g_hWndMain, tchr);
#else
	SetWindowText(g_hWndMain, work);
#endif
}

