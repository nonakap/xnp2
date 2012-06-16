#include	"compiler.h"
#include	<windowsx.h>
#include	<commctrl.h>
#include	<prsht.h>
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"dosio.h"
#include	"joymng.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"
#include	"dipswbmp.h"


static const TCHAR *sndioport[4] = {_T("0088"), _T("0188"),
									_T("0288"), _T("0388")};
static const TCHAR *sndinterrupt[4] =
								{str_int0, str_int4, str_int5, str_int6};
static const TCHAR *sndromaddr[5] =
								{_T("C8000"), _T("CC000"), _T("D0000"),
									_T("D4000"), _T("N/C")};
static const TCHAR *sndid[8] = {_T("0x"), _T("1x"), _T("2x"), _T("3x"),
								_T("4x"), _T("5x"), _T("6x"), _T("7x")};

static const TCHAR str_sndopt[] = _T("Sound board option");


typedef struct {
	UINT16	res;
	UINT16	resstr;
	UINT8	*value;
	UINT16	min;
	UINT16	max;
} SLIDERTBL;

static void slidersetvaluestr(HWND hWnd, const SLIDERTBL *item, UINT8 value) {

	TCHAR	work[32];

	wsprintf(work, str_d, value);
	SetDlgItemText(hWnd, item->resstr, work);
}

static void slidersetvalue(HWND hWnd, const SLIDERTBL *item, UINT8 value) {

	if (value > (UINT8)item->max) {
		value = (UINT8)item->max;
	}
	else if (value < (UINT8)item->min) {
		value = (UINT8)item->min;
	}
	SendDlgItemMessage(hWnd, item->res, TBM_SETPOS, TRUE, value);
	slidersetvaluestr(hWnd, item, value);
}

static void sliderinit(HWND hWnd, const SLIDERTBL *item) {

	SendDlgItemMessage(hWnd, item->res, TBM_SETRANGE, TRUE,
											MAKELONG(item->min, item->max));
	slidersetvalue(hWnd, item, *(item->value));
}

static void sliderresetpos(HWND hWnd, const SLIDERTBL *item) {

	UINT8	value;

	value = (UINT8)SendDlgItemMessage(hWnd, item->res, TBM_GETPOS, 0, 0);
	if (value > (UINT8)item->max) {
		value = (UINT8)item->max;
	}
	else if (value < (UINT8)item->min) {
		value = (UINT8)item->min;
	}
	slidersetvaluestr(hWnd, item, value);
}

static UINT8 sliderrestore(HWND hWnd, const SLIDERTBL *item) {

	UINT8	value;
	UINT8	ret;

	value = (UINT8)SendDlgItemMessage(hWnd, item->res, TBM_GETPOS, 0, 0);
	if (value > (UINT8)item->max) {
		value = (UINT8)item->max;
	}
	else if (value < (UINT8)item->min) {
		value = (UINT8)item->min;
	}
	ret = (*(item->value)) - value;
	if (ret) {
		*(item->value) = value;
	}
	return(ret);
}

// ---- mixer

static const SLIDERTBL sndmixitem[] = {
		{IDC_VOLFM,		IDC_VOLFMSTR,		&np2cfg.vol_fm,		0,128},
		{IDC_VOLPSG,	IDC_VOLPSGSTR,		&np2cfg.vol_ssg,	0,128},
		{IDC_VOLADPCM,	IDC_VOLADPCMSTR,	&np2cfg.vol_adpcm,	0,128},
		{IDC_VOLPCM,	IDC_VOLPCMSTR,		&np2cfg.vol_pcm,	0,128},
		{IDC_VOLRHYTHM,	IDC_VOLRHYTHMSTR,	&np2cfg.vol_rhythm,	0,128}};

static LRESULT CALLBACK SndmixDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	int		i;
	int		ctrlid;

	switch (msg) {
		case WM_INITDIALOG:
			for (i=0; i<5; i++) {
				sliderinit(hWnd, &sndmixitem[i]);
			}
			return(TRUE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDC_SNDMIXDEF:
					for (i=0; i<5; i++) {
						slidersetvalue(hWnd, &sndmixitem[i], 64);
					}
					break;
			}
			break;

		case WM_HSCROLL:
			ctrlid = GetDlgCtrlID((HWND)lp);
			for (i=0; i<5; i++) {
				if (ctrlid == sndmixitem[i].res) {
					sliderresetpos(hWnd, &sndmixitem[i]);
					return(TRUE);
				}
			}
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				for (i=0; i<5; i++) {
					if (sliderrestore(hWnd, &sndmixitem[i])) {
						sysmng_update(SYS_UPDATECFG);
					}
				}
				opngen_setvol(np2cfg.vol_fm);
				psggen_setvol(np2cfg.vol_ssg);
				rhythm_setvol(np2cfg.vol_rhythm);
				rhythm_update(&rhythm);
				adpcm_setvol(np2cfg.vol_adpcm);
				adpcm_update(&adpcm);
				pcm86gen_setvol(np2cfg.vol_pcm);
				pcm86gen_update();
				return(TRUE);
			}
			break;
	}
	return(FALSE);
}


// ---- PC-9801-14

static const SLIDERTBL snd14item[] = {
		{IDC_VOL14L,	IDC_VOL14LSTR,		np2cfg.vol14+0,		0,15},
		{IDC_VOL14R,	IDC_VOL14RSTR,		np2cfg.vol14+1,		0,15},
		{IDC_VOLF2,		IDC_VOLF2STR,		np2cfg.vol14+2,		0,15},
		{IDC_VOLF4,		IDC_VOLF4STR,		np2cfg.vol14+3,		0,15},
		{IDC_VOLF8,		IDC_VOLF8STR,		np2cfg.vol14+4,		0,15},
		{IDC_VOLF16,	IDC_VOLF16STR,		np2cfg.vol14+5,		0,15}};

static LRESULT CALLBACK Snd14optDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	int		i;
	int		ctrlid;

	switch (msg) {
		case WM_INITDIALOG:
			for (i=0; i<6; i++) {
				sliderinit(hWnd, &snd14item[i]);
			}
			return(TRUE);

		case WM_HSCROLL:
			ctrlid = GetDlgCtrlID((HWND)lp);
			for (i=0; i<6; i++) {
				if (ctrlid == snd14item[i].res) {
					sliderresetpos(hWnd, &snd14item[i]);
					return(TRUE);
				}
			}
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				for (i=0; i<6; i++) {
					if (sliderrestore(hWnd, &snd14item[i])) {
						sysmng_update(SYS_UPDATECFG);
					}
				}
				tms3631_setvol(np2cfg.vol14);
				return(TRUE);
			}
			break;
	}
	return(FALSE);
}


// ---- 26K, SPB jumper

static const UINT snd26paranum[4] = {0, 3, 1, 2};

static void setsnd26iopara(HWND hWnd, UINT8 value) {

	SendMessage(hWnd, CB_SETCURSEL, (WPARAM)((value >> 4) & 1), (LPARAM)0);
}

static UINT8 getsnd26io(HWND hWnd, UINT16 res) {

	TCHAR	work[8];

	GetDlgItemText(hWnd, res, work, NELEMENTS(work));
	return((UINT8)((work[1] == '1')?0x10:0x00));
}

static void setsnd26intpara(HWND hWnd, UINT8 value) {

	SendMessage(hWnd, CB_SETCURSEL,
						(WPARAM)snd26paranum[(value >> 6) & 3], (LPARAM)0);
}

static UINT8 getsnd26int(HWND hWnd, UINT16 res) {

	TCHAR	work[8];

	GetDlgItemText(hWnd, res, work, NELEMENTS(work));
	switch(work[3]) {
		case '0':
			return(0x00);

		case '4':
			return(0x80);

		case '6':
			return(0x40);
	}
	return(0xc0);
}

static void setsnd26rompara(HWND hWnd, UINT8 value) {

	int		para;

	para = value & 7;
	if (para > 4) {
		para = 4;
	}
	SendMessage(hWnd, CB_SETCURSEL, (WPARAM)para, (LPARAM)0);
}

static UINT8 getsnd26rom(HWND hWnd, UINT16 res) {

	TCHAR	work[8];
	UINT32	adrs;

	GetDlgItemText(hWnd, res, work, NELEMENTS(work));
	adrs = ((UINT32)milstr_solveHEX(work) - 0xc8000) >> 14;
	if (adrs < 4) {
		return((UINT8)adrs);
	}
	return(4);
}


// ---- PC-9801-26

static	UINT8	snd26 = 0;


static void set26jmp(HWND hWnd, UINT8 value, UINT8 bit) {

	if ((snd26 ^ value) & bit) {
		snd26 &= ~bit;
		snd26 |= value;
		InvalidateRect(GetDlgItem(hWnd, IDC_SND26JMP), NULL, TRUE);
	}
}

static void snd26cmdjmp(HWND hWnd) {

	RECT	rect1;
	RECT	rect2;
	POINT	p;
	BOOL	redraw;
	UINT8	b;
	UINT8	bit;

	GetWindowRect(GetDlgItem(hWnd, IDC_SND26JMP), &rect1);
	GetClientRect(GetDlgItem(hWnd, IDC_SND26JMP), &rect2);
	GetCursorPos(&p);
	redraw = FALSE;
	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 9;
	p.y /= 9;
	if ((p.y < 1) || (p.y >= 3)) {
		return;
	}
	if ((p.x >= 2) && (p.x < 7)) {
		b = (UINT8)(p.x - 2);
		if ((snd26 ^ b) & 7) {
			snd26 &= ~0x07;
			snd26 |= b;
			setsnd26rompara(GetDlgItem(hWnd, IDC_SND26ROM), b);
			redraw = TRUE;
		}
	}
	else if ((p.x >= 9) && (p.x < 12)) {
		b = snd26;
		bit = 0x40 << (2 - p.y);
		switch(p.x) {
			case 9:
				b |= bit;
				break;

			case 10:
				b ^= bit;
				break;

			case 11:
				b &= ~bit;
				break;
		}
		if (snd26 != b) {
			snd26 = b;
			setsnd26intpara(GetDlgItem(hWnd, IDC_SND26INT), b);
			redraw = TRUE;
		}
	}
	else if ((p.x >= 15) && (p.x < 17)) {
		b = (UINT8)((p.x - 15) << 4);
		if ((snd26 ^ b) & 0x10) {
			snd26 &= ~0x10;
			snd26 |= b;
			setsnd26iopara(GetDlgItem(hWnd, IDC_SND26IO), b);
			redraw = TRUE;
		}
	}
	if (redraw) {
		InvalidateRect(GetDlgItem(hWnd, IDC_SND26JMP), NULL, TRUE);
	}
}

static LRESULT CALLBACK Snd26optDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	HWND	sub;

	switch(msg) {
		case WM_INITDIALOG:
			snd26 = np2cfg.snd26opt;
			SETnLISTSTR(hWnd, IDC_SND26IO, sndioport, 2);
			setsnd26iopara(GetDlgItem(hWnd, IDC_SND26IO), snd26);
			SETLISTSTR(hWnd, IDC_SND26INT, sndinterrupt);
			setsnd26intpara(GetDlgItem(hWnd, IDC_SND26INT), snd26);
			SETLISTSTR(hWnd, IDC_SND26ROM, sndromaddr);
			setsnd26rompara(GetDlgItem(hWnd, IDC_SND26ROM), snd26);
			sub = GetDlgItem(hWnd, IDC_SND26JMP);
			SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
							(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));
			return(TRUE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDC_SND26IO:
					set26jmp(hWnd, getsnd26io(hWnd, IDC_SND26IO), 0x10);
					break;

				case IDC_SND26INT:
					set26jmp(hWnd, getsnd26int(hWnd, IDC_SND26INT), 0xc0);
					break;

				case IDC_SND26ROM:
					set26jmp(hWnd, getsnd26rom(hWnd, IDC_SND26ROM), 0x07);
					break;

				case IDC_SND26DEF:
					snd26 = 0xd1;
					setsnd26iopara(GetDlgItem(hWnd, IDC_SND26IO), snd26);
					setsnd26intpara(GetDlgItem(hWnd, IDC_SND26INT), snd26);
					setsnd26rompara(GetDlgItem(hWnd, IDC_SND26ROM), snd26);
					InvalidateRect(GetDlgItem(hWnd, IDC_SND26JMP), NULL, TRUE);
					break;

				case IDC_SND26JMP:
					snd26cmdjmp(hWnd);
					break;
			}
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				if (np2cfg.snd26opt != snd26) {
					np2cfg.snd26opt = snd26;
					sysmng_update(SYS_UPDATECFG);
				}
				return(TRUE);
			}
			break;

		case WM_DRAWITEM:
			if (LOWORD(wp) == IDC_SND26JMP) {
				dlgs_drawbmp(((LPDRAWITEMSTRUCT)lp)->hDC,
												dipswbmp_getsnd26(snd26));
			}
			return(FALSE);
	}
	return(FALSE);
}


// ---- PC-9801-86

static	UINT8	snd86 = 0;

static const UINT snd86paranum[4] = {0, 1, 3, 2};


static void setsnd86iopara(HWND hWnd, UINT8 value) {

	SendMessage(hWnd, CB_SETCURSEL, (WPARAM)((~value) & 1), (LPARAM)0);
}

static UINT8 getsnd86io(HWND hWnd, UINT16 res) {

	TCHAR	work[8];

	GetDlgItemText(hWnd, res, work, NELEMENTS(work));
	return((UINT8)((work[1] == '1')?0x01:0x00));
}

static void setsnd86intpara(HWND hWnd, UINT8 value) {

	SendMessage(hWnd, CB_SETCURSEL,
						(WPARAM)snd86paranum[(value >> 2) & 3], (LPARAM)0);
}

static UINT8 getsnd86int(HWND hWnd) {

	TCHAR	work[8];

	Edit_GetText(hWnd, work, NELEMENTS(work));
	switch(work[3]) {
		case '0':
			return(0x00);

		case '4':
			return(0x04);

		case '6':
			return(0x08);
	}
	return(0x0c);
}

static void setsnd86idpara(HWND hWnd, UINT8 value) {

	SendMessage(hWnd, CB_SETCURSEL, (WPARAM)(((~value) >> 5) & 7), (LPARAM)0);
}

static UINT8 getsnd86id(HWND hWnd) {

	TCHAR	work[8];

	Edit_GetText(hWnd, work, NELEMENTS(work));
	return((~work[0] & 7) << 5);
}

static void set86jmp(HWND hWnd, UINT8 value, UINT8 bit) {

	if ((snd86 ^ value) & bit) {
		snd86 &= ~bit;
		snd86 |= value;
		InvalidateRect(GetDlgItem(hWnd, IDC_SND86DIP), NULL, TRUE);
	}
}

static void snd86cmddipsw(HWND hWnd) {

	RECT	rect1;
	RECT	rect2;
	POINT	p;

	GetWindowRect(GetDlgItem(hWnd, IDC_SND86DIP), &rect1);
	GetClientRect(GetDlgItem(hWnd, IDC_SND86DIP), &rect2);
	GetCursorPos(&p);
	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 8;
	p.y /= 8;
	if ((p.x < 2) || (p.x >= 10) ||
		(p.y < 1) || (p.y >= 3)) {
		return;
	}
	p.x -= 2;
	snd86 ^= (1 << p.x);
	switch(p.x) {
		case 0:
			setsnd86iopara(GetDlgItem(hWnd, IDC_SND86IO), snd86);
			break;

		case 1:
			Button_SetCheck(GetDlgItem(hWnd, IDC_SND86ROM), snd86 & 2);
			break;

		case 2:
		case 3:
			setsnd86intpara(GetDlgItem(hWnd, IDC_SND86INTA), snd86);
			break;

		case 4:
			Button_SetCheck(GetDlgItem(hWnd, IDC_SND86INT), snd86 & 0x10);
			break;

		case 5:
		case 6:
		case 7:
			setsnd86idpara(GetDlgItem(hWnd, IDC_SND86ID), snd86);
			break;
	}
	InvalidateRect(GetDlgItem(hWnd, IDC_SND86DIP), NULL, TRUE);
}

static LRESULT CALLBACK Snd86optDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	HWND	sub;

	switch(msg) {
		case WM_INITDIALOG:
			snd86 = np2cfg.snd86opt;
			SETnLISTSTR(hWnd, IDC_SND86IO, sndioport+1, 2);
			setsnd86iopara(GetDlgItem(hWnd, IDC_SND86IO), snd86);
			Button_SetCheck(GetDlgItem(hWnd, IDC_SND86INT), snd86 & 0x10);
			SETLISTSTR(hWnd, IDC_SND86INTA, sndinterrupt);
			setsnd86intpara(GetDlgItem(hWnd, IDC_SND86INTA), snd86);
			SETLISTSTR(hWnd, IDC_SND86ID, sndid);
			setsnd86idpara(GetDlgItem(hWnd, IDC_SND86ID), snd86);
			Button_SetCheck(GetDlgItem(hWnd, IDC_SND86ROM), snd86 & 2);

			sub = GetDlgItem(hWnd, IDC_SND86DIP);
			SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
							(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));
			return(TRUE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDC_SND86IO:
					set86jmp(hWnd, getsnd86io(hWnd, IDC_SND86IO), 0x01);
					break;
				case IDC_SND86INT:
					set86jmp(hWnd,
						((Button_GetCheck(GetDlgItem(hWnd, IDC_SND86INT)))
														?0x10:0x00), 0x10);
					break;
				case IDC_SND86INTA:
					set86jmp(hWnd,
						getsnd86int(GetDlgItem(hWnd, IDC_SND86INTA)), 0x0c);
					break;
				case IDC_SND86ROM:
					set86jmp(hWnd,
						((Button_GetCheck(GetDlgItem(hWnd, IDC_SND86ROM)))
														?0x02:0x00), 0x02);
					break;
				case IDC_SND86ID:
					set86jmp(hWnd,
						getsnd86id(GetDlgItem(hWnd, IDC_SND86ID)), 0xe0);
					break;

				case IDC_SND86DEF:
					snd86 = 0x7f;
					setsnd86iopara(GetDlgItem(hWnd, IDC_SND86IO), snd86);
					Button_SetCheck(GetDlgItem(hWnd, IDC_SND86INT), TRUE);
					setsnd86intpara(GetDlgItem(hWnd, IDC_SND86INTA), snd86);
					setsnd86idpara(GetDlgItem(hWnd, IDC_SND86ID), snd86);
					Button_SetCheck(GetDlgItem(hWnd, IDC_SND86ROM), TRUE);
					InvalidateRect(GetDlgItem(hWnd, IDC_SND86DIP), NULL, TRUE);
					break;

				case IDC_SND86DIP:
					snd86cmddipsw(hWnd);
					break;
			}
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				if (np2cfg.snd86opt != snd86) {
					np2cfg.snd86opt = snd86;
					sysmng_update(SYS_UPDATECFG);
				}
				return(TRUE);
			}
			break;

		case WM_DRAWITEM:
			if (LOWORD(wp) == IDC_SND86DIP) {
				dlgs_drawbmp(((LPDRAWITEMSTRUCT)lp)->hDC,
												dipswbmp_getsnd86(snd86));
			}
			return(FALSE);
	}
	return(FALSE);
}


// ---- Speak board

static	UINT8	spb = 0;
static	UINT8	spbvrc = 0;


static void setspbVRch(HWND hWnd) {

	Button_SetCheck(GetDlgItem(hWnd, IDC_SPBVRL), spbvrc & 1);
	Button_SetCheck(GetDlgItem(hWnd, IDC_SPBVRR), spbvrc & 2);
}

static void spbcreate(HWND hWnd) {

	HWND	sub;

	spb = np2cfg.spbopt;
	SETnLISTSTR(hWnd, IDC_SPBIO, sndioport, 2);
	setsnd26iopara(GetDlgItem(hWnd, IDC_SPBIO), spb);
	SETLISTSTR(hWnd, IDC_SPBINT, sndinterrupt);
	setsnd26intpara(GetDlgItem(hWnd, IDC_SPBINT), spb);
	SETLISTSTR(hWnd, IDC_SPBROM, sndromaddr);
	setsnd26rompara(GetDlgItem(hWnd, IDC_SPBROM), spb);
	spbvrc = np2cfg.spb_vrc;								// ver0.30
	setspbVRch(hWnd);
	SendDlgItemMessage(hWnd, IDC_SPBVRLEVEL, TBM_SETRANGE, TRUE,
															MAKELONG(0, 24));
	SendDlgItemMessage(hWnd, IDC_SPBVRLEVEL, TBM_SETPOS, TRUE,
															np2cfg.spb_vrl);
	Button_SetCheck(GetDlgItem(hWnd, IDC_SPBREVERSE), np2cfg.spb_x);

	sub = GetDlgItem(hWnd, IDC_SPBJMP);
	SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
							(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));
}

static void spbcmdjmp(HWND hWnd) {

	RECT	rect1;
	RECT	rect2;
	POINT	p;
	BOOL	redraw;
	UINT8	b;
	UINT8	bit;

	GetWindowRect(GetDlgItem(hWnd, IDC_SPBJMP), &rect1);
	GetClientRect(GetDlgItem(hWnd, IDC_SPBJMP), &rect2);
	GetCursorPos(&p);
	redraw = FALSE;
	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 9;
	p.y /= 9;
	if ((p.y < 1) || (p.y >= 3)) {
		return;
	}
	if ((p.x >= 2) && (p.x < 5)) {
		b = spb;
		bit = 0x40 << (2 - p.y);
		switch(p.x) {
			case 2:
				b |= bit;
				break;

			case 3:
				b ^= bit;
				break;

			case 4:
				b &= ~bit;
				break;
		}
		if (spb != b) {
			spb = b;
			setsnd26intpara(GetDlgItem(hWnd, IDC_SPBINT), b);
			redraw = TRUE;
		}
	}
	else if (p.x == 7) {
		spb ^= 0x20;
		redraw = TRUE;
	}
	else if ((p.x >= 10) && (p.x < 12)) {
		b = (UINT8)((p.x - 10) << 4);
		if ((spb ^ b) & 0x10) {
			spb &= ~0x10;
			spb |= b;
			setsnd26iopara(GetDlgItem(hWnd, IDC_SPBIO), b);
			redraw = TRUE;
		}
	}
	else if ((p.x >= 14) && (p.x < 19)) {
		b = (UINT8)(p.x - 14);
		if ((spb ^ b) & 7) {
			spb &= ~0x07;
			spb |= b;
			setsnd26rompara(GetDlgItem(hWnd, IDC_SPBROM), b);
			redraw = TRUE;
		}
	}
	else if ((p.x >= 21) && (p.x < 24)) {
		spbvrc ^= (UINT8)(3 - p.y);
		setspbVRch(hWnd);
		redraw = TRUE;
	}
	if (redraw) {
		InvalidateRect(GetDlgItem(hWnd, IDC_SPBJMP), NULL, TRUE);
	}
}

static void setspbjmp(HWND hWnd, UINT8 value, UINT8 bit) {

	if ((spb ^ value) & bit) {
		spb &= ~bit;
		spb |= value;
		InvalidateRect(GetDlgItem(hWnd, IDC_SPBJMP), NULL, TRUE);
	}
}

static UINT8 getspbVRch(HWND hWnd) {

	UINT8	ret;

	ret = 0;
	if (Button_GetCheck(GetDlgItem(hWnd, IDC_SPBVRL))) {
		ret++;
	}
	if (Button_GetCheck(GetDlgItem(hWnd, IDC_SPBVRR))) {
		ret += 2;
	}
	return(ret);
}

static LRESULT CALLBACK SPBoptDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {
	UINT8	b;
	UINT	update;

	switch(msg) {
		case WM_INITDIALOG:
			spbcreate(hWnd);
			return(TRUE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDC_SPBIO:
					setspbjmp(hWnd, getsnd26io(hWnd, IDC_SPBIO), 0x10);
					break;

				case IDC_SPBINT:
					setspbjmp(hWnd, getsnd26int(hWnd, IDC_SPBINT), 0xc0);
					break;

				case IDC_SPBROM:
					setspbjmp(hWnd, getsnd26rom(hWnd, IDC_SPBROM), 0x07);
					break;

				case IDC_SPBDEF:
					spb = 0xd1;
					setsnd26iopara(GetDlgItem(hWnd, IDC_SPBIO), spb);
					setsnd26intpara(GetDlgItem(hWnd, IDC_SPBINT), spb);
					setsnd26rompara(GetDlgItem(hWnd, IDC_SPBROM), spb);
					spbvrc = 0;
					setspbVRch(hWnd);
					InvalidateRect(GetDlgItem(hWnd, IDC_SPBJMP), NULL, TRUE);
					break;

				case IDC_SPBVRL:
				case IDC_SPBVRR:
					b = getspbVRch(hWnd);
					if ((spbvrc ^ b) & 3) {
						spbvrc = b;
						InvalidateRect(GetDlgItem(hWnd, IDC_SPBJMP),
																NULL, TRUE);
					}
					break;

				case IDC_SPBJMP:
					spbcmdjmp(hWnd);
					break;
			}
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				update = 0;
				if (np2cfg.spbopt != spb) {
					np2cfg.spbopt = spb;
					update |= SYS_UPDATECFG;
				}
				if (np2cfg.spb_vrc != spbvrc) {
					np2cfg.spb_vrc = spbvrc;
					update |= SYS_UPDATECFG;
				}
				b = (UINT8)SendDlgItemMessage(hWnd, IDC_SPBVRLEVEL,
															TBM_GETPOS, 0, 0);
				if (np2cfg.spb_vrl != b) {
					np2cfg.spb_vrl = b;
					update |= SYS_UPDATECFG;
				}
				opngen_setVR(np2cfg.spb_vrc, np2cfg.spb_vrl);
				b = (UINT8)(Button_GetCheck(GetDlgItem(hWnd, IDC_SPBREVERSE))
																		?1:0);
				if (np2cfg.spb_x != b) {
					np2cfg.spb_x = b;
					update |= SYS_UPDATECFG;
				}
				sysmng_update(update);
				return(TRUE);
			}
			break;

		case WM_DRAWITEM:
			if (LOWORD(wp) == IDC_SPBJMP) {
				dlgs_drawbmp(((LPDRAWITEMSTRUCT)lp)->hDC,
											dipswbmp_getsndspb(spb, spbvrc));
			}
			return(FALSE);
	}
	return(FALSE);
}


// ---- JOYPAD

typedef struct {
	UINT16	res;
	UINT16	bit;
	UINT8	*ptr;
} CHECKTBL;

static const CHECKTBL pad1opt[13] = {
			{IDC_JOYPAD1, 0, &np2oscfg.JOYPAD1},
			{IDC_PAD1_1A, 0, np2oscfg.JOY1BTN + 0},
			{IDC_PAD1_1B, 0, np2oscfg.JOY1BTN + 1},
			{IDC_PAD1_1C, 0, np2oscfg.JOY1BTN + 2},
			{IDC_PAD1_1D, 0, np2oscfg.JOY1BTN + 3},
			{IDC_PAD1_2A, 1, np2oscfg.JOY1BTN + 0},
			{IDC_PAD1_2B, 1, np2oscfg.JOY1BTN + 1},
			{IDC_PAD1_2C, 1, np2oscfg.JOY1BTN + 2},
			{IDC_PAD1_2D, 1, np2oscfg.JOY1BTN + 3},
			{IDC_PAD1_RA, 2, np2oscfg.JOY1BTN + 0},
			{IDC_PAD1_RB, 2, np2oscfg.JOY1BTN + 1},
			{IDC_PAD1_RC, 2, np2oscfg.JOY1BTN + 2},
			{IDC_PAD1_RD, 2, np2oscfg.JOY1BTN + 3}};


static void checkbtnres_load(HWND hWnd, const CHECKTBL *item) {

	Button_SetCheck(GetDlgItem(hWnd, item->res),
								(*(item->ptr)) & (1 << (item->bit)));
}

static UINT8 checkbtnres_store(HWND hWnd, const CHECKTBL *item) {

	UINT8	value;
	UINT8	bit;
	UINT8	ret;

	bit = 1 << (item->bit);
	value = ((Button_GetCheck(GetDlgItem(hWnd, item->res)))?0xff:0) & bit;
	ret = ((*(item->ptr)) ^ value) & bit;
	if (ret) {
		(*(item->ptr)) ^= bit;
	}
	return(ret);
}

static LRESULT CALLBACK PAD1optDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	int		i;
	UINT8	renewal;

	switch(msg) {
		case WM_INITDIALOG:
			for (i=0; i<13; i++) {
				checkbtnres_load(hWnd, pad1opt + i);
			}
			if (np2oscfg.JOYPAD1 & 2) {
				Button_Enable(GetDlgItem(hWnd, IDC_JOYPAD1), FALSE);
			}
			return(TRUE);

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				renewal = 0;
				for (i=0; i<13; i++) {
					renewal |= checkbtnres_store(hWnd, pad1opt + i);
				}
				if (renewal) {
					joy_init();
					sysmng_update(SYS_UPDATECFG);
				}
				return(TRUE);
			}
			break;
	}
	return(FALSE);
}


// ----

void dialog_sndopt(HWND hWnd) {

	HINSTANCE		hinst;
	PROPSHEETPAGE	psp;
	PROPSHEETHEADER	psh;
	HPROPSHEETPAGE	hpsp[6];										// ver0.29

	hinst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

	ZeroMemory(&psp, sizeof(psp));
	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.dwFlags = PSP_DEFAULT;
	psp.hInstance = hinst;

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SNDMIX);
	psp.pfnDlgProc = (DLGPROC)SndmixDlgProc;
	hpsp[0] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SND14);
	psp.pfnDlgProc = (DLGPROC)Snd14optDlgProc;
	hpsp[1] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SND26);
	psp.pfnDlgProc = (DLGPROC)Snd26optDlgProc;
	hpsp[2] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SND86);
	psp.pfnDlgProc = (DLGPROC)Snd86optDlgProc;
	hpsp[3] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SNDSPB);
	psp.pfnDlgProc = (DLGPROC)SPBoptDlgProc;
	hpsp[4] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SNDPAD1);
	psp.pfnDlgProc = (DLGPROC)PAD1optDlgProc;
	hpsp[5] = CreatePropertySheetPage(&psp);

	ZeroMemory(&psh, sizeof(psh));
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_NOAPPLYNOW;
	psh.hwndParent = hWnd;
	psh.hInstance = hinst;
	psh.nPages = 6;
	psh.phpage = hpsp;
	psh.pszCaption = str_sndopt;
	PropertySheet(&psh);
	InvalidateRect(hWnd, NULL, TRUE);
}


// ----

#if defined(SUPPORT_S98)
static const TCHAR s98ui_file[] = _T("NP2_%04d.S98");
static const TCHAR s98ui_title[] = _T("Save as S98 log");
static const TCHAR s98ui_ext[] = _T("s98");
static const TCHAR s98ui_filter[] = _T("S98 log (*.s98)\0*.s98\0");
static const FILESEL s98ui = {s98ui_title, s98ui_ext, s98ui_filter, 1};

void dialog_s98(HWND hWnd) {

	BOOL	check;
	TCHAR	path[MAX_PATH];

	S98_close();
	check = FALSE;
	file_cpyname(path, bmpfilefolder, NELEMENTS(path));
	file_cutname(path);
	file_catname(path, s98ui_file, NELEMENTS(path));
	if ((dlgs_selectwritenum(hWnd, &s98ui, path, NELEMENTS(path))) &&
		(S98_open(path) == SUCCESS)) {
		file_cpyname(bmpfilefolder, path, NELEMENTS(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);
		check = TRUE;
	}
	xmenu_sets98logging(check);
}
#endif


// ----

#if defined(SUPPORT_WAVEREC)
static const TCHAR wrui_file[] = _T("NP2_%04d.WAV");
static const TCHAR wrui_title[] = _T("Save as Sound");
static const TCHAR wrui_ext[] = _T("WAV");
static const TCHAR wrui_filter[] = _T("Wave files (*.wav)\0*.wav\0");
static const FILESEL wrui = {wrui_title, wrui_ext, wrui_filter, 1};

void dialog_waverec(HWND hWnd) {

	UINT8	check;
	TCHAR	path[MAX_PATH];

	check = FALSE;
	sound_recstop();
	file_cpyname(path, bmpfilefolder, NELEMENTS(path));
	file_cutname(path);
	file_catname(path, wrui_file, NELEMENTS(path));
	if ((dlgs_selectwritenum(hWnd, &wrui, path, NELEMENTS(path))) &&
		(sound_recstart(path) == SUCCESS)) {
		file_cpyname(bmpfilefolder, path, NELEMENTS(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);
		check = TRUE;
	}
	xmenu_setwaverec(check);
}
#endif

