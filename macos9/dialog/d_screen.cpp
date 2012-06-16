#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrndraw.h"
#include	"palettes.h"


static void setchip(ControlHandle *btn, UINT chip) {

	SetControlValue(btn[0], (chip == 0)?1:0);
	SetControlValue(btn[1], (chip == 1)?1:0);
}

static void setgrcg(ControlHandle *btn, UINT grcg) {

	SetControlValue(btn[0], (grcg == 0)?1:0);
	SetControlValue(btn[1], (grcg == 1)?1:0);
	SetControlValue(btn[2], (grcg == 2)?1:0);
	SetControlValue(btn[3], (grcg == 3)?1:0);
}


void dialog_scropt(void) {

	DialogPtr		hDlg;
	ControlHandle	chipbtn[2];
	ControlHandle	grcgbtn[4];
	ControlHandle	lcdbtn[2];
	ControlHandle	skipbtn;
	ControlHandle	c16btn;
	UINT			chip;
	UINT			grcg;
	UINT			color16;
	UINT			lcd;
	UINT			skipline;
	UINT			val;
	char			work[32];
	Str255			workstr;
	int				done;
	short			item;
	UINT			update;
	UINT			renewal;
	BYTE			waitval[6];

	hDlg = GetNewDialog(IDD_SCREENOPT, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	chipbtn[0] = (ControlHandle)GetDlgItem(hDlg, IDC_GDC7220);
	chipbtn[1] = (ControlHandle)GetDlgItem(hDlg, IDC_GDC72020);

	grcgbtn[0] = (ControlHandle)GetDlgItem(hDlg, IDC_GRCGNON);
	grcgbtn[1] = (ControlHandle)GetDlgItem(hDlg, IDC_GRCG);
	grcgbtn[2] = (ControlHandle)GetDlgItem(hDlg, IDC_GRCG2);
	grcgbtn[3] = (ControlHandle)GetDlgItem(hDlg, IDC_EGC);

	c16btn = (ControlHandle)GetDlgItem(hDlg, IDC_PC980124);

	lcdbtn[0] = (ControlHandle)GetDlgItem(hDlg, IDC_LCD);
	lcdbtn[1] = (ControlHandle)GetDlgItem(hDlg, IDC_LCDX);
	skipbtn = (ControlHandle)GetDlgItem(hDlg, IDC_SKIPLINE);

	chip = (np2cfg.uPD72020)?1:0;
	setchip(chipbtn, chip);
	grcg = np2cfg.grcg & 3;
	setgrcg(grcgbtn, grcg);
	color16 = (np2cfg.color16)?1:0;
	SetControlValue(c16btn, color16);

	val = np2cfg.wait[0];
	if (val > 32) {
		val = 32;
	}
	SPRINTF(work, str_u, val);
	mkstr255(workstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_TRAMWAIT), workstr);
	val = np2cfg.wait[2];
	if (val > 32) {
		val = 32;
	}
	SPRINTF(work, str_u, val);
	mkstr255(workstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_VRAMWAIT), workstr);
	val = np2cfg.wait[4];
	if (val > 32) {
		val = 32;
	}
	SPRINTF(work, str_u, val);
	mkstr255(workstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_GRCGWAIT), workstr);
	val = np2cfg.realpal;
	if (val > 64) {
		val = 64;
	}
	SPRINTF(work, str_d, val - 32);
	mkstr255(workstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_REALPAL), workstr);

	lcd = np2cfg.LCD_MODE & 3;
	SetControlValue(lcdbtn[0], lcd & 1);
	SetControlValue(lcdbtn[1], (lcd & 2) >> 1);
	skipline = (np2cfg.skipline)?1:0;
	SetControlValue(skipbtn, skipline);
	SPRINTF(work, str_u, np2cfg.skiplight);
	mkstr255(workstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_SKIPLIGHT), workstr);

	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				update = 0;
				if (np2cfg.uPD72020 != chip) {
					np2cfg.uPD72020 = chip;
					update |= SYS_UPDATECFG;
					gdc_restorekacmode();
					gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
				}
				if (np2cfg.grcg != grcg) {
					np2cfg.grcg = grcg;
					update |= SYS_UPDATECFG;
					gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
				}
				if (np2cfg.color16 != color16) {
					np2cfg.color16 = color16;
					update |= SYS_UPDATECFG;
				}

				GetDialogItemText(GetDlgItem(hDlg, IDC_TRAMWAIT), workstr);
				mkcstr(work, sizeof(work), workstr);
				val = milstr_solveINT(work);
				if (val > 32) {
					val = 32;
				}
				waitval[0] = val;
				waitval[1] = (val)?1:0;
				GetDialogItemText(GetDlgItem(hDlg, IDC_VRAMWAIT), workstr);
				mkcstr(work, sizeof(work), workstr);
				val = milstr_solveINT(work);
				if (val > 32) {
					val = 32;
				}
				waitval[2] = val;
				waitval[3] = (val)?1:0;
				GetDialogItemText(GetDlgItem(hDlg, IDC_GRCGWAIT), workstr);
				mkcstr(work, sizeof(work), workstr);
				val = milstr_solveINT(work);
				if (val > 32) {
					val = 32;
				}
				waitval[4] = val;
				waitval[5] = (val)?1:0;
				if (memcmp(np2cfg.wait, waitval, 6)) {
					CopyMemory(np2cfg.wait, waitval, 6);
					update |= SYS_UPDATECFG;
				}
				GetDialogItemText(GetDlgItem(hDlg, IDC_REALPAL), workstr);
				mkcstr(work, sizeof(work), workstr);
				val = milstr_solveINT(work) + 32;
				if (val > 64) {
					val = 64;
				}
				if (np2cfg.realpal != val) {
					np2cfg.realpal = val;
					update |= SYS_UPDATECFG;
				}

				renewal = 0;
				if (np2cfg.skipline != skipline) {
					np2cfg.skipline = skipline;
					renewal = 1;
				}
				GetDialogItemText(GetDlgItem(hDlg, IDC_SKIPLIGHT), workstr);
				mkcstr(work, sizeof(work), workstr);
				val = milstr_solveINT(work);
				if (val > 255) {
					val = 255;
				}
				if (np2cfg.skiplight != val) {
					np2cfg.skiplight = val;
					renewal = 1;
				}
				if (renewal) {
					pal_makeskiptable();
				}
				if (np2cfg.LCD_MODE != lcd) {
					np2cfg.LCD_MODE = lcd;
					pal_makelcdpal();
					renewal = 1;
				}
				if (renewal) {
					update |= SYS_UPDATECFG;
					scrndraw_redraw();
				}

				sysmng_update(update);
				done = IDOK;
				break;

			case IDCANCEL:
				done = IDCANCEL;
				break;

			case IDC_LCD:
				lcd ^= 1;
				SetControlValue(lcdbtn[0], lcd & 1);
				break;

			case IDC_LCDX:
				lcd ^= 2;
				SetControlValue(lcdbtn[1], (lcd & 2) >> 1);
				break;

			case IDC_SKIPLINE:
				skipline ^= 1;
				SetControlValue(skipbtn, skipline);
				break;

			case IDC_GDC7220:
				chip = 0;
				setchip(chipbtn, chip);
				break;

			case IDC_GDC72020:
				chip = 1;
				setchip(chipbtn, chip);
				break;

			case IDC_GRCGNON:
				grcg = 0;
				setgrcg(grcgbtn, grcg);
				break;

			case IDC_GRCG:
				grcg = 1;
				setgrcg(grcgbtn, grcg);
				break;

			case IDC_GRCG2:
				grcg = 2;
				setgrcg(grcgbtn, grcg);
				break;

			case IDC_EGC:
				grcg = 3;
				setgrcg(grcgbtn, grcg);
				break;

			case IDC_PC980124:
				color16 ^= 1;
				SetControlValue(c16btn, color16);
				break;
		}
	}
	DisposeDialog(hDlg);
}

