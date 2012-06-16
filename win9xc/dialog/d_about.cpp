#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"np2info.h"


static const TCHAR str_np2title[] = _T("Neko Project II  ");
static const TCHAR np2infostr[] = 								\
					_T("CPU: %CPU% %CLOCK%\n")					\
					_T("MEM: %MEM1%\n")							\
					_T("GDC: %GDC%\n")							\
					_T("     %GDC2%\n")							\
					_T("TEXT: %TEXT%\n")						\
					_T("GRPH: %GRPH%\n")						\
					_T("SOUND: %EXSND%\n")						\
					_T("\n")									\
					_T("BIOS: %BIOS%\n")						\
					_T("RHYTHM: %RHYTHM%\n")					\
					_T("\n")									\
					_T("SCREEN: %DISP%");


static void about_init(HWND hWnd) {

	TCHAR	work[128];
	TCHAR	infostr[1024];

	milstr_ncpy(work, str_np2title, NELEMENTS(work));
	milstr_ncat(work, np2version, NELEMENTS(work));
	SetDlgItemText(hWnd, IDC_NP2VER, work);
	np2info(infostr, np2infostr, NELEMENTS(infostr), NULL);
	SetDlgItemText(hWnd, IDC_NP2INFO, infostr);
	SetFocus(GetDlgItem(hWnd, IDOK));
}

LRESULT CALLBACK AboutDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_INITDIALOG:
			about_init(hWnd);
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					EndDialog(hWnd, IDOK);
					break;

				default:
					return(FALSE);
			}
			break;

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDOK, 0);
			break;

		default:
			return(FALSE);
	}
	return(TRUE);
}

