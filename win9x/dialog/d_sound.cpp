/**
 * @file	d_sound.cpp
 * @brief	Sound configure dialog procedure
 *
 * @author	$Author: yui $
 * @date	$Date: 2011/03/07 09:54:11 $
 */

#include "compiler.h"
#include <commctrl.h>
#include <prsht.h>
#include "strres.h"
#include "resource.h"
#include "np2.h"
#include "oemtext.h"
#include "dosio.h"
#include "joymng.h"
#include "sysmng.h"
#include "menu.h"
#include "np2class.h"
#include "dialog.h"
#include "dialogs.h"
#include "pccore.h"
#include "iocore.h"
#include "sound.h"
#include "fmboard.h"
#include "s98.h"
#include "dipswbmp.h"

#if !defined(__GNUC__)
#pragma comment(lib, "comctl32.lib")
#endif	// !defined(__GNUC__)

static const CBPARAM cpIO26[] =
{
	{MAKEINTRESOURCE(IDS_0088),		0x00},
	{MAKEINTRESOURCE(IDS_0188),		0x10},
};

static const CBPARAM cpIO86[] =
{
	{MAKEINTRESOURCE(IDS_0188),		0x01},
	{MAKEINTRESOURCE(IDS_0288),		0x00},
};

static const CBPARAM cpInt26[] =
{
	{MAKEINTRESOURCE(IDS_INT0),		0x00},
	{MAKEINTRESOURCE(IDS_INT41),	0x80},
	{MAKEINTRESOURCE(IDS_INT5),		0xc0},
	{MAKEINTRESOURCE(IDS_INT6),		0x40},
};

static const CBPARAM cpInt86[] =
{
	{MAKEINTRESOURCE(IDS_INT0),		0x00},
	{MAKEINTRESOURCE(IDS_INT41),	0x08},
	{MAKEINTRESOURCE(IDS_INT5),		0x0c},
	{MAKEINTRESOURCE(IDS_INT6),		0x04},
};

static const CBPARAM cpAddr[] =
{
	{MAKEINTRESOURCE(IDS_C8000),		0x00},
	{MAKEINTRESOURCE(IDS_CC000),		0x01},
	{MAKEINTRESOURCE(IDS_D0000),		0x02},
	{MAKEINTRESOURCE(IDS_D4000),		0x03},
	{MAKEINTRESOURCE(IDS_NONCONNECT),	0x04},
};

static const CBPARAM cpID[] =
{
	{MAKEINTRESOURCE(IDS_0X),	0xe0},
	{MAKEINTRESOURCE(IDS_1X),	0xc0},
	{MAKEINTRESOURCE(IDS_2X),	0xa0},
	{MAKEINTRESOURCE(IDS_3X),	0x80},
	{MAKEINTRESOURCE(IDS_4X),	0x60},
	{MAKEINTRESOURCE(IDS_5X),	0x40},
	{MAKEINTRESOURCE(IDS_6X),	0x20},
	{MAKEINTRESOURCE(IDS_7X),	0x00},
};


typedef struct {
	UINT16	res;
	UINT16	resstr;
	UINT8	*value;
	UINT16	min;
	UINT16	max;
} SLIDERTBL;

static void slidersetvaluestr(HWND hWnd, const SLIDERTBL *item, UINT8 value) {

	TCHAR	work[32];

	wsprintf(work, tchar_d, value);
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

static void setsnd26io(HWND hWnd, UINT uID, UINT8 cValue)
{
	dlgs_setcbcur(hWnd, uID, cValue & 0x10);
}

static UINT8 getsnd26io(HWND hWnd, UINT uID)
{
	return dlgs_getcbcur(hWnd, uID, 0x10);
}

static void setsnd26int(HWND hWnd, UINT uID, UINT8 cValue)
{
	dlgs_setcbcur(hWnd, uID, cValue & 0xc0);
}

static UINT8 getsnd26int(HWND hWnd, UINT uID)
{
	return dlgs_getcbcur(hWnd, uID, 0xc0);
}

static void setsnd26rom(HWND hWnd, UINT uID, UINT8 cValue)
{
	UINT	uParam;

	uParam = cValue & 0x07;
	uParam = min(uParam, 0x04);
	dlgs_setcbcur(hWnd, uID, uParam);
}

static UINT8 getsnd26rom(HWND hWnd, UINT uID)
{
	return dlgs_getcbcur(hWnd, uID, 0x04);
}


// ---- PC-9801-26

static	UINT8	snd26 = 0;

static void snd26set(HWND hWnd, UINT8 cValue)
{
	setsnd26io(hWnd, IDC_SND26IO, cValue);
	setsnd26int(hWnd, IDC_SND26INT, cValue);
	setsnd26rom(hWnd, IDC_SND26ROM, cValue);
}

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
			setsnd26rom(hWnd, IDC_SND26ROM, b);
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
			setsnd26int(hWnd, IDC_SND26INT, b);
			redraw = TRUE;
		}
	}
	else if ((p.x >= 15) && (p.x < 17)) {
		b = (UINT8)((p.x - 15) << 4);
		if ((snd26 ^ b) & 0x10) {
			snd26 &= ~0x10;
			snd26 |= b;
			setsnd26io(hWnd, IDC_SND26IO, b);
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
			dlgs_setcbitem(hWnd, IDC_SND26IO, cpIO26, NELEMENTS(cpIO26));
			dlgs_setcbitem(hWnd, IDC_SND26INT, cpInt26, NELEMENTS(cpInt26));
			dlgs_setcbitem(hWnd, IDC_SND26ROM, cpAddr, NELEMENTS(cpAddr));
			snd26set(hWnd, snd26);
			sub = GetDlgItem(hWnd, IDC_SND26JMP);
			SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
							(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));
			return(TRUE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
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
					snd26set(hWnd, snd26);
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
			break;
	}
	return(FALSE);
}


// ---- PC-9801-86

static	UINT8	snd86 = 0;

static void setsnd86io(HWND hWnd, UINT uID, UINT8 cValue)
{
	dlgs_setcbcur(hWnd, uID, cValue & 0x01);
}

static UINT8 getsnd86io(HWND hWnd, UINT uID)
{
	return dlgs_getcbcur(hWnd, uID, 0x01);
}

static void setsnd86int(HWND hWnd, UINT uID, UINT8 cValue)
{
	dlgs_setcbcur(hWnd, uID, cValue & 0x0c);
}

static UINT8 getsnd86int(HWND hWnd, INT uID)
{
	return dlgs_getcbcur(hWnd, uID, 0x0c);
}

static void setsnd86id(HWND hWnd, UINT uID, UINT8 cValue)
{
	dlgs_setcbcur(hWnd, uID, cValue & 0xe0);
}

static UINT8 getsnd86id(HWND hWnd, UINT uID)
{
	return dlgs_getcbcur(hWnd, uID, 0x00);
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
			setsnd86io(hWnd, IDC_SND86IO, snd86);
			break;

		case 1:
			SetDlgItemCheck(hWnd, IDC_SND86ROM, snd86 & 2);
			break;

		case 2:
		case 3:
			setsnd86int(hWnd, IDC_SND86INTA, snd86);
			break;

		case 4:
			SetDlgItemCheck(hWnd, IDC_SND86INT, snd86 & 0x10);
			break;

		case 5:
		case 6:
		case 7:
			setsnd86id(hWnd, IDC_SND86ID, snd86);
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
			dlgs_setcbitem(hWnd, IDC_SND86IO, cpIO86, NELEMENTS(cpIO86));
			setsnd86io(hWnd, IDC_SND86IO, snd86);
			SetDlgItemCheck(hWnd, IDC_SND86INT, snd86 & 0x10);
			dlgs_setcbitem(hWnd, IDC_SND86INTA, cpInt86, NELEMENTS(cpInt86));
			setsnd86int(hWnd, IDC_SND86INTA, snd86);
			dlgs_setcbitem(hWnd, IDC_SND86ID, cpID, NELEMENTS(cpID));
			setsnd86id(hWnd, IDC_SND86ID, snd86);
			SetDlgItemCheck(hWnd, IDC_SND86ROM, snd86 & 2);

			sub = GetDlgItem(hWnd, IDC_SND86DIP);
			SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
							(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));
			return(TRUE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDC_SND86IO:
					set86jmp(hWnd, getsnd86io(hWnd, IDC_SND86IO), 0x01);
					break;

				case IDC_SND86INT:
					set86jmp(hWnd,
							(GetDlgItemCheck(hWnd, IDC_SND86INT))?0x10:0x00,
																		0x10);
					break;

				case IDC_SND86INTA:
					set86jmp(hWnd, getsnd86int(hWnd, IDC_SND86INTA), 0x0c);
					break;

				case IDC_SND86ROM:
					set86jmp(hWnd,
							(GetDlgItemCheck(hWnd, IDC_SND86ROM))?0x02:0x00,
																		0x02);
					break;

				case IDC_SND86ID:
					set86jmp(hWnd, getsnd86id(hWnd, IDC_SND86ID), 0xe0);
					break;

				case IDC_SND86DEF:
					snd86 = 0x7f;
					setsnd86io(hWnd, IDC_SND86IO, snd86);
					SetDlgItemCheck(hWnd, IDC_SND86INT, TRUE);
					setsnd86int(hWnd, IDC_SND86INTA, snd86);
					setsnd86id(hWnd, IDC_SND86ID, snd86);
					SetDlgItemCheck(hWnd, IDC_SND86ROM, TRUE);
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

	SetDlgItemCheck(hWnd, IDC_SPBVRL, spbvrc & 1);
	SetDlgItemCheck(hWnd, IDC_SPBVRR, spbvrc & 2);
}

static void spbcreate(HWND hWnd)
{
	HWND	sub;

	spb = np2cfg.spbopt;

	dlgs_setcbitem(hWnd, IDC_SPBIO, cpIO26, NELEMENTS(cpIO26));
	setsnd26io(hWnd, IDC_SPBIO, spb);
	dlgs_setcbitem(hWnd, IDC_SPBINT, cpInt26, NELEMENTS(cpInt26));
	setsnd26int(hWnd, IDC_SPBINT, spb);
	dlgs_setcbitem(hWnd, IDC_SPBROM, cpAddr, NELEMENTS(cpAddr));
	setsnd26rom(hWnd, IDC_SPBROM, spb);
	spbvrc = np2cfg.spb_vrc;								// ver0.30
	setspbVRch(hWnd);
	SendDlgItemMessage(hWnd, IDC_SPBVRLEVEL, TBM_SETRANGE, TRUE,
															MAKELONG(0, 24));
	SendDlgItemMessage(hWnd, IDC_SPBVRLEVEL, TBM_SETPOS, TRUE,
															np2cfg.spb_vrl);
	SetDlgItemCheck(hWnd, IDC_SPBREVERSE, np2cfg.spb_x);

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
			setsnd26int(hWnd, IDC_SPBINT, b);
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
			setsnd26io(hWnd, IDC_SPBIO, b);
			redraw = TRUE;
		}
	}
	else if ((p.x >= 14) && (p.x < 19)) {
		b = (UINT8)(p.x - 14);
		if ((spb ^ b) & 7) {
			spb &= ~0x07;
			spb |= b;
			setsnd26rom(hWnd, IDC_SPBROM, b);
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
	if (GetDlgItemCheck(hWnd, IDC_SPBVRL)) {
		ret += 1;
	}
	if (GetDlgItemCheck(hWnd, IDC_SPBVRR)) {
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
					setsnd26io(hWnd, IDC_SPBIO, spb);
					setsnd26int(hWnd, IDC_SPBINT, spb);
					setsnd26rom(hWnd, IDC_SPBROM, spb);
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
				b = (UINT8)GetDlgItemCheck(hWnd, IDC_SPBREVERSE);
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

	SetDlgItemCheck(hWnd, item->res, (*(item->ptr)) & (1 << (item->bit)));
}

static UINT8 checkbtnres_store(HWND hWnd, const CHECKTBL *item) {

	UINT8	value;
	UINT8	bit;
	UINT8	ret;

	bit = 1 << (item->bit);
	value = GetDlgItemCheck(hWnd, item->res)?bit:0;
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
				EnableWindow(GetDlgItem(hWnd, IDC_JOYPAD1), FALSE);
			}
			return(TRUE);

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				renewal = 0;
				for (i=0; i<13; i++) {
					renewal |= checkbtnres_store(hWnd, pad1opt + i);
				}
				if (renewal) {
					joymng_initialize();
					sysmng_update(SYS_UPDATECFG);
				}
				return(TRUE);
			}
			break;
	}
	return(FALSE);
}


// ----

void dialog_sndopt(HWND hWnd)
{
	HINSTANCE		hInstance;
	PROPSHEETPAGE	psp;
	PROPSHEETHEADER	psh;
	HPROPSHEETPAGE	hpsp[6];
	TCHAR			szTitle[128];

	hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

	ZeroMemory(&psp, sizeof(psp));
	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.dwFlags = PSP_DEFAULT;
	psp.hInstance = hInstance;

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

	loadstringresource(IDS_SOUNDOPTION, szTitle, NELEMENTS(szTitle));

	ZeroMemory(&psh, sizeof(psh));
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
	psh.hwndParent = hWnd;
	psh.hInstance = hInstance;
	psh.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	psh.nPages = 6;
	psh.phpage = hpsp;
	psh.pszCaption = szTitle;
	psh.pfnCallback = np2class_propetysheet;
	PropertySheet(&psh);
	InvalidateRect(hWnd, NULL, TRUE);
}


// ----

#if defined(SUPPORT_S98)

static const FSPARAM fpS98 =
{
	MAKEINTRESOURCE(IDS_S98TITLE),
	MAKEINTRESOURCE(IDS_S98EXT),
	MAKEINTRESOURCE(IDS_S98FILTER),
	1
};
static const OEMCHAR szS98File[] = OEMTEXT("NP2_####.S98");

void dialog_s98(HWND hWnd)
{
	BOOL	bCheck;
	OEMCHAR	szPath[MAX_PATH];

	S98_close();
	bCheck = FALSE;
	file_cpyname(szPath, bmpfilefolder, NELEMENTS(szPath));
	file_cutname(szPath);
	file_catname(szPath, szS98File, NELEMENTS(szPath));
	if ((dlgs_createfilenum(hWnd, &fpS98, szPath, NELEMENTS(szPath))) &&
		(S98_open(szPath) == SUCCESS))
	{
		file_cpyname(bmpfilefolder, szPath, NELEMENTS(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);
		bCheck = TRUE;
	}
	xmenu_sets98logging(bCheck);
}
#endif	// defined(SUPPORT_S98)


// ----

#if defined(SUPPORT_WAVEREC)

static const FSPARAM fpWave =
{
	MAKEINTRESOURCE(IDS_WAVETITLE),
	MAKEINTRESOURCE(IDS_WAVEEXT),
	MAKEINTRESOURCE(IDS_WAVEFILTER),
	1
};
static const OEMCHAR szWaveFile[] = OEMTEXT("NP2_####.WAV");

void dialog_waverec(HWND hWnd)
{
	UINT8	bCheck;
	OEMCHAR	szPath[MAX_PATH];

	bCheck = FALSE;
	sound_recstop();
	file_cpyname(szPath, bmpfilefolder, NELEMENTS(szPath));
	file_cutname(szPath);
	file_catname(szPath, szWaveFile, NELEMENTS(szPath));
	if ((dlgs_createfilenum(hWnd, &fpWave, szPath, NELEMENTS(szPath))) &&
		(sound_recstart(szPath) == SUCCESS))
	{
		file_cpyname(bmpfilefolder, szPath, NELEMENTS(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);
		bCheck = TRUE;
	}
	xmenu_setwaverec(bCheck);
}
#endif	// defined(SUPPORT_WAVEREC)

