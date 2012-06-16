#include	"compiler.h"
#include	"resource.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"


void MPU98DialogProc(void) {

	DialogPtr		hDlg;
	int				done;
	short			item;
	ControlHandle	lst[2];
	UINT8			mpu;
	short			value;

	hDlg = GetNewDialog(IDD_MPU98II, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return;
	}
	lst[0] = (ControlHandle)GetDlgItem(hDlg, IDC_MPUPORT);
	lst[1] = (ControlHandle)GetDlgItem(hDlg, IDC_MPUIRQ);

	mpu = np2cfg.mpuopt;
	SetControlValue(lst[0], ((mpu >> 4) & 15) + 1);
	SetControlValue(lst[1], (mpu & 3) + 1);

	SetDialogDefaultItem(hDlg, IDOK);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				mpu = np2cfg.mpuopt;
				value = GetControlValue(lst[0]);
				if (value) {
					mpu &= ~0xf0;
					mpu |= (UINT8)((value - 1) << 4);
				}
				value = GetControlValue(lst[1]);
				if (value) {
					mpu &= ~0x03;
					mpu |= (UINT8)((value - 1) & 3);
				}
				if (np2cfg.mpuopt != mpu) {
					np2cfg.mpuopt = mpu;
					sysmng_update(SYS_UPDATEOSCFG);
				}
				done = IDOK;
				break;

			case IDCANCEL:
				done = IDCANCEL;
				break;

			case IDC_MPUDEF:
				SetControlValue(lst[0], ((0x82 >> 4) & 15) + 1);
				SetControlValue(lst[1], (0x82 & 3) + 1);
				break;
		}
	}
	DisposeDialog(hDlg);
}

