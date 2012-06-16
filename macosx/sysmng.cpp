#include	"compiler.h"
#include	"np2.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"diskdrv.h"

	UINT	sys_updates;


static	char	strtitle[256];
static	char	strclock[64];

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

void sysmng_updatecaption(BYTE flag) {

    char	name1[255], name2[255];
	char	work[256];
#ifndef NP2GCC
	Str255	str;
#endif

	if (flag & 1) {
		strtitle[0] = '\0';
		if (fdd_diskready(0)) {
			milstr_ncat(strtitle, "  FDD1:", sizeof(strtitle));
            if (getLongFileName(name1, fdd_diskname(0))) {
                milstr_ncat(strtitle, name1, sizeof(strtitle));
            }
            else {
                milstr_ncat(strtitle, file_getname((char *)fdd_diskname(0)),
															sizeof(strtitle));
            }
		}
		if (fdd_diskready(1)) {
			milstr_ncat(strtitle, "  FDD2:", sizeof(strtitle));
            if (getLongFileName(name2, fdd_diskname(1))) {
                milstr_ncat(strtitle, name2, sizeof(strtitle));
            }
            else {
                milstr_ncat(strtitle, file_getname((char *)fdd_diskname(1)),
															sizeof(strtitle));
            }
		}
	}
	if (flag & 2) {
		strclock[0] = '\0';
		if (np2oscfg.DISPCLK & 2) {
			if (workclock.fps) {
				SPRINTF(strclock, " - %u.%1uFPS",
								workclock.fps / 10, workclock.fps % 10);
			}
			else {
				milstr_ncpy(strclock, " - 0FPS", sizeof(strclock));
			}
		}
		if (np2oscfg.DISPCLK & 1) {
			SPRINTF(work, " %2u.%03uMHz",
								workclock.khz / 1000, workclock.khz % 1000);
			if (strclock[0] == '\0') {
				milstr_ncpy(strclock, " -", sizeof(strclock));
			}
			milstr_ncat(strclock, work, sizeof(strclock));
		}
	}
#if defined(NP2GCC)
	milstr_ncpy(work, np2oscfg.titles, sizeof(work));
#else
#if !defined(SUPPORT_PC9821)
	milstr_ncpy(work, "Neko Project II", sizeof(work));
#else
	milstr_ncpy(work, "Neko Project 21", sizeof(work));
#endif
#endif
	milstr_ncat(work, strtitle, sizeof(work));
	milstr_ncat(work, strclock, sizeof(work));

#if defined(NP2GCC)
	CFStringRef cfstr;
	cfstr = CFStringCreateWithCString(NULL, work, CFStringGetSystemEncoding());
    SetWindowTitleWithCFString(hWndMain, cfstr);
	CFRelease(cfstr);
#else
	mkstr255(str, work);
	SetWTitle(hWndMain, str);
#endif
}
