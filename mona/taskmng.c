#include	"compiler.h"
// #include	<signal.h>
#include	"inputmng.h"
#include	"taskmng.h"
#include	"sdlkbd.h"
#include	"vramhdl.h"
#include	"menubase.h"
#include	"sysmenu.h"


	BOOL	task_avail;


void sighandler(int signo) {

	(void)signo;
	task_avail = FALSE;
}


void taskmng_initialize(void) {

	task_avail = TRUE;
}

void taskmng_exit(void) {

	task_avail = FALSE;
}

void taskmng_rol(void) {

	BOOL		moving;
	int			x;
	int			y;
	SDL_Event	e;

	moving = FALSE;
	x = 0;
	y = 0;
	while((task_avail) && (SDL_PollEvent(&e))) {
		switch(e.type) {
			case SDL_MOUSEMOTION:
				moving = TRUE;
				x = e.motion.x;
				y = e.motion.y;
				break;

			case SDL_MOUSEBUTTONUP:
				if (moving) {
					moving = FALSE;
					if (menuvram == NULL) {
					}
					else {
						menubase_moving(x, y, 0);
					}
				}
				switch(e.button.button) {
					case SDL_BUTTON_LEFT:
						if (menuvram == NULL) {
							sysmenu_menuopen(0, e.button.x, e.button.y);
						}
						else {
							menubase_moving(e.button.x, e.button.y, 2);
						}
						break;

					case SDL_BUTTON_RIGHT:
						break;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (moving) {
					moving = FALSE;
					if (menuvram == NULL) {
					}
					else {
						menubase_moving(x, y, 0);
					}
				}
				switch(e.button.button) {
					case SDL_BUTTON_LEFT:
						if (menuvram == NULL) {
						}
						else {
							menubase_moving(e.button.x, e.button.y, 1);
						}
						break;

					case SDL_BUTTON_RIGHT:
						break;
				}
				break;

			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_F11) {
					if (menuvram == NULL) {
						sysmenu_menuopen(0, 0, 0);
					}
					else {
						menubase_close();
					}
				}
				else {
					sdlkbd_keydown(e.key.keysym.sym);
				}
				break;

			case SDL_KEYUP:
				sdlkbd_keyup(e.key.keysym.sym);
				break;

			case SDL_QUIT:
				task_avail = FALSE;
				break;
		}
	}
	if (moving) {
		moving = FALSE;
		if (menuvram == NULL) {
		}
		else {
			menubase_moving(x, y, 0);
		}
	}
}

BOOL taskmng_sleep(UINT32 tick) {

	UINT32	base;

	base = GETTICK();
	while((task_avail) && ((GETTICK() - base) < tick)) {
		taskmng_rol();
#if !defined(TARGET_MONA)
		Sleep(1);
#endif
	}
	return(task_avail);
}

