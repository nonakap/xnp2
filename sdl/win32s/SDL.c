#include	"compiler.h"


static	char	__sdl_error[256];


int SDL_InitSubSystem(DWORD flags) {

	return(0);
}

void SDL_Quit(void) {
}

char *SDL_GetError(void) {

	return(__sdl_error);
}

void __sdl_seterror(const char *error) {

	milstr_ncpy(__sdl_error, error, sizeof(__sdl_error));
}


// ----

static	const char	szClassName[] = "Windebug-SDL";
static	const char	szCaptionName[] = "SDL-Window";

		BOOL		__sdl_avail = FALSE;
		HWND		__sdl_hWnd = NULL;
		SDL_Surface	*__sdl_vsurf = NULL;
		int			__sdl_eventw = 0;
		int			__sdl_mousex = 0;
		int			__sdl_mousey = 0;
		BYTE		__sdl_mouseb = 0;
static	SINT16		winkeytbl[256];
extern	void		__sdl_videoinit(void);
extern	void		__sdl_videopaint(HWND hWnd, SDL_Surface *screen);
extern	void		__sdl_audio_cb(UINT msg, HWAVEOUT hwo, WAVEHDR *whd);

typedef struct {
	UINT16	wincode;
	SINT16	sdlcode;
} SDLKEYTBL;

#define	VK_SEMICOLON	0xBA		// ÅG (106 :)
#define	VK_EQUALS		0xBB		// ÅÅ (106 /)
#define	VK_COMMA		0xBC		// ÅC
#define	VK_MINUS		0xBD		// Å|
#define	VK_PERIOD		0xBE		// ÅD
#define	VK_SLASH		0xBF		// Å^
#define	VK_GRAVE		0xC0		// Åó
#define	VK_LBRACKET		0xDB		// Åm
#define	VK_BACKSLASH	0xDC		// Åè
#define	VK_RBRACKET		0xDD		// Ån
#define	VK_APOSTROPHE	0xDE		// ÅO
#define	VK_BACKTICK		0xDF		// Åe (106 .)

static const SDLKEYTBL sdlkeytbl[] = {
		{VK_BACK,		SDLK_BACKSPACE},	{VK_TAB,		SDLK_TAB},
		{VK_CLEAR,		SDLK_CLEAR},		{VK_RETURN,		SDLK_RETURN},
		{VK_PAUSE,		SDLK_PAUSE},		{VK_ESCAPE,		SDLK_ESCAPE},
		{VK_SPACE,		SDLK_SPACE},		{VK_APOSTROPHE,	SDLK_QUOTE},
		{VK_COMMA,		SDLK_COMMA},		{VK_MINUS,		SDLK_MINUS},
		{VK_PERIOD,		SDLK_PERIOD},		{VK_SLASH,		SDLK_SLASH},

		{'0', SDLK_0}, {'1', SDLK_1}, {'2', SDLK_2}, {'3', SDLK_3},
		{'4', SDLK_4}, {'5', SDLK_5}, {'6', SDLK_6}, {'7', SDLK_7},
		{'8', SDLK_8}, {'9', SDLK_9},

		{VK_SEMICOLON,	SDLK_SEMICOLON},	{VK_EQUALS,		SDLK_EQUALS},

		{VK_LBRACKET,	SDLK_LEFTBRACKET},	{VK_BACKSLASH,	SDLK_BACKSLASH},
		{VK_RBRACKET,	SDLK_RIGHTBRACKET},	{VK_GRAVE,		SDLK_BACKQUOTE},
		{VK_BACKTICK,	SDLK_BACKQUOTE},

		{'A', SDLK_a}, {'B', SDLK_b}, {'C', SDLK_c}, {'D', SDLK_d},
		{'E', SDLK_e}, {'F', SDLK_f}, {'G', SDLK_g}, {'H', SDLK_h},
		{'I', SDLK_i}, {'J', SDLK_j}, {'K', SDLK_k}, {'L', SDLK_l},
		{'M', SDLK_m}, {'N', SDLK_n}, {'O', SDLK_o}, {'P', SDLK_p},
		{'Q', SDLK_q}, {'R', SDLK_r}, {'S', SDLK_s}, {'T', SDLK_t},
		{'U', SDLK_u}, {'V', SDLK_v}, {'W', SDLK_w}, {'X', SDLK_x},
		{'Y', SDLK_y}, {'Z', SDLK_z},

		{VK_DELETE,		SDLK_DELETE},

		{VK_NUMPAD0,	SDLK_KP0},			{VK_NUMPAD1,	SDLK_KP1},
		{VK_NUMPAD2,	SDLK_KP2},			{VK_NUMPAD3,	SDLK_KP3},
		{VK_NUMPAD4,	SDLK_KP4},			{VK_NUMPAD5,	SDLK_KP5},
		{VK_NUMPAD6,	SDLK_KP6},			{VK_NUMPAD7,	SDLK_KP7},
		{VK_NUMPAD8,	SDLK_KP8},			{VK_NUMPAD9,	SDLK_KP9},
		{VK_DECIMAL,	SDLK_KP_PERIOD},	{VK_DIVIDE,		SDLK_KP_DIVIDE},
		{VK_MULTIPLY,	SDLK_KP_MULTIPLY},	{VK_SUBTRACT,	SDLK_KP_MINUS},
		{VK_ADD,		SDLK_KP_PLUS},

		{VK_UP,			SDLK_UP},			{VK_DOWN,		SDLK_DOWN},
		{VK_RIGHT,		SDLK_RIGHT},		{VK_LEFT,		SDLK_LEFT},
		{VK_INSERT,		SDLK_INSERT},		{VK_HOME,		SDLK_HOME},
		{VK_END,		SDLK_END},			{VK_PRIOR,		SDLK_PAGEUP},
		{VK_NEXT,		SDLK_PAGEDOWN},

		{VK_F1,			SDLK_F1},			{VK_F2,			SDLK_F2},
		{VK_F3,			SDLK_F3},			{VK_F4,			SDLK_F4},
		{VK_F5,			SDLK_F5},			{VK_F6,			SDLK_F6},
		{VK_F7,			SDLK_F7},			{VK_F8,			SDLK_F8},
		{VK_F9,			SDLK_F9},			{VK_F10,		SDLK_F10},
		{VK_F11,		SDLK_F11},			{VK_F12,		SDLK_F12},

		{VK_NUMLOCK,	SDLK_NUMLOCK},		{VK_CAPITAL,	SDLK_CAPSLOCK},
		{VK_SCROLL,		SDLK_SCROLLOCK},	{VK_RSHIFT,		SDLK_RSHIFT},
		{VK_LSHIFT,		SDLK_LSHIFT},		{VK_RCONTROL,	SDLK_RCTRL},
		{VK_LCONTROL,	SDLK_LCTRL},		{VK_RMENU,		SDLK_RALT},
		{VK_LMENU,		SDLK_LALT},			{VK_RWIN,		SDLK_RSUPER},
		{VK_LWIN,		SDLK_LSUPER},		{VK_HELP,		SDLK_HELP},
		{VK_PRINT,		SDLK_PRINT},		{VK_SNAPSHOT,	SDLK_PRINT},
		{VK_CANCEL,		SDLK_BREAK},		{VK_APPS,		SDLK_MENU}};


static void makekeytbl(void) {

	int			i;
const SDLKEYTBL	*key;
const SDLKEYTBL	*keyterm;

	for (i=0; i<256; i++) {
		winkeytbl[i] = SDLK_UNKNOWN;
	}
	key = sdlkeytbl;
	keyterm = key + (sizeof(sdlkeytbl)/sizeof(SDLKEYTBL));
	while(key < keyterm) {
		winkeytbl[key->wincode & 0xff] = key->sdlcode;
		key++;
	}
}

static short cnvsdlkey(WPARAM wp, LPARAM lp) {

	switch(wp) {
		case VK_CONTROL:
			return((lp & (1 << 24))?SDLK_RCTRL:SDLK_LCTRL);

		case VK_SHIFT:
			return(SDLK_LSHIFT);

		case VK_MENU:
			return((lp & (1 << 24))?SDLK_RALT:SDLK_LALT);
	}
	return(winkeytbl[wp & 0xff]);
}

static LRESULT CALLBACK SdlProc(HWND hWnd, UINT msg,
											WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT	ps;
	HDC			hdc;
	SDL_Event	event;

	switch (msg) {
		case WM_CREATE:
			makekeytbl();
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			__sdl_videopaint(hWnd, __sdl_vsurf);
			EndPaint(hWnd, &ps);
			break;

		case WM_MOUSEMOVE:
			__sdl_mousex = LOWORD(lParam);
			__sdl_mousey = HIWORD(lParam);
			ZeroMemory(&event, sizeof(event));
			event.motion.type = SDL_MOUSEMOTION;
			event.motion.state = __sdl_mouseb;
			event.motion.x = LOWORD(lParam);
			event.motion.y = HIWORD(lParam);
			SDL_PushEvent(&event);
			break;

		case WM_LBUTTONDOWN:
			__sdl_mousex = LOWORD(lParam);
			__sdl_mousey = HIWORD(lParam);
			__sdl_mouseb |= 1 << (SDL_BUTTON_LEFT - 1);
			ZeroMemory(&event, sizeof(event));
			event.button.type = SDL_MOUSEBUTTONDOWN;
			event.button.button = SDL_BUTTON_LEFT;
			event.button.state = SDL_PRESSED;
			event.button.x = LOWORD(lParam);
			event.button.y = HIWORD(lParam);
			SDL_PushEvent(&event);
			break;

		case WM_LBUTTONUP:
			__sdl_mousex = LOWORD(lParam);
			__sdl_mousey = HIWORD(lParam);
			__sdl_mouseb &= ~(1 << (SDL_BUTTON_LEFT - 1));
			ZeroMemory(&event, sizeof(event));
			event.button.type = SDL_MOUSEBUTTONUP;
			event.button.button = SDL_BUTTON_LEFT;
			event.button.state = SDL_RELEASED;
			event.button.x = LOWORD(lParam);
			event.button.y = HIWORD(lParam);
			SDL_PushEvent(&event);
			break;

		case WM_RBUTTONDOWN:
			__sdl_mousex = LOWORD(lParam);
			__sdl_mousey = HIWORD(lParam);
			__sdl_mouseb |= 1 << (SDL_BUTTON_RIGHT - 1);
			ZeroMemory(&event, sizeof(event));
			event.button.type = SDL_MOUSEBUTTONDOWN;
			event.button.button = SDL_BUTTON_RIGHT;
			event.button.state = SDL_PRESSED;
			event.button.x = LOWORD(lParam);
			event.button.y = HIWORD(lParam);
			SDL_PushEvent(&event);
			break;

		case WM_RBUTTONUP:
			__sdl_mousex = LOWORD(lParam);
			__sdl_mousey = HIWORD(lParam);
			__sdl_mouseb &= ~(1 << (SDL_BUTTON_RIGHT - 1));
			ZeroMemory(&event, sizeof(event));
			event.button.type = SDL_MOUSEBUTTONUP;
			event.button.button = SDL_BUTTON_RIGHT;
			event.button.state = SDL_RELEASED;
			event.button.x = LOWORD(lParam);
			event.button.y = HIWORD(lParam);
			SDL_PushEvent(&event);
			break;

		case WM_KEYDOWN:
			ZeroMemory(&event, sizeof(event));
			event.key.type = SDL_KEYDOWN;
			event.key.keysym.sym = cnvsdlkey(wParam, lParam);
			if (event.key.keysym.sym != SDLK_UNKNOWN) {
				SDL_PushEvent(&event);
			}
			break;

		case WM_KEYUP:
			ZeroMemory(&event, sizeof(event));
			event.key.type = SDL_KEYUP;
			event.key.keysym.sym = cnvsdlkey(wParam, lParam);
			if (event.key.keysym.sym != SDLK_UNKNOWN) {
				SDL_PushEvent(&event);
			}
			break;

		case WM_CLOSE:
			ZeroMemory(&event, sizeof(event));
			event.type = SDL_QUIT;
			SDL_PushEvent(&event);
			__sdl_avail = FALSE;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
#if 1
		case MM_WOM_DONE:
			__sdl_audio_cb(MM_WOM_DONE, (HWAVEOUT)wParam, (WAVEHDR *)lParam);
			break;
#endif
		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}

static BOOL sdlinit(void) {

	HINSTANCE	hInstance;
	WNDCLASS	sdlcls;

	hInstance = (HINSTANCE)GetWindowLong(HWND_DESKTOP, GWL_HINSTANCE);
	sdlcls.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	sdlcls.lpfnWndProc = SdlProc;
	sdlcls.cbClsExtra = 0;
	sdlcls.cbWndExtra = 0;
	sdlcls.hInstance = hInstance;
	sdlcls.hIcon = NULL;
	sdlcls.hCursor = LoadCursor(NULL, IDC_ARROW);
	sdlcls.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	sdlcls.lpszMenuName = NULL;
	sdlcls.lpszClassName = szClassName;
	if (!RegisterClass(&sdlcls)) {
		return(FAILURE);
	}
	__sdl_hWnd = CreateWindowEx(0,
						szClassName, szCaptionName,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						CW_USEDEFAULT, CW_USEDEFAULT, 320, 240,
						NULL, NULL, hInstance, NULL);
	if (__sdl_hWnd == NULL) {
		return(FAILURE);
	}
	ShowWindow(__sdl_hWnd, SW_SHOW);
	UpdateWindow(__sdl_hWnd);
	__sdl_avail = TRUE;
	return(SUCCESS);
}

static void sdlterm(void) {
}


int main(int argc, char **argv) {

	int		r;

	__sdl_videoinit();
	if (sdlinit() != SUCCESS) {
		return(0);
	}
	r = SDL_main(argc, argv);
	sdlterm();
	return(r);
}

