/**
 * @file	c_dipsw.cpp
 * @brief	DIPSW �R���g���[�� �N���X�Q�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "c_dipsw.h"
#include "common/bmpdata.h"

/**
 * ������
 */
void CStaticDipSw::PreSubclassWindow()
{
	ModifyStyle(SS_TYPEMASK, SS_OWNERDRAW);
}

/**
 * �`��
 * @param[in] hdc �f�o�C�X �R���e�L�X�g
 * @param[in] lpBitmap �r�b�g�}�b�v
 */
void CStaticDipSw::Draw(HDC hdc, const void* lpBitmap)
{
	if (lpBitmap == NULL)
	{
		return;
	}

	const BMPFILE* lpBmpFile = static_cast<const BMPFILE*>(lpBitmap);
	const BMPINFO* lpBmpInfo = reinterpret_cast<const BMPINFO*>(lpBmpFile + 1);

	BMPDATA inf;
	if (::bmpdata_getinfo(lpBmpInfo, &inf) != SUCCESS)
	{
		return;
	}

	void* pImage;
	HBITMAP hBitmap = ::CreateDIBSection(hdc, reinterpret_cast<const BITMAPINFO*>(lpBmpInfo), DIB_RGB_COLORS, &pImage, NULL, 0);
	if (hBitmap == NULL)
	{
		return;
	}
	CopyMemory(pImage, static_cast<const UINT8*>(lpBitmap) + (LOADINTELDWORD(lpBmpFile->bfOffBits)), ::bmpdata_getdatasize(lpBmpInfo));
	HDC hdcMem = CreateCompatibleDC(hdc);
	::SelectObject(hdcMem, hBitmap);
	if (inf.height < 0)
	{
		inf.height *= -1;
	}
	::BitBlt(hdc, 0, 0, inf.width, inf.height, hdcMem, 0, 0, SRCCOPY);
	::DeleteDC(hdcMem);
	::DeleteObject(hBitmap);
}
