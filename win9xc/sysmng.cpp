#include	"compiler.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"cpucore.h"
#include	"pccore.h"


	UINT	sys_updates;


// ----

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

static BOOL workclockrenewal(void) {

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

void sysmng_updatecaption(void) {

	TCHAR	title[256];
	TCHAR	work[32];

	if (workclockrenewal()) {
		milstr_ncpy(title, szAppCaption, NELEMENTS(title));
		wsprintf(work, _T(" - %u.%1uFPS"),
									workclock.fps / 10, workclock.fps % 10);
		milstr_ncat(title, work, sizeof(title));
		wsprintf(work, _T(" %2u.%03uMHz"),
								workclock.khz / 1000, workclock.khz % 1000);
		milstr_ncat(title, work, NELEMENTS(title));
		SetWindowText(hWndMain, title);
	}
}

