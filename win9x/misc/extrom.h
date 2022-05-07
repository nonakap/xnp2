/**
 * @file	extrom.h
 * @brief	EXTROM ���\�[�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief EXTROM ���\�[�X �N���X
 */
class CExtRom
{
public:
	CExtRom();
	~CExtRom();
	bool Open(LPCTSTR lpFilename);
	void Close();
	UINT Read(LPVOID lpBuffer, UINT cbBuffer);
	LONG Seek(LONG lDistanceToMove, DWORD dwMoveMethod);

private:
	HGLOBAL m_hGlobal;	//!< �n���h��
	LPVOID m_lpRes;		//!< ���\�[�X
	UINT m_nSize;		//!< �T�C�Y
	UINT m_nPointer;	//!< �|�C���^
};
