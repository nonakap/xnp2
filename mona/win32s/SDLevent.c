#include	"compiler.h"

#define	EVENT_MAX	128

extern	BOOL	__sdl_avail;
extern	int		__sdl_mousex;
extern	int		__sdl_mousey;

static	SDL_Event	events[EVENT_MAX];
static	UINT		eventp = 0;
static	UINT		eventc = 0;


int SDL_PushEvent(SDL_Event *event) {

	if (eventc < (sizeof(events)/sizeof(SDL_Event))) {
		events[(eventp + eventc) % EVENT_MAX] = *event;
		eventc++;
		return(1);
	}
	else {
		return(0);
	}
}

int SDL_PollEvent(SDL_Event *event) {

	MSG		msg;

	if (__sdl_avail) {
		while(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				SDL_Event quitevent;
				ZeroMemory(&quitevent, sizeof(quitevent));
				quitevent.type = SDL_QUIT;
				SDL_PushEvent(&quitevent);
				__sdl_avail = FALSE;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	if (eventc) {
		eventc--;
		if (event) {
			*event = events[eventp];
		}
		eventp = (eventp + 1) % EVENT_MAX;
		return(1);
	}
	else {
		return(0);
	}
}

BYTE SDL_GetMouseState(int *x, int *y) {

	if (x) {
		*x = __sdl_mousex;
	}
	if (y) {
		*y = __sdl_mousey;
	}
	return(0);
}

