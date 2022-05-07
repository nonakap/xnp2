/**
 * @file	viewitem.h
 * @brief	DebugUty �p�r���[���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

class CDebugUtyView;

/**
 * @brief �f�o�O�\���A�C�e���̊��N���X
 */
class CDebugUtyItem
{
public:
	static CDebugUtyItem* New(UINT nID, CDebugUtyView* lpView, const CDebugUtyItem* lpItem = NULL);

	CDebugUtyItem(CDebugUtyView* lpView, UINT nID);
	virtual ~CDebugUtyItem();
	UINT GetID() const;

	virtual void Initialize(const CDebugUtyItem* lpItem = NULL);
	virtual bool Update();
	virtual bool Lock();
	virtual void Unlock();
	virtual bool IsLocked();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnPaint(HDC hDC, const RECT& rect);

protected:
	CDebugUtyView* m_lpView;	//!< �r���[ �N���X
	UINT m_nID;					//!< ID
};

/**
 * ID ��Ԃ�
 * @return ID
 */
inline UINT CDebugUtyItem::GetID() const
{
	return m_nID;
}
