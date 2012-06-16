#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"dosio.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"diskdrv.h"
#include	"newdisk.h"


void dialog_changefdd(BYTE drv) {

	char	fname[MAX_PATH];

	if (drv < 4) {
		if (dlgs_selectfile(fname, sizeof(fname))) {
			diskdrv_setfdd(drv, fname, 0);
		}
	}
}

void dialog_changehdd(BYTE drv) {

	UINT	num;
	BOOL	equip;
	char	fname[MAX_PATH];

	num = drv & 0x0f;
	equip = FALSE;
	if (!(drv & 0x20)) {		// SASI/IDE
		if (num < 2) {
			equip = TRUE;
		}
	}
#if defined(SUPPORT_SCSI)
	else {						// SCSI
		if (num < 4) {
			equip = TRUE;
		}
	}
#endif
	if ((equip) && (dlgs_selectfile(fname, sizeof(fname)))) {
		diskdrv_sethdd(drv, fname);
	}
}


// ---- newdisk

static const char str_newdisk[] = "newdisk.d88";

typedef struct {
	BYTE	fdtype;
	char	label[16+1];
} NEWDISK;

static int NewHddDlgProc(UINT *hddsize, UINT hddminsize, UINT hddmaxsize) {

	DialogPtr	hDlg;
	Str255		sizestr;
	int			done;
	short		item;
	char		work[32];

	hDlg = GetNewDialog(IDD_NEWHDDDISK, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return(IDCANCEL);
	}

	mkstr255(sizestr, "41");
	SetDialogItemText(GetDlgItem(hDlg, IDC_HDDSIZE), sizestr);
	SelectDialogItemText(hDlg, IDC_HDDSIZE, 0x0000, 0x7fff);
	SPRINTF(work, "(%d-%dMB)", hddminsize, hddmaxsize);
	mkstr255(sizestr, work);
	SetDialogItemText(GetDlgItem(hDlg, IDC_HDDLIMIT), sizestr);
	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				GetDialogItemText(GetDlgItem(hDlg, IDC_HDDSIZE), sizestr);
				mkcstr(work, sizeof(work), sizestr);
				*hddsize = milstr_solveINT(work);
				done = IDOK;
				break;

			case IDCANCEL:
				done = IDCANCEL;
				break;
		}
	}
	DisposeDialog(hDlg);
	return(done);
}

static const short sasires[6] =
				{IDC_NEWSASI5MB, IDC_NEWSASI10MB, IDC_NEWSASI15MB,
				IDC_NEWSASI20MB, IDC_NEWSASI30MB, IDC_NEWSASI40MB};

static int NewSASIDlgProc(UINT *hddtype) {

	DialogPtr		hDlg;
	int				i;
	ControlHandle	btn[6];
	int				done;
	short			item;
	int				type;

	hDlg = GetNewDialog(IDD_NEWSASIHDD, NULL, (WindowPtr)-1);
	if (!hDlg) {
		return(IDCANCEL);
	}
	for (i=0; i<6; i++) {
		btn[i] = (ControlHandle)GetDlgItem(hDlg, sasires[i]);
		SetControlValue(btn[i], 0);
	}
	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	type = -1;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				if (type >= 0) {
					if (type > 3) {
						type++;
					}
					*hddtype = type;
					done = IDOK;
				}
				break;

			case IDCANCEL:
				done = IDCANCEL;
				break;

			case IDC_NEWSASI5MB:
			case IDC_NEWSASI10MB:
			case IDC_NEWSASI15MB:
			case IDC_NEWSASI20MB:
			case IDC_NEWSASI30MB:
			case IDC_NEWSASI40MB:
				for (i=0; i<6; i++) {
					if (item == sasires[i]) {
						SetControlValue(btn[i], 1);
						type = i;
					}
					else {
						SetControlValue(btn[i], 0);
					}
				}
				break;
		}
	}
	DisposeDialog(hDlg);
	return(done);
}

static int NewdiskDlgProc(NEWDISK *newdisk) {

	BOOL			usefd144;
	DialogPtr		hDlg;
	int				media;
	int				done;
	short			item;
	Str255			disklabel;
	ControlHandle	btn[3];

	usefd144 = (np2cfg.usefd144)?TRUE:FALSE;
	if (!usefd144) {
		hDlg = GetNewDialog(IDD_NEWFDDDISK, NULL, (WindowPtr)-1);
	}
	else {
		hDlg = GetNewDialog(IDD_NEWFDDDISK2, NULL, (WindowPtr)-1);
	}
	if (!hDlg) {
		return(IDCANCEL);
	}

	SelectDialogItemText(hDlg, IDC_DISKLABEL, 0x0000, 0x7fff);
	btn[0] = (ControlHandle)GetDlgItem(hDlg, IDC_MAKE2DD);
	btn[1] = (ControlHandle)GetDlgItem(hDlg, IDC_MAKE2HD);
	btn[2] = (ControlHandle)NULL;
	if (usefd144) {
		btn[2] = (ControlHandle)GetDlgItem(hDlg, IDC_MAKE144);
	}
	SetControlValue(btn[0], 0);
	SetControlValue(btn[1], 1);
	media = 1;
	SetDialogDefaultItem(hDlg, IDOK);
	SetDialogCancelItem(hDlg, IDCANCEL);

	done = 0;
	while(!done) {
		ModalDialog(NULL, &item);
		switch(item) {
			case IDOK:
				if (media == 0) {
					newdisk->fdtype = (DISKTYPE_2DD << 4);
				}
				else if (media == 1) {
					newdisk->fdtype = (DISKTYPE_2HD << 4);
				}
				else {
					newdisk->fdtype = (DISKTYPE_2HD << 4) + 1;
				}
				GetDialogItemText(GetDlgItem(hDlg, IDC_DISKLABEL), disklabel);
				mkcstr(newdisk->label, sizeof(newdisk->label), disklabel);
				done = IDOK;
				break;

			case IDCANCEL:
				done = IDCANCEL;
				break;

			case IDC_DISKLABEL:
				break;

			case IDC_MAKE2DD:
				SetControlValue(btn[0], 1);
				SetControlValue(btn[1], 0);
				if (usefd144) {
					SetControlValue(btn[2], 0);
				}
				media = 0;
				break;

			case IDC_MAKE2HD:
				SetControlValue(btn[0], 0);
				SetControlValue(btn[1], 1);
				if (usefd144) {
					SetControlValue(btn[2], 0);
				}
				media = 1;
				break;

			case IDC_MAKE144:
				if (usefd144) {
					SetControlValue(btn[0], 0);
					SetControlValue(btn[1], 0);
					SetControlValue(btn[2], 1);
					media = 2;
				}
				break;
		}
	}
	DisposeDialog(hDlg);
	return(done);
}

void dialog_newdisk(void) {

	char	path[MAX_PATH];
	char	*ext;
	UINT	hddsize;
	NEWDISK	disk;

	if (!dlgs_selectwritefile(path, sizeof(path), str_newdisk)) {
		return;
	}
	ext = file_getext(path);
	if (!file_cmpname(ext, str_thd)) {
		hddsize = 0;
		if (NewHddDlgProc(&hddsize, 5, 256) == IDOK) {
			newdisk_thd(path, hddsize);
		}
	}
	else if (!file_cmpname(ext, str_nhd)) {
		hddsize = 0;
		if (NewHddDlgProc(&hddsize, 5, 512) == IDOK) {
			newdisk_vhd(path, hddsize);
		}
	}
	else if (!file_cmpname(ext, str_hdi)) {
		hddsize = 7;
		if (NewSASIDlgProc(&hddsize) == IDOK) {
			newdisk_hdi(path, hddsize);
		}
	}
#if defined(SUPPORT_SCSI)
	else if (!file_cmpname(ext, str_hdd)) {
		hddsize = 0;
		if (NewHddDlgProc(&hddsize, 2, 512) == IDOK) {
			newdisk_vhd(path, hddsize);
		}
	}
#endif
	else if ((!file_cmpname(ext, str_d88)) ||
			(!file_cmpname(ext, str_d98)) ||
			(!file_cmpname(ext, str_88d)) ||
			(!file_cmpname(ext, str_98d))) {
		if (NewdiskDlgProc(&disk) == IDOK) {
			newdisk_fdd(path, disk.fdtype, disk.label);
		}
	}
}

