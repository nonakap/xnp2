#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"oemtext.h"
#include	"commng.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"dipswbmp.h"


#ifdef __cplusplus
extern "C" {
#endif

extern	COMMNG	cm_mpu98;

#ifdef __cplusplus
}
#endif

static const CBPARAM cpInt[] =
{
	{MAKEINTRESOURCE(IDS_INT0),		0},
	{MAKEINTRESOURCE(IDS_INT1),		1},
	{MAKEINTRESOURCE(IDS_INT2),		2},
	{MAKEINTRESOURCE(IDS_INT5),		3},
};

static	UINT8	mpu = 0;

static void setmpuio(HWND hWnd, UINT8 cValue)
{
	dlgs_setcbcur(hWnd, IDC_MPUIO, (cValue & 0xf0));
}

static UINT8 getmpuio(HWND hWnd)
{
	return dlgs_getcbcur(hWnd, IDC_MPUIO, 0x00);
}

static void setmpuint(HWND hWnd, UINT8 cValue)
{
	dlgs_setcbcur(hWnd, IDC_MPUINT, (cValue & 0x03));
}

static UINT8 getmpuint(HWND hWnd)
{
	return dlgs_getcbcur(hWnd, IDC_MPUINT, 0x00);
}

static void setmpujmp(HWND hWnd, UINT8 cValue, UINT8 cBit)
{
	if ((mpu ^ cValue) & cBit)
	{
		mpu &= ~cBit;
		mpu |= cValue;
		InvalidateRect(GetDlgItem(hWnd, IDC_MPUDIP), NULL, TRUE);
	}
}


// ----

static void mpucreate(HWND hWnd)
{
	UINT	i;
	TCHAR	szBuf[8];
	int		nIndex;
	HWND	sub;

	mpu = np2cfg.mpuopt;
	for (i=0; i<16; i++)
	{
		wsprintf(szBuf, tchar_4X, 0xC0D0 + (i << 10));
		nIndex = (int)SendDlgItemMessage(hWnd, IDC_MPUIO, CB_ADDSTRING,
														0, (LPARAM)szBuf);
		if (nIndex >= 0)
		{
			SendDlgItemMessage(hWnd, IDC_MPUIO, CB_SETITEMDATA,
											(WPARAM)nIndex, (LPARAM)(i << 4));
		}
	}
	setmpuio(hWnd, mpu);

	dlgs_setcbitem(hWnd, IDC_MPUINT, cpInt, NELEMENTS(cpInt));
	setmpuint(hWnd, mpu);

	dlgs_setlistmidiout(hWnd, IDC_MPU98MMAP, np2oscfg.mpu.mout);
	dlgs_setlistmidiin(hWnd, IDC_MPU98MDIN, np2oscfg.mpu.min);
	SETLISTSTR(hWnd, IDC_MPU98MMDL, cmmidi_mdlname);
	SetDlgItemText(hWnd, IDC_MPU98MMDL, np2oscfg.mpu.mdl);
	SetDlgItemCheck(hWnd, IDC_MPU98DEFE, np2oscfg.mpu.def_en);
	SetDlgItemText(hWnd, IDC_MPU98DEFF, np2oscfg.mpu.def);

	// SS_OWNERDRAW‚É‚·‚é‚Æ IDE‚Å•s“s‡‚ªo‚é‚Ì‚Åc
	sub = GetDlgItem(hWnd, IDC_MPUDIP);
	SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
							(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));

	SetFocus(GetDlgItem(hWnd, IDC_MPUIO));
}

static void mpuupdate(HWND hWnd) {

	union {
		OEMCHAR	mmap[MAXPNAMELEN];
		OEMCHAR	mmdl[64];
		OEMCHAR	mdef[MAX_PATH];
		OEMCHAR	mdin[MAXPNAMELEN];
	} s;
	UINT	update;

	update = 0;
	if (np2cfg.mpuopt != mpu) {
		np2cfg.mpuopt = mpu;
		update |= SYS_UPDATECFG | SYS_UPDATEMIDI;
	}
	GetDlgItemText(hWnd, IDC_MPU98MMAP, s.mmap, NELEMENTS(s.mmap));
	if (milstr_cmp(np2oscfg.mpu.mout, s.mmap)) {
		milstr_ncpy(np2oscfg.mpu.mout, s.mmap, NELEMENTS(np2oscfg.mpu.mout));
		update |= SYS_UPDATEOSCFG | SYS_UPDATEMIDI;
	}
	GetDlgItemText(hWnd, IDC_MPU98MDIN, s.mdin, NELEMENTS(s.mdin));
	if (milstr_cmp(np2oscfg.mpu.min, s.mdin)) {
		milstr_ncpy(np2oscfg.mpu.min, s.mdin, NELEMENTS(np2oscfg.mpu.min));
		update |= SYS_UPDATEOSCFG | SYS_UPDATEMIDI;
	}
	GetDlgItemText(hWnd, IDC_MPU98MMDL, s.mmdl, NELEMENTS(s.mmdl));
	if (milstr_cmp(np2oscfg.mpu.mdl, s.mmdl)) {
		milstr_ncpy(np2oscfg.mpu.mdl, s.mmdl, NELEMENTS(np2oscfg.mpu.mdl));
		update |= SYS_UPDATEOSCFG | SYS_UPDATEMIDI;
	}

	np2oscfg.mpu.def_en = GetDlgItemCheck(hWnd, IDC_MPU98DEFE);
	if (cm_mpu98) {
		cm_mpu98->msg(cm_mpu98, COMMSG_MIMPIDEFEN, np2oscfg.mpu.def_en);
	}
	GetDlgItemText(hWnd, IDC_MPU98DEFF, s.mdef, NELEMENTS(s.mdef));
	if (milstr_cmp(np2oscfg.mpu.def, s.mdef)) {
		milstr_ncpy(np2oscfg.mpu.def, s.mdef, NELEMENTS(np2oscfg.mpu.def));
		if (cm_mpu98) {
			cm_mpu98->msg(cm_mpu98, COMMSG_MIMPIDEFFILE, (long)s.mdef);
		}
		update |= SYS_UPDATEOSCFG;
	}
	sysmng_update(update);
}

static void mpucmddipsw(HWND hWnd) {

	RECT	rect1;
	RECT	rect2;
	POINT	p;
	BOOL	redraw;
	UINT8	bit;

	GetWindowRect(GetDlgItem(hWnd, IDC_MPUDIP), &rect1);
	GetClientRect(GetDlgItem(hWnd, IDC_MPUDIP), &rect2);
	GetCursorPos(&p);
	redraw = FALSE;
	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 9;
	p.y /= 9;
	if ((p.y < 1) || (p.y >= 3)) {
		return;
	}
	if ((p.x >= 2) && (p.x < 6)) {
		bit = 0x80 >> (p.x - 2);
		mpu ^= bit;
		setmpuio(hWnd, mpu);
		redraw = TRUE;
	}
	else if ((p.x >= 9) && (p.x < 13)) {
		bit = (UINT8)(12 - p.x);
		if ((mpu ^ bit) & 3) {
			mpu &= ~0x3;
			mpu |= bit;
			setmpuint(hWnd, mpu);
			redraw = TRUE;
		}
	}
	if (redraw) {
		InvalidateRect(GetDlgItem(hWnd, IDC_MPUDIP), NULL, TRUE);
	}
}

LRESULT CALLBACK MidiDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_INITDIALOG:
			mpucreate(hWnd);
			return(FALSE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDOK:
					mpuupdate(hWnd);
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

				case IDC_MPUIO:
					setmpujmp(hWnd, getmpuio(hWnd), 0xf0);
					return(FALSE);

				case IDC_MPUINT:
					setmpujmp(hWnd, getmpuint(hWnd), 0x03);
					return(FALSE);

				case IDC_MPUDEF:
					mpu = 0x82;
					setmpuio(hWnd, mpu);
					setmpuint(hWnd, mpu);
					InvalidateRect(GetDlgItem(hWnd, IDC_MPUDIP), NULL, TRUE);
					return(FALSE);

				case IDC_MPUDIP:
					mpucmddipsw(hWnd);
					return(FALSE);

				case IDC_MPU98DEFB:
					dlgs_browsemimpidef(hWnd, IDC_MPU98DEFF);
					return(FALSE);
			}
			break;

		case WM_DRAWITEM:
			if (LOWORD(wp) == IDC_MPUDIP) {
				dlgs_drawbmp(((LPDRAWITEMSTRUCT)lp)->hDC,
													dipswbmp_getmpu(mpu));
			}
			return(FALSE);

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
			break;

		default:
			return(FALSE);
	}
	return(TRUE);
}

