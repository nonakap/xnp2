/**
 * @file	extrom.cpp
 * @brief	EXTROM ���\�[�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "ExtRom.h"
#include "np2.h"
#include "WndProc.h"

//! ���\�[�X��
static const TCHAR s_szExtRom[] = TEXT("EXTROM");

/**
 * �R���X�g���N�^
 */
CExtRom::CExtRom()
	: m_hGlobal(NULL)
	, m_lpRes(NULL)
	, m_nSize(0)
	, m_nPointer(0)
{
}

/**
 * �f�X�g���N�^
 */
CExtRom::~CExtRom()
{
	Close();
}

/**
 * �I�[�v��
 * @param[in] lpFilename �t�@�C����
 * @retval true ����
 * @retval false ���s
 */
bool CExtRom::Open(LPCTSTR lpFilename)
{
	Close();

	HINSTANCE hInstance = CWndProc::FindResourceHandle(lpFilename, s_szExtRom);
	HRSRC hRsrc = ::FindResource(hInstance, lpFilename, s_szExtRom);
	if (hRsrc == NULL)
	{
		return false;
	}

	m_hGlobal = ::LoadResource(hInstance, hRsrc);
	m_lpRes = ::LockResource(m_hGlobal);
	m_nSize = ::SizeofResource(hInstance, hRsrc);
	m_nPointer = 0;
	return true;
}

/**
 * �N���[�Y
 */
void CExtRom::Close()
{
	if (m_hGlobal)
	{
		::FreeResource(m_hGlobal);
		m_hGlobal = NULL;
	}
	m_lpRes = NULL;
	m_nSize = 0;
	m_nPointer = 0;
}

/**
 * �ǂݍ���
 * @param[out] lpBuffer �o�b�t�@
 * @param[out] cbBuffer �o�b�t�@��
 * @return �T�C�Y
 */
UINT CExtRom::Read(LPVOID lpBuffer, UINT cbBuffer)
{
	UINT nLength = m_nSize - m_nPointer;
	nLength = min(nLength, cbBuffer);
	if (nLength)
	{
		if (lpBuffer)
		{
			CopyMemory(lpBuffer, static_cast<char*>(m_lpRes) + m_nPointer, nLength);
		}
		m_nPointer += nLength;
	}
	return nLength;
}

/**
 * �V�[�N
 * @param[in] lDistanceToMove �t�@�C���|�C���^�̈ړ��o�C�g��
 * @param[in] dwMoveMethod �t�@�C���|�C���^���ړ����邽�߂̊J�n�_�i��_�j���w�肵�܂�
 * @return ���݂̈ʒu
 */
LONG CExtRom::Seek(LONG lDistanceToMove, DWORD dwMoveMethod)
{
	switch (dwMoveMethod)
	{
		case FILE_BEGIN:
		default:
			break;

		case FILE_CURRENT:
			lDistanceToMove += m_nPointer;
			break;

		case FILE_END:
			lDistanceToMove += m_nSize;
			break;
	}

	if (lDistanceToMove < 0)
	{
		lDistanceToMove = 0;
	}
	else if (static_cast<UINT>(lDistanceToMove) > m_nSize)
	{
		lDistanceToMove = m_nSize;
	}
	m_nPointer = lDistanceToMove;
	return lDistanceToMove;
}
