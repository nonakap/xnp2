/**
 * @file	viewseg.h
 * @brief	������ ���W�X�^�\���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <vector>
#include "viewitem.h"
#include "viewmem.h"

/**
 * @brief ������ ���W�X�^�\���N���X
 */
class CDebugUtySeg : public CDebugUtyItem
{
public:
	CDebugUtySeg(CDebugUtyView* lpView);
	virtual ~CDebugUtySeg();

	virtual void Initialize(const CDebugUtyItem* lpItem = NULL);
	virtual bool Update();
	virtual bool Lock();
	virtual void Unlock();
	virtual bool IsLocked();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnPaint(HDC hDC, const RECT& rect);

private:
	UINT m_nSegment;							//!< �Z�O�����g
	DebugUtyViewMemory m_mem;					//!< ������
	std::vector<unsigned char> m_buffer;		//!< �o�b�t�@
	void SetSegment(UINT nSegment);
};
