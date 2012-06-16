#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"oemtext.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrnsave.h"
#include	"font.h"


static const FSPARAM fpFont =
{
	MAKEINTRESOURCE(IDS_FONTTITLE),
	MAKEINTRESOURCE(IDS_FONTEXT),
	MAKEINTRESOURCE(IDS_FONTFILTER),
	3
};

static const LPTSTR lpszBmpFilter[] =
{
	MAKEINTRESOURCE(IDS_BMPFILTER1),
	MAKEINTRESOURCE(IDS_BMPFILTER4),
	MAKEINTRESOURCE(IDS_BMPFILTER8),
	MAKEINTRESOURCE(IDS_BMPFILTER24)
};
static const OEMCHAR szBmpFile[] = OEMTEXT("NP2_####.BMP");

void dialog_font(HWND hWnd)
{
	OEMCHAR	szPath[MAX_PATH];

	file_cpyname(szPath, np2cfg.fontfile, NELEMENTS(szPath));
	if ((dlgs_openfile(hWnd, &fpFont, szPath, NELEMENTS(szPath), NULL)) &&
		(font_load(szPath, FALSE)))
	{
		gdcs.textdisp |= GDCSCRN_ALLDRAW2;
		milstr_ncpy(np2cfg.fontfile, szPath, NELEMENTS(np2cfg.fontfile));
		sysmng_update(SYS_UPDATECFG);
	}
}

void dialog_writebmp(HWND hWnd) {

	SCRNSAVE	ss;
	FSPARAM		fp;
	OEMCHAR		szPath[MAX_PATH];
const OEMCHAR	*pszExt;

	ss = scrnsave_get();
	if (ss == NULL)
	{
		return;
	}
	fp.lpszTitle = MAKEINTRESOURCE(IDS_BMPTITLE);
	fp.lpszDefExt = MAKEINTRESOURCE(IDS_BMPEXT);
	fp.lpszFilter = lpszBmpFilter[ss->type];
	fp.nFilterIndex = 1;
	file_cpyname(szPath, bmpfilefolder, NELEMENTS(szPath));
	file_cutname(szPath);
	file_catname(szPath, szBmpFile, NELEMENTS(szPath));
	if (dlgs_createfilenum(hWnd, &fp, szPath, NELEMENTS(szPath)))
	{
		file_cpyname(bmpfilefolder, szPath, NELEMENTS(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);
		pszExt = file_getext(szPath);
		if ((ss->type <= SCRNSAVE_8BIT) &&
			(!file_cmpname(pszExt, OEMTEXT("gif"))))
		{
			scrnsave_writegif(ss, szPath, SCRNSAVE_AUTO);
		}
		else if (!file_cmpname(pszExt, str_bmp))
		{
			scrnsave_writebmp(ss, szPath, SCRNSAVE_AUTO);
		}
	}
	scrnsave_trash(ss);
}

