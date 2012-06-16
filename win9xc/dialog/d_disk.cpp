#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"diskdrv.h"
#include	"fddfile.h"
#include	"newdisk.h"


static const TCHAR fddui_title[] = _T("Select floppy image");
static const TCHAR fddui_filter[] =										\
				_T("D88 image files (*.D88;*.D98)\0")					\
								_T("*.d88;*.88d;*.d98;*.98d\0")			\
				_T("Floppy disk images (*.XDF;*.HDM;*.DUP;*.2HD)\0")	\
								_T("*.xdf;*.hdm;*.dup;*.2hd;*.tfd\0")	\
				_T("All supported Files\0")								\
								_T("*.d88;*.88d;*.d98;*.98d;*.fdi;")	\
								_T("*.xdf;*.hdm;*.dup;*.2hd;*.tfd\0")	\
				_T("All files (*.*)\0*.*\0");
static const FILESEL fddui = {fddui_title, str_d88, fddui_filter, 3};

#if defined(SUPPORT_SASI)
static const TCHAR sasiui_title[] = _T("Select SASI/IDE HDD image");
#else
static const TCHAR sasiui_title[] = _T("Select HDD image");
#endif
static const TCHAR sasiui_filter[] =									\
				_T("Anex86 harddisk image files (*.HDI)\0")				\
								_T("*.hdi\0")							\
				_T("T98 harddisk image files (*.THD)\0")				\
								_T("*.thd\0")							\
				_T("T98-Next harddisk image files (*.NHD)\0")			\
								_T("*.nhd\0")							\
				_T("All supported Files\0")								\
								_T("*.thd;*.nhd;*.hdi\0");
static const FILESEL sasiui = {sasiui_title, str_thd, sasiui_filter, 4};

#if defined(SUPPORT_SCSI)
static const TCHAR scsiui_title[] = _T("Select SCSI HDD image");
static const TCHAR scsiui_filter[] =									\
				_T("Virtual98 harddisk image files (*.HDD)\0")			\
								_T("*.hdd\0");
static const FILESEL scsiui = {scsiui_title, str_hdd, scsiui_filter, 1};
#endif

static const TCHAR newdisk_title[] = _T("Create disk image");
#if defined(SUPPORT_SCSI)
static const TCHAR newdisk_filter[] =									\
				_T("D88 image files (*.D88;*.88D)\0")					\
								_T("*.d88;*.88d;*.d98;*.98d\0")			\
				_T("Anex86 harddisk image files (*.HDI)\0")				\
								_T("*.hdi\0")							\
				_T("T98 harddisk image files (*.THD)\0")				\
								_T("*.thd\0")							\
				_T("T98-Next harddisk image files (*.NHD)\0")			\
								_T("*.nhd\0")							\
				_T("Virtual98 harddisk image files (*.HDD)\0")			\
								_T("*.hdd\0");
#else
static const TCHAR newdisk_filter[] =									\
				_T("D88 image files (*.D88;*.88D)\0")					\
								_T("*.d88;*.88d;*.d98;*.98d\0")			\
				_T("Anex86 harddisk image files (*.HDI)\0")				\
								_T("*.hdi\0")							\
				_T("T98 harddisk image files (*.THD)\0")				\
								_T("*.thd\0")							\
				_T("T98-Next harddisk image files (*.NHD)\0")			\
								_T("*.nhd\0");
#endif
static const FILESEL newdiskui = {newdisk_title, str_d88, newdisk_filter, 1};


// ----

void dialog_changefdd(HWND hWnd, REG8 drv) {

const TCHAR	*p;
	TCHAR	path[MAX_PATH];
	int		readonly;

	if (drv < 4) {
		p = fdd_diskname(drv);
		if ((p == NULL) || (p[0] == '\0')) {
			p = fddfolder;
		}
		file_cpyname(path, p, NELEMENTS(path));
		if (dlgs_selectfile(hWnd, &fddui, path, NELEMENTS(path), &readonly)) {
			file_cpyname(fddfolder, path, NELEMENTS(fddfolder));
			sysmng_update(SYS_UPDATEOSCFG);
			diskdrv_setfdd(drv, path, readonly);
		}
	}
}

void dialog_changehdd(HWND hWnd, REG8 drv) {

	UINT		num;
const TCHAR		*p;
const FILESEL	*hddui;
	TCHAR		path[MAX_PATH];

	num = drv & 0x0f;
	p = NULL;
	hddui = NULL;
	if (!(drv & 0x20)) {		// SASI/IDE
		if (num < 2) {
			p = np2cfg.sasihdd[num];
			hddui = &sasiui;
		}
	}
#if defined(SUPPORT_SCSI)
	else {						// SCSI
		if (num < 4) {
			p = np2cfg.scsihdd[num];
			hddui = &scsiui;
		}
	}
#endif
	if (hddui == NULL) {
		return;
	}
	if (p[0] == '\0') {
		p = hddfolder;
	}
	file_cpyname(path, p, NELEMENTS(path));
	if (dlgs_selectfile(hWnd, hddui, path, NELEMENTS(path), NULL)) {
		file_cpyname(hddfolder, path, NELEMENTS(hddfolder));
		sysmng_update(SYS_UPDATEOSCFG);
		diskdrv_sethdd(drv, path);
	}
}


// ---- newdisk

static const TCHAR str_newdisk[] = _T("newdisk");
static const UINT32 hddsizetbl[5] = {20, 41, 65, 80, 128};

static const UINT16 sasires[6] = {
				IDC_NEWSASI5MB, IDC_NEWSASI10MB,
				IDC_NEWSASI15MB, IDC_NEWSASI20MB,
				IDC_NEWSASI30MB, IDC_NEWSASI40MB};

static	BYTE	makefdtype = DISKTYPE_2HD << 4;
static	TCHAR	disklabel[16+1];
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
			switch (LOWORD(wp)) {
				case IDOK:
					GetWindowText(GetDlgItem(hWnd, IDC_HDDSIZE),
													work, NELEMENTS(work));
					val = (UINT)milstr_solveINT(work);
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
			switch (LOWORD(wp)) {
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
			switch (LOWORD(wp)) {
				case IDOK:
					GetWindowText(GetDlgItem(hWnd, IDC_DISKLABEL),
											disklabel, NELEMENTS(disklabel));
					if (milstr_kanji1st(disklabel, NELEMENTS(disklabel) - 1)) {
						disklabel[NELEMENTS(disklabel) - 1] = '\0';
					}
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

	TCHAR		path[MAX_PATH];
	HINSTANCE	hinst;
const TCHAR		*ext;

	file_cpyname(path, fddfolder, NELEMENTS(path));
	file_cutname(path);
	file_catname(path, str_newdisk, NELEMENTS(path));

	if (!dlgs_selectwritefile(hWnd, &newdiskui, path, NELEMENTS(path))) {
		return;
	}
	hinst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
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

