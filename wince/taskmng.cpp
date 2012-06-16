#include	"compiler.h"
#include	"taskmng.h"


	BOOL	task_avail = FALSE;


void taskmng_initialize(void) {

	task_avail = TRUE;
}

void taskmng_exit(void) {

	task_avail = FALSE;
}

void taskmng_rol(void) {

	MSG		msg;

	if (task_avail) {
		while(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				task_avail = FALSE;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

#if 0	// Å® const macro
BOOL taskmng_isavail(void) {

	return(task_avail);
}
#endif

BOOL taskmng_sleep(UINT32 tick) {

	DWORD	base;

	base = GETTICK();
	while((task_avail) && ((GETTICK() - base) < tick)) {
		taskmng_rol();
		Sleep(1);
	}
	return(task_avail);
}

#if defined(WIN32_PLATFORM_PSPC)
void taskmng_minimize(void) {

extern	HWND	hWndMain;
		MSG		msg;

	ShowWindow(hWndMain, SW_MINIMIZE);
	Sleep(500);

	// Ç≈ ïúãAÇ≥ÇÍÇÈÇ‹Ç≈ë“Ç¬ÅB
	while(1) {
		if (GetActiveWindow() == hWndMain) {
			break;
		}
		if (!GetMessage(&msg, NULL, 0, 0)) {
			task_avail = FALSE;
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
#endif

