#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"oemtext.h"
#include	"np2class.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"np2ver.h"
#include	"pccore.h"
#include	"np2info.h"


static	SIZE	s_szAbout;

static const OEMCHAR str_np2title[] = OEMTEXT(PROJECTNAME) \
										OEMTEXT(PROJECTSUBNAME) \
										OEMTEXT("  ");
static const OEMCHAR np2infostr[] = OEMTEXT("CPU: %CPU% %CLOCK%\nMEM: %MEM1%\nGDC: %GDC%\n     %GDC2%\nTEXT: %TEXT%\nGRPH: %GRPH%\nSOUND: %EXSND%\n\nBIOS: %BIOS%\nRHYTHM: %RHYTHM%\n\nSCREEN: %DISP%");


static void onInitDialog(HWND hWnd)
{
	OEMCHAR	szWork[128];
	RECT	rect;
	RECT	rectMore;
	RECT	rectInfo;
	int		nHeight;
	POINT	pt;
#if defined(OSLANG_UTF8)
	TCHAR	szWork2[128];
#endif	// defined(OSLANG_UTF8)

	milstr_ncpy(szWork, str_np2title, NELEMENTS(szWork));
	milstr_ncat(szWork, np2version, NELEMENTS(szWork));
#if defined(NP2VER_WIN9X)
	milstr_ncat(szWork, NP2VER_WIN9X, NELEMENTS(szWork));
#endif
#if defined(OSLANG_UTF8)
	oemtotchar(szWork2, NELEMENTS(szWork2), szWork, -1);
	SetDlgItemText(hWnd, IDC_NP2VER, szWork2);
#else
	SetDlgItemText(hWnd, IDC_NP2VER, szWork);
#endif

	GetWindowRect(hWnd, &rect);
	s_szAbout.cx = rect.right - rect.left;
	s_szAbout.cy = rect.bottom - rect.top;

	if ((dlgs_getitemrect(hWnd, IDC_MORE, &rectMore)) &&
		(dlgs_getitemrect(hWnd, IDC_NP2INFO, &rectInfo)))
	{
		nHeight = s_szAbout.cy - (rectInfo.bottom - rectMore.bottom);
		GetClientRect(GetParent(hWnd), &rect);
		pt.x = (rect.right - rect.left - s_szAbout.cx) / 2;
		pt.y = (rect.bottom - rect.top - s_szAbout.cy) / 2;
		ClientToScreen(GetParent(hWnd), &pt);
		np2class_move(hWnd, pt.x, pt.y, s_szAbout.cx, nHeight);
	}

	SetFocus(GetDlgItem(hWnd, IDOK));
}

static void onMore(HWND hWnd)
{
	OEMCHAR	szInfo[1024];
	RECT	rect;
#if defined(OSLANG_UTF8)
	TCHAR	szInfo2[1024];
#endif	// defined(OSLANG_UTF8)

	np2info(szInfo, np2infostr, NELEMENTS(szInfo), NULL);
#if defined(OSLANG_UTF8)
	oemtotchar(szInfo2, NELEMENTS(szInfo2), szInfo, -1);
	SetDlgItemText(hWnd, IDC_NP2INFO, szInfo2);
#else
	SetDlgItemText(hWnd, IDC_NP2INFO, szInfo);
#endif
	EnableWindow(GetDlgItem(hWnd, IDC_MORE), FALSE);
	GetWindowRect(hWnd, &rect);
	np2class_move(hWnd, rect.left, rect.top, s_szAbout.cx, s_szAbout.cy);
	SetFocus(GetDlgItem(hWnd, IDOK));
}

LRESULT CALLBACK AboutDialogProc(HWND hWnd, UINT uMsg,
												WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			onInitDialog(hWnd);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					EndDialog(hWnd, IDOK);
					return TRUE;

				case IDC_MORE:
					onMore(hWnd);
					break;
			}
			break;

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDOK, 0);
			break;
	}
	return FALSE;
}

