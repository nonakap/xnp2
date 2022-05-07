/**
 * @file	cmpara.cpp
 * @brief	�p������ �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "cmpara.h"

/**
 * �C���X�^���X�쐬
 * @param[in] nPort �|�[�g�ԍ�
 * @return �C���X�^���X
 */
CComPara* CComPara::CreateInstance(UINT nPort)
{
	CComPara* pPara = new CComPara;
	if (!pPara->Initialize(nPort))
	{
		delete pPara;
		pPara = NULL;
	}
	return pPara;
}

/**
 * �R���X�g���N�^
 */
CComPara::CComPara()
	: CComBase(COMCONNECT_PARALLEL)
	, m_hParallel(INVALID_HANDLE_VALUE)
{
}

/**
 * �f�X�g���N�^
 */
CComPara::~CComPara()
{
	if (m_hParallel != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hParallel);
	}
}

/**
 * ������
 * @param[in] nPort �|�[�g�ԍ�
 * @retval true ����
 * @retval false ���s
 */
bool CComPara::Initialize(UINT nPort)
{
	TCHAR szName[16];
	wsprintf(szName, TEXT("LPT%u"), nPort);
	m_hParallel = CreateFile(szName, GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
	return (m_hParallel != INVALID_HANDLE_VALUE);
}

/**
 * �ǂݍ���
 * @param[out] pData �o�b�t�@
 * @return �T�C�Y
 */
UINT CComPara::Read(UINT8* pData)
{
	return 0;
}

/**
 * ��������
 * @param[out] cData �f�[�^
 * @return �T�C�Y
 */
UINT CComPara::Write(UINT8 cData)
{
	DWORD dwWrittenSize;
	return (::WriteFile(m_hParallel, &cData, 1, &dwWrittenSize, NULL)) ? 1 : 0;
}

/**
 * �X�e�[�^�X�𓾂�
 * @return �X�e�[�^�X
 */
UINT8 CComPara::GetStat()
{
	return 0x00;
}

/**
 * ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 * @param[in] nParam �p�����^
 * @return ���U���g �R�[�h
 */
INTPTR CComPara::Message(UINT nMessage, INTPTR nParam)
{
	return 0;
}
