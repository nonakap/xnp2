/**
 * @file	d_bmp.cpp
 * @brief	bmp dialog
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include "dosio.h"
#include "np2.h"
#include "sysmng.h"
#include "misc/DlgProc.h"
#include "pccore.h"
#include "iocore.h"
#include "common/strres.h"
#include "vram/scrnsave.h"

/** �t�B���^�[ */
static const UINT s_nFilter[4] =
{
	IDS_BMPFILTER1,
	IDS_BMPFILTER4,
	IDS_BMPFILTER8,
	IDS_BMPFILTER24
};

/**
 * �f�t�H���g �t�@�C���𓾂�
 * @param[in] lpExt �g���q
 * @param[out] lpFilename �t�@�C����
 * @param[in] cchFilename �t�@�C������
 */
static void GetDefaultFilename(LPCTSTR lpExt, LPTSTR lpFilename, UINT cchFilename)
{
	for (UINT i = 0; i < 10000; i++)
	{
		TCHAR szFilename[MAX_PATH];
		wsprintf(szFilename, TEXT("NP2_%04d.%s"), i, lpExt);

		file_cpyname(lpFilename, bmpfilefolder, cchFilename);
		file_cutname(lpFilename);
		file_catname(lpFilename, szFilename, cchFilename);

		if (file_attr(lpFilename) == -1)
		{
			break;
		}
	}
}

/**
 * BMP �o��
 * @param[in] hWnd �e�E�B���h�E
 */
void dialog_writebmp(HWND hWnd)
{
	SCRNSAVE ss = scrnsave_create();
	if (ss == NULL)
	{
		return;
	}
	const int nType = scrnsave_gettype(ss);

	std::tstring rExt(LoadTString(IDS_BMPEXT));
	std::tstring rFilter(LoadTString(s_nFilter[nType]));
	std::tstring rTitle(LoadTString(IDS_BMPTITLE));

	TCHAR szPath[MAX_PATH];
	GetDefaultFilename(rExt.c_str(), szPath, _countof(szPath));

	CFileDlg dlg(FALSE, rExt.c_str(), szPath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, rFilter.c_str(), hWnd);
	dlg.m_ofn.lpstrTitle = rTitle.c_str();
	dlg.m_ofn.nFilterIndex = 1;
	if (dlg.DoModal())
	{
		LPCTSTR lpFilename = dlg.GetPathName();
		file_cpyname(bmpfilefolder, lpFilename, _countof(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);

		LPCTSTR lpExt = file_getext(szPath);
		if ((nType <= SCRNSAVE_8BIT) && (!file_cmpname(lpExt, TEXT("gif"))))
		{
			scrnsave_writegif(ss, lpFilename, SCRNSAVE_AUTO);
		}
		else if (!file_cmpname(lpExt, str_bmp))
		{
			scrnsave_writebmp(ss, lpFilename, SCRNSAVE_AUTO);
		}
	}
	scrnsave_destroy(ss);
}
