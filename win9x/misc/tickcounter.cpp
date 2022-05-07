/**
 * @file	tickcounter.cpp
 * @brief	TICK �J�E���^�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "tickcounter.h"

/**
 * @brief TICK �J�E���^�[ �N���X
 */
class TickCounter
{
public:
	TickCounter();
	DWORD Get();

private:
	LARGE_INTEGER m_nFreq;		//!< ���g��
	LARGE_INTEGER m_nLast;		//!< �Ō�̃J�E���^
	DWORD m_dwLastTick;			//!< �Ō�� TICK
};

/**
 * �R���X�g���N�^
 */
TickCounter::TickCounter()
{
	m_nFreq.QuadPart = 0;
	::QueryPerformanceFrequency(&m_nFreq);
	if (m_nFreq.QuadPart)
	{
		m_dwLastTick = ::GetTickCount();
		::QueryPerformanceCounter(&m_nLast);
	}
}

/**
 * TICK �𓾂�
 * @return TICK
 */
DWORD TickCounter::Get()
{
	if (m_nFreq.QuadPart)
	{
		LARGE_INTEGER nNow;
		::QueryPerformanceCounter(&nNow);
		const ULONGLONG nPast = nNow.QuadPart - m_nLast.QuadPart;

		const DWORD dwTick = static_cast<DWORD>((nPast * 1000U) / m_nFreq.QuadPart);
		const DWORD dwRet = m_dwLastTick + dwTick;
		if (dwTick >= 1000)
		{
			const DWORD dwSeconds = dwTick / 1000;
			m_nLast.QuadPart += m_nFreq.QuadPart * dwSeconds;
			m_dwLastTick += dwSeconds * 1000;
		}
		return dwRet;
	}
	else
	{
		return ::GetTickCount();
	}
}


// ---- C �C���^�t�F�C�X

//! �J�E���^ �C���X�^���X
static TickCounter s_tick;

/**
 * �J�E���^�𓾂�
 * @return TICK
 */
DWORD GetTickCounter()
{
	return s_tick.Get();
}
