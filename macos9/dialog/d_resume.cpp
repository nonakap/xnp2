#include	"compiler.h"
#include	"resource.h"
#include	"dialog.h"
#include	"dialogs.h"


void ResumeErrorDialogProc(void) {

	DialogPtr	hDlg;
	int			done;
	short		item;

	hDlg = GetNewDialog(IDD_RESUMEERROR, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	SetDialogDefaultItem(hDlg, IDOK);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				done = 1;
				break;
		}
	}
	DisposeDialog(hDlg);
}

int ResumeWarningDialogProc(const char *string) {

	DialogPtr	hDlg;
	Str255		str;
	int			done;
	short		item;

	hDlg = GetNewDialog(IDD_RESUMEWARNING, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return(IDCANCEL);
	}
	mkstr255(str, string);
	SetDialogItemText(GetDlgItem(hDlg, IDC_RESUMEREASON), str);
	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
			case IDCANCEL:
			case IDC_RESUMEEXIT:
				done = item;
				break;
		}
	}
	DisposeDialog(hDlg);
	return(done);
}

