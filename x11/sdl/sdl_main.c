#include "compiler.h"

#include "np2.h"
#include "dosio.h"
#include "scrndraw.h"
#include "timing.h"
#include "vramhdl.h"
#include "menubase.h"
#include "sysmenu.h"

#include "toolkit.h"

#include "joymng.h"
#include "mousemng.h"
#include "scrnmng.h"
#include "taskmng.h"

#include "sdl/xnp2.h"
#include "sdl/sdl_keyboard.h"


/*
 * toolkit
 */
const char *
gui_sdl_get_toolkit(void)
{

	return "sdl";
}

BOOL
gui_sdl_arginit(int *argcp, char ***argvp)
{

	UNUSED(argcp);
	UNUSED(argvp);

	return SUCCESS;
}

void
gui_sdl_widget_create(void)
{
}

void
gui_sdl_terminate(void)
{

	SDL_Quit();
}

void
gui_sdl_widget_show(void)
{
}

void
gui_sdl_widget_mainloop(void)
{

	while (taskmng_isavail()) {
		gui_sdl_event_process();
		mainloop(0);
	}
}

void
gui_sdl_widget_quit(void)
{

	taskmng_exit();
}

void
gui_sdl_event_process(void)
{
	SDL_Event e;

	if (!taskmng_isavail() || !SDL_PollEvent(&e)) {
		return;
	}

	switch (e.type) {
	case SDL_MOUSEMOTION:
		if (menuvram == NULL) {
		} else {
			menubase_moving(e.motion.x, e.motion.y, 0);
		}
		break;

	case SDL_MOUSEBUTTONUP:
		switch(e.button.button) {
		case SDL_BUTTON_LEFT:
			if (menuvram == NULL) {
				sysmenu_menuopen(0, e.button.x, e.button.y);
			} else {
				menubase_moving(e.button.x, e.button.y, 2);
			}
			break;

		case SDL_BUTTON_RIGHT:
			break;
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
		switch(e.button.button) {
		case SDL_BUTTON_LEFT:
			if (menuvram == NULL) {
			} else {
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
			} else {
				menubase_close();
			}
		} else {
			sdlkbd_keydown(e.key.keysym.sym);
		}
		break;

	case SDL_KEYUP:
		sdlkbd_keyup(e.key.keysym.sym);
		break;

	case SDL_QUIT:
		taskmng_exit();
		break;
	}
}

void
gui_sdl_set_window_title(const char* str)
{

	SDL_WM_SetCaption(str, str);
}

void
gui_sdl_messagebox(const char *title, const char *msg)
{

	printf("%s\n", title);
	printf(msg);
}

/* toolkit data */
gui_toolkit_t gtk_toolkit = {
	gui_sdl_get_toolkit,
	gui_sdl_arginit,
	gui_sdl_terminate,
	gui_sdl_widget_create,
	gui_sdl_widget_show,
	gui_sdl_widget_mainloop,
	gui_sdl_widget_quit,
	gui_sdl_event_process,
	gui_sdl_set_window_title,
	gui_sdl_messagebox,
};
