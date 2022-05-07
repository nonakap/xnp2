/*!
 * @file	threadbase.cpp
 * @brief	�X���b�h���N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "threadbase.h"
#include <process.h>

/*!
 * @brief �R���X�g���N�^
 */
CThreadBase::CThreadBase()
	: m_hThread(INVALID_HANDLE_VALUE)
	, m_dwThreadId(0)
	, m_bAbort(false)
	, m_bDone(false)
	, m_nStackSize(0)
{
}

/*!
 * @brief �f�X�g���N�^
 */
CThreadBase::~CThreadBase()
{
	Stop();
}

/*!
 * @brief �X���b�h�J�n
 *
 * @retval true ����
 */
bool CThreadBase::Start()
{
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		return false;
	}

	m_bAbort = false;
	m_bDone = false;
	unsigned int nThreadId = 0;
	HANDLE hThread = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, static_cast<unsigned>(m_nStackSize), &ThreadProc, this, 0, &nThreadId));
	if (hThread == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	m_hThread = hThread;
	m_dwThreadId = nThreadId;
	return true;
}

/*!
 * @brief �X���b�h�I��
 *
 * @retval true ����
 */
void CThreadBase::Stop()
{
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		m_bAbort = true;
		::WaitForSingleObject(m_hThread, INFINITE);
		::CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
}

/*!
 * @brief �X���b�h�ĊJ
 *
 * @retval true ����
 */
bool CThreadBase::Restart()
{
	if ((m_hThread != INVALID_HANDLE_VALUE) && (m_bDone))
	{
		Stop();
	}
	return Start();
}

/*!
 * �X���b�h����
 * @param[in] pParam this �|�C���^
 * @retval 0 ���0
 */
unsigned __stdcall CThreadBase::ThreadProc(LPVOID pParam)
{
	CThreadBase& obj = *(static_cast<CThreadBase*>(pParam));
	while ((!obj.m_bAbort) && (obj.Task()))
	{
	}

	obj.m_bDone = true;
	return 0;
}
