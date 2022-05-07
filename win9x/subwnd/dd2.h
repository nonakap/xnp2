/**
 * @file	dd2.h
 * @brief	DirectDraw2 �`��N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <ddraw.h>
#include "cmndraw.h"

/**
 * @brief DirectDraw2 class
 */
class DD2Surface
{
public:
	DD2Surface();
	~DD2Surface();

	bool Create(HWND hWnd, int nWidth, int nHeight);
	void Release();
	CMNVRAM* Lock();
	void Unlock();
	void Blt(const POINT* pt, const RECT* lpRect = NULL);
	UINT16 GetPalette16(RGB32 pal) const;

protected:
	HWND					m_hWnd;				/*!< �E�B���h�E �n���h�� */
	LPDIRECTDRAW			m_pDDraw;			/*!< DirectDraw �C���X�^���X */
	LPDIRECTDRAW2			m_pDDraw2;			/*!< DirectDraw2 �C���X�^���X */
	LPDIRECTDRAWSURFACE		m_pPrimarySurface;	/*!< �v���C�}�� �T�[�t�F�X */
	LPDIRECTDRAWSURFACE		m_pBackSurface;		/*!< �o�b�N �T�[�t�F�X */
	LPDIRECTDRAWCLIPPER		m_pClipper;			/*!< �N���b�p�[ */
	LPDIRECTDRAWPALETTE		m_pPalette;			/*!< �p���b�g */
	RGB32					m_pal16;			/*!< 16BPP�}�X�N */
	UINT8					m_r16b;				/*!< B �V�t�g�� */
	UINT8					m_l16r;				/*!< R �V�t�g�� */
	UINT8					m_l16g;				/*!< G �V�t�g�� */
	CMNVRAM					m_vram;				/*!< VRAM */
	PALETTEENTRY			m_pal[256];			/*!< �p���b�g */
};
