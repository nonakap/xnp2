/**
 * @file	c_dipsw.h
 * @brief	DIPSW �R���g���[�� �N���X�Q�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "misc\WndProc.h"

/**
 * @brief MIDI �f�o�C�X �N���X
 */
class CStaticDipSw : public CWndProc
{
public:
	virtual void PreSubclassWindow();
	static void Draw(HDC hdc, const void* lpBitmap);
};
