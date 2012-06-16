
#include	"cmndraw.h"

#define	DD2HDL		void *


DD2HDL dd2_create(HWND hwnd, int width, int height);
void dd2_release(DD2HDL dd2hdl);
CMNVRAM *dd2_bsurflock(DD2HDL dd2);
void dd2_bsurfunlock(DD2HDL dd2);
void dd2_blt(DD2HDL dd2, const POINT *pt, const RECT *rect);
UINT16 dd2_get16pal(DD2HDL dd2, RGB32 pal);

