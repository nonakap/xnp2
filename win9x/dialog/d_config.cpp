#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"oemtext.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"


static const CBPARAM cpBase[] =
{
	{MAKEINTRESOURCE(IDS_2_0MHZ),	PCBASECLOCK20},
	{MAKEINTRESOURCE(IDS_2_5MHZ),	PCBASECLOCK25},
};
static const UINT32 mulval[10] = {1, 2, 4, 5, 6, 8, 10, 12, 16, 20};
static const TCHAR str_clockfmt[] = _T("%2u.%.4u");


static void setclock(HWND hWnd, UINT uMultiple)
{
	UINT	uClock;
	TCHAR	szWork[32];

	uClock = dlgs_getcbcur(hWnd, IDC_BASECLOCK, PCBASECLOCK20);

	if (uMultiple == 0)
	{
		GetDlgItemText(hWnd, IDC_MULTIPLE, szWork, NELEMENTS(szWork));
		uMultiple = (UINT)miltchar_solveINT(szWork);
	}
	uMultiple = max(uMultiple, 1);
	uMultiple = min(uMultiple, 32);

	uClock = (uClock / 100) * uMultiple;
	wsprintf(szWork, str_clockfmt, uClock / 10000, uClock % 10000);
	SetDlgItemText(hWnd, IDC_CLOCKMSG, szWork);
}

static void cfgcreate(HWND hWnd) {

	TCHAR	work[32];
	UINT	val;

	dlgs_setcbitem(hWnd, IDC_BASECLOCK, cpBase, NELEMENTS(cpBase));
	if (np2cfg.baseclock < AVE(PCBASECLOCK25, PCBASECLOCK20))
	{
		val = PCBASECLOCK20;
	}
	else
	{
		val = PCBASECLOCK25;
	}
	dlgs_setcbcur(hWnd, IDC_BASECLOCK, val);

	SETLISTUINT32(hWnd, IDC_MULTIPLE, mulval);
	wsprintf(work, tchar_u, np2cfg.multiple);
	SetDlgItemText(hWnd, IDC_MULTIPLE, work);

	if (!milstr_cmp(np2cfg.model, str_VM))
	{
		val = IDC_MODELVM;
	}
	else if (!milstr_cmp(np2cfg.model, str_EPSON))
	{
		val = IDC_MODELEPSON;
	}
	else
	{
		val = IDC_MODELVX;
	}
	SetDlgItemCheck(hWnd, val, TRUE);

	if (np2cfg.samplingrate < AVE(11025, 22050))
	{
		val = IDC_RATE11;
	}
	else if (np2cfg.samplingrate < AVE(22050, 44100))
	{
		val = IDC_RATE22;
	}
	else
	{
		val = IDC_RATE44;
	}
	SetDlgItemCheck(hWnd, val, TRUE);
	wsprintf(work, tchar_u, np2cfg.delayms);
	SetDlgItemText(hWnd, IDC_SOUNDBUF, work);

	SetDlgItemCheck(hWnd, IDC_ALLOWRESIZE, np2oscfg.thickframe);
#if !defined(_WIN64)
	if (mmxflag & MMXFLAG_NOTSUPPORT)
	{
		EnableWindow(GetDlgItem(hWnd, IDC_DISABLEMMX), FALSE);
		SetDlgItemCheck(hWnd, IDC_DISABLEMMX, TRUE);
	}
	else
	{
		SetDlgItemCheck(hWnd, IDC_DISABLEMMX, np2oscfg.disablemmx);
	}
#endif
	SetDlgItemCheck(hWnd, IDC_COMFIRM, np2oscfg.comfirm);
	SetDlgItemCheck(hWnd, IDC_RESUME, np2oscfg.resume);
	setclock(hWnd, 0);
	SetFocus(GetDlgItem(hWnd, IDC_BASECLOCK));
}

static void cfgupdate(HWND hWnd)
{
	UINT		update;
	TCHAR		work[32];
	UINT		val;
const OEMCHAR	*str;

	update = 0;

	val = dlgs_getcbcur(hWnd, IDC_BASECLOCK, PCBASECLOCK20);
	if (np2cfg.baseclock != val)
	{
		np2cfg.baseclock = val;
		update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
	}

	GetDlgItemText(hWnd, IDC_MULTIPLE, work, sizeof(work));
	val = (UINT)miltchar_solveINT(work);
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
	val = (UINT)miltchar_solveINT(work);
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

	val = GetDlgItemCheck(hWnd, IDC_ALLOWRESIZE);
	if (np2oscfg.thickframe != (UINT8)val) {
		np2oscfg.thickframe = (UINT8)val;
		update |= SYS_UPDATEOSCFG;
	}

#if !defined(_WIN64)
	if (!(mmxflag & MMXFLAG_NOTSUPPORT))
	{
		val = GetDlgItemCheck(hWnd, IDC_DISABLEMMX);
		if (np2oscfg.disablemmx != (UINT8)val)
		{
			np2oscfg.disablemmx = (UINT8)val;
			mmxflag &= ~MMXFLAG_DISABLE;
			mmxflag |= (val)?MMXFLAG_DISABLE:0;
			update |= SYS_UPDATEOSCFG;
		}
	}
#endif

	val = GetDlgItemCheck(hWnd, IDC_COMFIRM);
	if (np2oscfg.comfirm != (UINT8)val)
	{
		np2oscfg.comfirm = (UINT8)val;
		update |= SYS_UPDATEOSCFG;
	}

	val = GetDlgItemCheck(hWnd, IDC_RESUME);
	if (np2oscfg.resume != (UINT8)val)
	{
		np2oscfg.resume = (UINT8)val;
		update |= SYS_UPDATEOSCFG;
	}
	sysmng_update(update);
}

LRESULT CALLBACK CfgDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_INITDIALOG:
			cfgcreate(hWnd);
			return(FALSE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
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

