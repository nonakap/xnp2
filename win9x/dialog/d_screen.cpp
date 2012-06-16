/**
 * @file	d_screen.cpp
 * @brief	Screen configure dialog procedure
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
#include "scrnmng.h"
#include "sysmng.h"
#include "np2class.h"
#include "dialog.h"
#include "dialogs.h"
#include "pccore.h"
#include "iocore.h"
#include "scrndraw.h"
#include "palettes.h"

#if !defined(__GNUC__)
#pragma comment(lib, "comctl32.lib")
#endif	// !defined(__GNUC__)

static LRESULT CALLBACK Scropt1DlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {
	TCHAR	work[32];
	UINT16	ret;
	UINT8	b;
	int		renewal;

	switch(msg) {
		case WM_INITDIALOG:
			SetDlgItemCheck(hWnd, IDC_LCD, np2cfg.LCD_MODE & 1);
			EnableWindow(GetDlgItem(hWnd, IDC_LCDX), np2cfg.LCD_MODE & 1);
			SetDlgItemCheck(hWnd, IDC_LCDX, np2cfg.LCD_MODE & 2);

			SetDlgItemCheck(hWnd, IDC_SKIPLINE, np2cfg.skipline);
			SendDlgItemMessage(hWnd, IDC_SKIPLIGHT, TBM_SETRANGE, TRUE,
											MAKELONG(0, 255));
			SendDlgItemMessage(hWnd, IDC_SKIPLIGHT, TBM_SETPOS, TRUE,
											np2cfg.skiplight);
			wsprintf(work, tchar_u, np2cfg.skiplight);
			SetDlgItemText(hWnd, IDC_LIGHTSTR, work);
			return(TRUE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDC_LCD:
					EnableWindow(GetDlgItem(hWnd, IDC_LCDX),
											GetDlgItemCheck(hWnd, IDC_LCD));
					break;
			}
			break;

		case WM_HSCROLL:
			switch(GetDlgCtrlID((HWND)lp)) {
				case IDC_SKIPLIGHT:
					ret = (UINT16)SendDlgItemMessage(hWnd, IDC_SKIPLIGHT,
													TBM_GETPOS, 0, 0);
					wsprintf(work, tchar_u, ret);
					SetDlgItemText(hWnd, IDC_LIGHTSTR, work);
					break;
			}
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				renewal = 0;
				b = (UINT8)GetDlgItemCheck(hWnd, IDC_SKIPLINE);
				if (np2cfg.skipline != b) {
					np2cfg.skipline = b;
					renewal = 1;
				}
				ret = (UINT16)SendDlgItemMessage(hWnd, IDC_SKIPLIGHT,
														TBM_GETPOS, 0, 0);
				if (ret > 255) {
					ret = 255;
				}
				if (np2cfg.skiplight != ret) {
					np2cfg.skiplight = ret;
					renewal = 1;
				}
				if (renewal) {
					pal_makeskiptable();
				}
				b = (GetDlgItemCheck(hWnd, IDC_LCD)?0x01:0x00) |
					(GetDlgItemCheck(hWnd, IDC_LCDX)?0x02:0x00);
				if (np2cfg.LCD_MODE != b) {
					np2cfg.LCD_MODE = b;
					pal_makelcdpal();
					renewal = 1;
				}
				if (renewal) {
					scrndraw_redraw();
					sysmng_update(SYS_UPDATECFG);
				}
				return(TRUE);
			}
			break;
	}
	return(FALSE);
}

static const int gdcchip[4] = {IDC_GRCGNON, IDC_GRCG, IDC_GRCG2, IDC_EGC};

static LRESULT CALLBACK Scropt2DlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	UINT8	b;
	UINT	update;

	switch(msg) {
		case WM_INITDIALOG:
			if (!np2cfg.uPD72020) {
				SetDlgItemCheck(hWnd, IDC_GDC7220, TRUE);
			}
			else {
				SetDlgItemCheck(hWnd, IDC_GDC72020, TRUE);
			}
			SetDlgItemCheck(hWnd, gdcchip[np2cfg.grcg & 3], TRUE);
			SetDlgItemCheck(hWnd, IDC_PC980124, np2cfg.color16);
#if defined(SUPPORT_PC9821)
			EnableWindow(GetDlgItem(hWnd, IDC_GCBOX), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_GRCGNON), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_GRCG), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_GRCG2), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_EGC), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_PC980124), FALSE);
#endif
			return(TRUE);

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				update = 0;
				b = (UINT8)GetDlgItemCheck(hWnd, IDC_GDC72020);
				if (np2cfg.uPD72020 != b) {
					np2cfg.uPD72020 = b;
					update |= SYS_UPDATECFG;
					gdc_restorekacmode();
					gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
				}
				for (b=0; (b<3) && (!GetDlgItemCheck(hWnd, gdcchip[b])); b++);
				if (np2cfg.grcg != b) {
					np2cfg.grcg = b;
					update |= SYS_UPDATECFG;
					gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
				}
				b = (UINT8)GetDlgItemCheck(hWnd, IDC_PC980124);
				if (np2cfg.color16 != b) {
					np2cfg.color16 = b;
					update |= SYS_UPDATECFG;
				}
				sysmng_update(update);
				return(TRUE);
			}
			break;
	}
	return(FALSE);
}

static LRESULT CALLBACK Scropt3DlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	TCHAR	work[32];
	UINT8	value[6];
	UINT8	b;
	UINT	update;

	switch(msg) {
		case WM_INITDIALOG:
			SendDlgItemMessage(hWnd, IDC_TRAMWAIT, TBM_SETRANGE, TRUE,
											MAKELONG(0, 32));
			SendDlgItemMessage(hWnd, IDC_TRAMWAIT, TBM_SETPOS, TRUE,
											np2cfg.wait[0]);
			wsprintf(work, tchar_u, np2cfg.wait[0]);
			SetDlgItemText(hWnd, IDC_TRAMSTR, work);
			SendDlgItemMessage(hWnd, IDC_VRAMWAIT, TBM_SETRANGE, TRUE,
											MAKELONG(0, 32));
			SendDlgItemMessage(hWnd, IDC_VRAMWAIT, TBM_SETPOS, TRUE,
											np2cfg.wait[2]);
			wsprintf(work, tchar_u, np2cfg.wait[2]);
			SetDlgItemText(hWnd, IDC_VRAMSTR, work);
			SendDlgItemMessage(hWnd, IDC_GRCGWAIT, TBM_SETRANGE, TRUE,
											MAKELONG(0, 32));
			SendDlgItemMessage(hWnd, IDC_GRCGWAIT, TBM_SETPOS, TRUE,
											np2cfg.wait[4]);
			wsprintf(work, tchar_u, np2cfg.wait[4]);
			SetDlgItemText(hWnd, IDC_GRCGSTR, work);

			SendDlgItemMessage(hWnd, IDC_REALPAL, TBM_SETRANGE, TRUE,
											MAKELONG(0, 64));
			SendDlgItemMessage(hWnd, IDC_REALPAL, TBM_SETPOS, TRUE,
											np2cfg.realpal);
			wsprintf(work, tchar_d, (int)np2cfg.realpal - 32);
			SetDlgItemText(hWnd, IDC_REALPALSTR, work);

			return(TRUE);

		case WM_HSCROLL:
			switch(GetDlgCtrlID((HWND)lp)) {
				case IDC_TRAMWAIT:
					b = (UINT8)SendDlgItemMessage(hWnd, IDC_TRAMWAIT,
													TBM_GETPOS, 0, 0);
					wsprintf(work, tchar_u, b);
					SetDlgItemText(hWnd, IDC_TRAMSTR, work);
					break;

				case IDC_VRAMWAIT:
					b = (UINT8)SendDlgItemMessage(hWnd, IDC_VRAMWAIT,
													TBM_GETPOS, 0, 0);
					wsprintf(work, tchar_u, b);
					SetDlgItemText(hWnd, IDC_VRAMSTR, work);
					break;

				case IDC_GRCGWAIT:
					b = (UINT8)SendDlgItemMessage(hWnd, IDC_GRCGWAIT,
													TBM_GETPOS, 0, 0);
					wsprintf(work, tchar_u, b);
					SetDlgItemText(hWnd, IDC_GRCGSTR, work);
					break;

				case IDC_REALPAL:
					b = (UINT8)SendDlgItemMessage(hWnd, IDC_REALPAL,
													TBM_GETPOS, 0, 0);
					wsprintf(work, tchar_d, (int)b - 32);
					SetDlgItemText(hWnd, IDC_REALPALSTR, work);
			}
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				update = 0;
				ZeroMemory(value, sizeof(value));
				value[0] = (UINT8)SendDlgItemMessage(hWnd, IDC_TRAMWAIT,
													TBM_GETPOS, 0, 0);
				if (value[0]) {
					value[1] = 1;
				}
				value[2] = (UINT8)SendDlgItemMessage(hWnd, IDC_VRAMWAIT,
													TBM_GETPOS, 0, 0);
				if (value[2]) {
					value[3] = 1;
				}
				value[4] = (UINT8)SendDlgItemMessage(hWnd, IDC_GRCGWAIT,
													TBM_GETPOS, 0, 0);
				if (value[4]) {
					value[5] = 1;
				}
				for (b=0; b<6; b++) {
					if (np2cfg.wait[b] != value[b]) {
						np2cfg.wait[b] = value[b];
						update |= SYS_UPDATECFG;
					}
				}
				b = (UINT8)SendDlgItemMessage(hWnd, IDC_REALPAL,
														TBM_GETPOS, 0, 0);
				if (np2cfg.realpal != b) {
					np2cfg.realpal = b;
					update |= SYS_UPDATECFG;
				}
				sysmng_update(update);
				return(TRUE);
			}
			break;
	}
	return(FALSE);
}

static const CBPARAM cpZoom[] =
{
	{MAKEINTRESOURCE(IDS_ZOOM_NONE),			0},
	{MAKEINTRESOURCE(IDS_ZOOM_FIXEDASPECT),		1},
	{MAKEINTRESOURCE(IDS_ZOOM_ADJUSTASPECT),	2},
	{MAKEINTRESOURCE(IDS_ZOOM_FULL),			3},
};

static LRESULT CALLBACK ScroptFullScreenDlgProc(HWND hWnd, UINT uMsg,
												WPARAM wParam, LPARAM lParam)
{
	UINT8	c;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			c = np2oscfg.fscrnmod;
			SetDlgItemCheck(hWnd, IDC_FULLSCREEN_SAMEBPP,
													(c & FSCRNMOD_SAMEBPP));
			SetDlgItemCheck(hWnd, IDC_FULLSCREEN_SAMERES,
													(c & FSCRNMOD_SAMERES));
			dlgs_setcbitem(hWnd, IDC_FULLSCREEN_ZOOM,
												cpZoom, NELEMENTS(cpZoom));
			dlgs_setcbcur(hWnd, IDC_FULLSCREEN_ZOOM, (c & 3));
			EnableWindow(GetDlgItem(hWnd, IDC_FULLSCREEN_ZOOM),
												(c & FSCRNMOD_SAMERES) != 0);
			return(TRUE);

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_FULLSCREEN_SAMERES:
					dlgs_enablebyautocheck(hWnd, IDC_FULLSCREEN_ZOOM,
													IDC_FULLSCREEN_SAMERES);
					break;
			}
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lParam)->code) == (UINT)PSN_APPLY)
			{
				c = 0;
				if (GetDlgItemCheck(hWnd, IDC_FULLSCREEN_SAMEBPP))
				{
					c |= FSCRNMOD_SAMEBPP;
				}
				if (GetDlgItemCheck(hWnd, IDC_FULLSCREEN_SAMERES))
				{
					c |= FSCRNMOD_SAMERES;
				}
				c |= dlgs_getcbcur(hWnd, IDC_FULLSCREEN_ZOOM, 0);
				if (np2oscfg.fscrnmod != c)
				{
					np2oscfg.fscrnmod = c;
					sysmng_update(SYS_UPDATEOSCFG);
				}
				return(TRUE);
			}
			break;
	}
	return(FALSE);
}

void dialog_scropt(HWND hWnd)
{
	HINSTANCE		hInstance;
	PROPSHEETPAGE	psp;
	PROPSHEETHEADER	psh;
	HPROPSHEETPAGE	hpsp[4];
	TCHAR			szTitle[128];

	hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

	ZeroMemory(&psp, sizeof(psp));
	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.dwFlags = PSP_DEFAULT;
	psp.hInstance = hInstance;

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SCROPT1);
	psp.pfnDlgProc = (DLGPROC)Scropt1DlgProc;
	hpsp[0] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SCROPT2);
	psp.pfnDlgProc = (DLGPROC)Scropt2DlgProc;
	hpsp[1] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SCROPT3);
	psp.pfnDlgProc = (DLGPROC)Scropt3DlgProc;
	hpsp[2] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SCROPT_FULLSCREEN);
	psp.pfnDlgProc = (DLGPROC)ScroptFullScreenDlgProc;
	hpsp[3] = CreatePropertySheetPage(&psp);

	loadstringresource(IDS_SCREENOPTION, szTitle, NELEMENTS(szTitle));

	ZeroMemory(&psh, sizeof(psh));
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
	psh.hwndParent = hWnd;
	psh.hInstance = hInstance;
	psh.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	psh.nPages = 4;
	psh.phpage = hpsp;
	psh.pszCaption = szTitle;
	psh.pfnCallback = np2class_propetysheet;
	PropertySheet(&psh);
	InvalidateRect(hWnd, NULL, TRUE);
}

