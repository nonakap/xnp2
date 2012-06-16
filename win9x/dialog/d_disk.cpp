#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"oemtext.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"toolwin.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"diskdrv.h"
#include	"fddfile.h"
#include	"newdisk.h"

static const FSPARAM fpFDD =
{
	MAKEINTRESOURCE(IDS_FDDTITLE),
	MAKEINTRESOURCE(IDS_FDDEXT),
	MAKEINTRESOURCE(IDS_FDDFILTER),
	3
};

#if defined(SUPPORT_SASI)
static const FSPARAM fpSASI =
{
	MAKEINTRESOURCE(IDS_SASITITLE),
	MAKEINTRESOURCE(IDS_HDDEXT),
	MAKEINTRESOURCE(IDS_HDDFILTER),
	4
};
#else	// defined(SUPPORT_SASI)
static const FSPARAM fpSASI =
{
	MAKEINTRESOURCE(IDS_HDDTITLE),
	MAKEINTRESOURCE(IDS_HDDEXT),
	MAKEINTRESOURCE(IDS_HDDFILTER),
	4
};
#endif	// defined(SUPPORT_SASI)

#if defined(SUPPORT_IDEIO)
static const FSPARAM fpISO =
{
	MAKEINTRESOURCE(IDS_ISOTITLE),
	MAKEINTRESOURCE(IDS_ISOEXT),
	MAKEINTRESOURCE(IDS_ISOFILTER),
	3
};
#endif	// defined(SUPPORT_IDEIO)

#if defined(SUPPORT_SCSI)
static const FSPARAM fpSCSI =
{
	MAKEINTRESOURCE(IDS_SCSITITLE),
	MAKEINTRESOURCE(IDS_SCSIEXT),
	MAKEINTRESOURCE(IDS_SCSIFILTER),
	1
};
#endif	// defined(SUPPORT_SCSI)

#if defined(SUPPORT_SCSI)
static const FSPARAM fpNewDisk =
{
	MAKEINTRESOURCE(IDS_NEWDISKTITLE),
	MAKEINTRESOURCE(IDS_NEWDISKEXT),
	MAKEINTRESOURCE(IDS_NEWDISKFILTER),
	1
};
#else	// defined(SUPPORT_SCSI)
static const FSPARAM fpNewDisk =
{
	MAKEINTRESOURCE(IDS_NEWDISKTITLE),
	MAKEINTRESOURCE(IDS_NEWDISKEXT),
	MAKEINTRESOURCE(IDS_NEWDISKFILTER2),
	1
};
#endif	// defined(SUPPORT_SCSI)


// ----

void dialog_changefdd(HWND hWnd, REG8 drv) {

const OEMCHAR	*p;
	OEMCHAR		path[MAX_PATH];
	int			readonly;

	if (drv < 4) {
		p = fdd_diskname(drv);
		if ((p == NULL) || (p[0] == '\0')) {
			p = fddfolder;
		}
		file_cpyname(path, p, NELEMENTS(path));
		if (dlgs_openfile(hWnd, &fpFDD, path, NELEMENTS(path), &readonly))
		{
			file_cpyname(fddfolder, path, NELEMENTS(fddfolder));
			sysmng_update(SYS_UPDATEOSCFG);
			diskdrv_setfdd(drv, path, readonly);
			toolwin_setfdd(drv, path);
		}
	}
}

void dialog_changehdd(HWND hWnd, REG8 drv) {

	UINT		num;
const OEMCHAR	*p;
	PCFSPARAM	pfp;
	OEMCHAR		path[MAX_PATH];

	p = diskdrv_getsxsi(drv);
	num = drv & 0x0f;
	pfp = NULL;
	if (!(drv & 0x20))			// SASI/IDE
	{
		if (num < 2)
		{
			pfp = &fpSASI;
		}
#if defined(SUPPORT_IDEIO)
		else if (num == 2)
		{
			pfp = &fpISO;
		}
#endif
	}
#if defined(SUPPORT_SCSI)
	else						// SCSI
	{
		if (num < 4)
		{
			pfp = &fpSCSI;
		}
	}
#endif
	if (pfp == NULL)
	{
		return;
	}
	if ((p == NULL) || (p[0] == '\0'))
	{
		p = hddfolder;
	}
	file_cpyname(path, p, NELEMENTS(path));
	if (dlgs_openfile(hWnd, pfp, path, NELEMENTS(path), NULL))
	{
		file_cpyname(hddfolder, path, NELEMENTS(hddfolder));
		sysmng_update(SYS_UPDATEOSCFG);
		diskdrv_sethdd(drv, path);
	}
}


// ---- newdisk

static const OEMCHAR str_newdisk[] = OEMTEXT("newdisk");
static const UINT32 hddsizetbl[5] = {20, 41, 65, 80, 128};

static const UINT16 sasires[6] = {
				IDC_NEWSASI5MB, IDC_NEWSASI10MB,
				IDC_NEWSASI15MB, IDC_NEWSASI20MB,
				IDC_NEWSASI30MB, IDC_NEWSASI40MB};

static	UINT8	makefdtype = DISKTYPE_2HD << 4;
static	OEMCHAR	disklabel[16+1];
static	UINT	hddsize;
static	UINT	hddminsize;
static	UINT	hddmaxsize;

static LRESULT CALLBACK NewHddDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	UINT	val;
	TCHAR	work[32];

	switch (msg) {
		case WM_INITDIALOG:
			SETLISTUINT32(hWnd, IDC_HDDSIZE, hddsizetbl);
			wsprintf(work, _T("(%d-%dMB)"), hddminsize, hddmaxsize);
			SetWindowText(GetDlgItem(hWnd, IDC_HDDLIMIT), work);
			SetFocus(GetDlgItem(hWnd, IDC_HDDSIZE));
			return(FALSE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDOK:
					GetWindowText(GetDlgItem(hWnd, IDC_HDDSIZE),
													work, NELEMENTS(work));
					val = (UINT)miltchar_solveINT(work);
					if (val < hddminsize) {
						val = hddminsize;
					}
					else if (val > hddmaxsize) {
						val = hddmaxsize;
					}
					hddsize = val;
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

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

static LRESULT CALLBACK NewSASIDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	UINT	val;

	switch (msg) {
		case WM_INITDIALOG:
			SetFocus(GetDlgItem(hWnd, IDC_NEWSASI5MB));
			return(FALSE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDOK:
					for (val=0; val<6; val++) {
						if (GetDlgItemCheck(hWnd, sasires[val])) {
							break;
						}
					}
					if (val > 3) {
						val++;
					}
					hddsize = val;
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

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

static LRESULT CALLBACK NewdiskDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	UINT16	res;
#if defined(OSLANG_UTF8)
	TCHAR	buf[17];
#endif

	switch (msg) {
		case WM_INITDIALOG:
			switch(makefdtype) {
				case (DISKTYPE_2DD << 4):
					res = IDC_MAKE2DD;
					break;

				case (DISKTYPE_2HD << 4):
					res = IDC_MAKE2HD;
					break;

				default:
					res = IDC_MAKE144;
					break;
			}
			SetDlgItemCheck(hWnd, res, 1);
			SetFocus(GetDlgItem(hWnd, IDC_DISKLABEL));
			return(FALSE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDOK:
#if defined(OSLANG_UTF8)
					GetWindowText(GetDlgItem(hWnd, IDC_DISKLABEL),
														buf, NELEMENTS(buf));
					tchartooem(disklabel, NELEMENTS(disklabel), buf, -1);
#else
					GetWindowText(GetDlgItem(hWnd, IDC_DISKLABEL),
											disklabel, NELEMENTS(disklabel));
					if (milstr_kanji1st(disklabel, NELEMENTS(disklabel) - 1)) {
						disklabel[NELEMENTS(disklabel) - 1] = '\0';
					}
#endif
					if (GetDlgItemCheck(hWnd, IDC_MAKE2DD)) {
						makefdtype = (DISKTYPE_2DD << 4);
					}
					else if (GetDlgItemCheck(hWnd, IDC_MAKE2HD)) {
						makefdtype = (DISKTYPE_2HD << 4);
					}
					else {
						makefdtype = (DISKTYPE_2HD << 4) + 1;
					}
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

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

void dialog_newdisk(HWND hWnd) {

	OEMCHAR		path[MAX_PATH];
	HINSTANCE	hinst;
const OEMCHAR	*ext;

	file_cpyname(path, fddfolder, NELEMENTS(path));
	file_cutname(path);
	file_catname(path, str_newdisk, NELEMENTS(path));

	if (!dlgs_createfile(hWnd, &fpNewDisk, path, NELEMENTS(path)))
	{
		return;
	}
	hinst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	ext = file_getext(path);
	if (!file_cmpname(ext, str_thd)) {
		hddsize = 0;
		hddminsize = 5;
		hddmaxsize = 256;
		if (DialogBox(hinst, MAKEINTRESOURCE(IDD_NEWHDDDISK),
									hWnd, (DLGPROC)NewHddDlgProc) == IDOK) {
			newdisk_thd(path, hddsize);
		}
	}
	else if (!file_cmpname(ext, str_nhd)) {
		hddsize = 0;
		hddminsize = 5;
		hddmaxsize = 512;
		if (DialogBox(hinst, MAKEINTRESOURCE(IDD_NEWHDDDISK),
									hWnd, (DLGPROC)NewHddDlgProc) == IDOK) {
			newdisk_nhd(path, hddsize);
		}
	}
	else if (!file_cmpname(ext, str_hdi)) {
		hddsize = 7;
		if (DialogBox(hinst, MAKEINTRESOURCE(IDD_NEWSASI),
									hWnd, (DLGPROC)NewSASIDlgProc) == IDOK) {
			newdisk_hdi(path, hddsize);
		}
	}
#if defined(SUPPORT_SCSI)
	else if (!file_cmpname(ext, str_hdd)) {
		hddsize = 0;
		hddminsize = 2;
		hddmaxsize = 512;
		if (DialogBox(hinst, MAKEINTRESOURCE(IDD_NEWHDDDISK),
									hWnd, (DLGPROC)NewHddDlgProc) == IDOK) {
			newdisk_vhd(path, hddsize);
		}
	}
#endif
	else if ((!file_cmpname(ext, str_d88)) ||
			(!file_cmpname(ext, str_d98)) ||
			(!file_cmpname(ext, str_88d)) ||
			(!file_cmpname(ext, str_98d))) {
		if (DialogBox(hinst,
				MAKEINTRESOURCE((np2cfg.usefd144)?IDD_NEWDISK2:IDD_NEWDISK),
									hWnd, (DLGPROC)NewdiskDlgProc) == IDOK) {
			newdisk_fdd(path, makefdtype, disklabel);
		}
	}
}

