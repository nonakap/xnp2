#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"timemng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"calendar.h"


static const char str_fmtdate[] = "%4d/%2d/%2d";
static const char str_fmttime[] = "%.2d/%.2d/%.2d";


static void setbtn(ControlHandle *btn, UINT8 calendar) {

	SetControlValue(btn[0], (calendar == 1)?1:0);
	SetControlValue(btn[1], (calendar == 0)?1:0);
}

static void setvirdate(DialogPtr hDlg, const _SYSTIME *st) {

	char	work[64];
	Str255	workstr;

	SPRINTF(work, str_fmtdate, st->year, st->month, st->day);
	mkstr255(workstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_VIRDATE), workstr);
	SPRINTF(work, str_fmttime, st->hour, st->minute, st->second);
	mkstr255(workstr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_VIRTIME), workstr);
}

void CalendarDialogProc(void) {

	DialogPtr		hDlg;
	ControlHandle	btn[2];
	UINT8			calendar;
	BOOL			setnow;
	_SYSTIME		st;
	int				done;
	short			item;

	hDlg = GetNewDialog(IDD_CALENDAR, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	btn[0] = (ControlHandle)GetDlgItem(hDlg, IDC_CLNDREAL);
	btn[1] = (ControlHandle)GetDlgItem(hDlg, IDC_CLNDVIR);
	calendar = (np2cfg.calendar)?1:0;
	setbtn(btn, calendar);
	setnow = FALSE;
	st = cal.dt;
	setvirdate(hDlg, &st);
	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				if (np2cfg.calendar != calendar) {
					np2cfg.calendar = calendar;
					sysmng_update(SYS_UPDATECFG);
				}
				if (setnow) {
					cal.dt = st;
				}
				done = IDOK;
				break;

			case IDCANCEL:
				done = IDCANCEL;
				break;

			case IDC_CLNDREAL:
				calendar = 1;
				setbtn(btn, calendar);
				break;

			case IDC_CLNDVIR:
				calendar = 0;
				setbtn(btn, calendar);
				break;

			case IDC_SETNOW:
				setnow = TRUE;
				timemng_gettime(&st);
				setvirdate(hDlg, &st);
				break;
		}
	}
	DisposeDialog(hDlg);
}

