/**
 * @file	viewreg.h
 * @brief	���W�X�^�\���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <vector>
#include "viewitem.h"

/**
 * @brief ���W�X�^�\���N���X
 */
class CDebugUtyReg : public CDebugUtyItem
{
public:
	CDebugUtyReg(CDebugUtyView* lpView);
	virtual ~CDebugUtyReg();

	virtual void Initialize(const CDebugUtyItem* lpItem = NULL);
	virtual bool Update();
	virtual bool Lock();
	virtual void Unlock();
	virtual bool IsLocked();
	virtual void OnPaint(HDC hDC, const RECT& rect);

private:
	std::vector<unsigned char> m_buffer;		//!< �o�b�t�@
};
