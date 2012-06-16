#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"


static const TCHAR str_2halfmhz[] = _T("2.4576MHz");
static const TCHAR str_2mhz[] = _T("1.9968MHz");
static const TCHAR *basecstr[2] = {str_2mhz, str_2halfmhz};
static const UINT32 mulval[10] = {1, 2, 4, 5, 6, 8, 10, 12, 16, 20};
static const TCHAR str_clockfmt[] = _T("%2u.%.4u");


static void setclock(HWND hWnd, UINT multiple) {

	UINT32	clock;
	TCHAR	work[32];

	GetDlgItemText(hWnd, IDC_BASECLOCK, work, NELEMENTS(work));
	if (work[0] == '1') {
		clock = PCBASECLOCK20 / 100;
	}
	else {
		clock = PCBASECLOCK25 / 100;
	}
	if (multiple == 0) {
		GetDlgItemText(hWnd, IDC_MULTIPLE, work, NELEMENTS(work));
		multiple = (UINT)milstr_solveINT(work);
	}
	if (multiple < 1) {
		multiple = 1;
	}
	else if (multiple > 32) {
		multiple = 32;
	}
	clock *= multiple;
	wsprintf(work, str_clockfmt, clock / 10000, clock % 10000);
	SetDlgItemText(hWnd, IDC_CLOCKMSG, work);
}

static void cfgcreate(HWND hWnd) {

	TCHAR	work[32];
	UINT	val;

	SETLISTSTR(hWnd, IDC_BASECLOCK, basecstr);
	if (np2cfg.baseclock < AVE(PCBASECLOCK25, PCBASECLOCK20)) {
		val = 0;
	}
	else {
		val = 1;
	}
	SendDlgItemMessage(hWnd, IDC_BASECLOCK, CB_SETCURSEL, val, 0);
	SETLISTUINT32(hWnd, IDC_MULTIPLE, mulval);
	wsprintf(work, str_u, np2cfg.multiple);
	SetDlgItemText(hWnd, IDC_MULTIPLE, work);

	if (!milstr_cmp(np2cfg.model, str_VM)) {
		val = IDC_MODELVM;
	}
	else if (!milstr_cmp(np2cfg.model, str_EPSON)) {
		val = IDC_MODELEPSON;
	}
	else {
		val = IDC_MODELVX;
	}
	SetDlgItemCheck(hWnd, val, TRUE);

	if (np2cfg.samplingrate < AVE(11025, 22050)) {
		val = IDC_RATE11;
	}
	else if (np2cfg.samplingrate < AVE(22050, 44100)) {
		val = IDC_RATE22;
	}
	else {
		val = IDC_RATE44;
	}
	SetDlgItemCheck(hWnd, val, TRUE);
	wsprintf(work, str_u, np2cfg.delayms);
	SetDlgItemText(hWnd, IDC_SOUNDBUF, work);

	setclock(hWnd, 0);
	SetFocus(GetDlgItem(hWnd, IDC_BASECLOCK));
}

static void cfgupdate(HWND hWnd) {

	UINT	update;
	TCHAR	work[32];
	UINT	val;
const TCHAR	*str;

	update = 0;
	GetDlgItemText(hWnd, IDC_BASECLOCK, work, NELEMENTS(work));
	if (work[0] == '1') {
		val = PCBASECLOCK20;
	}
	else {
		val = PCBASECLOCK25;
	}
	if (np2cfg.baseclock != val) {
		np2cfg.baseclock = val;
		update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
	}

	GetDlgItemText(hWnd, IDC_MULTIPLE, work, NELEMENTS(work));
	val = (UINT)milstr_solveINT(work);
	if (val < 1) {
		val = 1;
	}
	else if (val > 32) {
		val = 32;
	}
	if (np2cfg.multiple != val) {
		np2cfg.multiple = val;
		update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
	}

	if (GetDlgItemCheck(hWnd, IDC_MODELVM)) {
		str = str_VM;
	}
	else if (GetDlgItemCheck(hWnd, IDC_MODELEPSON)) {
		str = str_EPSON;
	}
	else {
		str = str_VX;
	}
	if (milstr_cmp(np2cfg.model, str)) {
		milstr_ncpy(np2cfg.model, str, NELEMENTS(np2cfg.model));
		update |= SYS_UPDATECFG;
	}

	if (GetDlgItemCheck(hWnd, IDC_RATE11)) {
		val = 11025;
	}
	else if (GetDlgItemCheck(hWnd, IDC_RATE22)) {
		val = 22050;
	}
	else {
		val = 44100;
	}
	if (np2cfg.samplingrate != (UINT16)val) {
		np2cfg.samplingrate = (UINT16)val;
		update |= SYS_UPDATECFG | SYS_UPDATERATE;
		soundrenewal = 1;
	}

	GetDlgItemText(hWnd, IDC_SOUNDBUF, work, NELEMENTS(work));
	val = (UINT)milstr_solveINT(work);
	if (val < 40) {
		val = 40;
	}
	else if (val > 1000) {
		val = 1000;
	}
	if (np2cfg.delayms != (UINT16)val) {
		np2cfg.delayms = (UINT16)val;
		update |= SYS_UPDATECFG | SYS_UPDATESBUF;
		soundrenewal = 1;
	}

	sysmng_update(update);
}

LRESULT CALLBACK CfgDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_INITDIALOG:
			cfgcreate(hWnd);
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					cfgupdate(hWnd);
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

				case IDC_BASECLOCK:
					setclock(hWnd, 0);
					return(FALSE);

				case IDC_MULTIPLE:
					if (HIWORD(wp) == CBN_SELCHANGE) {
						UINT val;
						val = (UINT)SendDlgItemMessage(hWnd, IDC_MULTIPLE,
														CB_GETCURSEL, 0, 0);
						if (val < NELEMENTS(mulval)) {
							setclock(hWnd, mulval[val]);
						}
					}
					else {
						setclock(hWnd, 0);
					}
					return(FALSE);

				default:
					return(FALSE);
			}
			break;

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
			break;

		default:
			return(FALSE);
	}
	return(TRUE);
}

