/*!
 * @file	threadbase.h
 * @brief	�X���b�h���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/*!
 * @brief �X���b�h���N���X
 */
class CThreadBase
{
public:
	CThreadBase();
	virtual ~CThreadBase();

	bool Start();
	void Stop();
	bool Restart();
	void SetStackSize(size_t nStackSize);
	static void Delay(unsigned int usec);

protected:
	virtual bool Task() = 0;	//!< �X���b�h �^�X�N

private:
	HANDLE m_hThread;			//!< �X���b�h �n���h��
	DWORD m_dwThreadId;			//!< �X���b�h ID
	bool m_bAbort;				//!< ���f�t���O
	bool m_bDone;				//!< �����t���O
	size_t m_nStackSize;		//!< �X�^�b�N �T�C�Y
	static unsigned __stdcall ThreadProc(LPVOID pParam);
};

/**
 * �X�^�b�N �T�C�Y�̐ݒ�
 * @param[in] nStackSize �X�^�b�N �T�C�Y
 */
inline void CThreadBase::SetStackSize(size_t nStackSize)
{
	m_nStackSize = nStackSize;
}

/**
 * �X���[�v
 * @param[in] usec �}�C�N���b
 */
inline void CThreadBase::Delay(unsigned int usec)
{
	::Sleep((usec + 999) / 1000);
}
