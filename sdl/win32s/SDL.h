
#include	"SDLmpw.h"
#include	"SDLevent.h"
#include	"SDLvideo.h"
#include	"SDLaudio.h"

enum {
	SDL_INIT_TIMER			= 0x00000001,
	SDL_INIT_AUDIO			= 0x00000010,
	SDL_INIT_VIDEO			= 0x00000020,
//	SDL_INIT_CDROM			= 0x00000100,
//	SDL_INIT_JOYSTICK		= 0x00000200,
//	SDL_INIT_NOPARACHUTE	= 0x00100000,
//	SDL_INIT_EVENTTHREAD	= 0x01000000,
	SDL_INIT_EVERYTHING		= 0x0000FFFF
};


#ifdef __cplusplus
extern "C" {
#endif

extern int SDL_main(int argc, char **argv);

int SDL_InitSubSystem(DWORD flags);
void SDL_Quit(void);
char *SDL_GetError(void);

#ifdef __cplusplus
}
#endif

