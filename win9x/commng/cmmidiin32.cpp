/**
 * @file	cmmidiin32.cpp
 * @brief	MIDI IN win32 �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "cmmidiin32.h"
#include "np2.h"

#if !defined(__GNUC__)
#pragma comment(lib, "winmm.lib")
#endif	// !defined(__GNUC__)

/*!< �n���h�� �}�b�v */
std::map<HMIDIIN, CComMidiIn32*> CComMidiIn32::sm_midiinMap;

/**
 * �C���X�^���X���쐬
 * @param[in] lpMidiIn �f�o�C�X��
 * @return �C���X�^���X
 */
CComMidiIn32* CComMidiIn32::CreateInstance(LPCTSTR lpMidiIn)
{
	UINT nId;
	if (!GetId(lpMidiIn, &nId))
	{
		return NULL;
	}

	HMIDIIN hMidiIn = NULL;
	if (::midiInOpen(&hMidiIn, nId, reinterpret_cast<DWORD_PTR>(g_hWndMain), 0, CALLBACK_WINDOW) != MMSYSERR_NOERROR)
	{
		return NULL;
	}
	return new CComMidiIn32(hMidiIn);
}

/**
 * �R���X�g���N�^
 * @param[in] hMidiIn �n���h��
 */
CComMidiIn32::CComMidiIn32(HMIDIIN hMidiIn)
	: m_hMidiIn(hMidiIn)
{
	::midiInReset(hMidiIn);

	sm_midiinMap[hMidiIn] = this;

	ZeroMemory(&m_midihdr, sizeof(m_midihdr));
	ZeroMemory(m_midiinBuffer, sizeof(m_midiinBuffer));

	m_midihdr.lpData = m_midiinBuffer;
	m_midihdr.dwBufferLength = sizeof(m_midiinBuffer);
	::midiInPrepareHeader(hMidiIn, &m_midihdr, sizeof(m_midihdr));
	::midiInAddBuffer(hMidiIn, &m_midihdr, sizeof(m_midihdr));
	::midiInStart(hMidiIn);
}

/**
 * �f�X�g���N�^
 */
CComMidiIn32::~CComMidiIn32()
{
	::midiInStop(m_hMidiIn);
	::midiInUnprepareHeader(m_hMidiIn, &m_midihdr, sizeof(m_midihdr));
	sm_midiinMap.erase(m_hMidiIn);

	::midiInReset(m_hMidiIn);
	::midiInClose(m_hMidiIn);
}

/**
 * �ǂݍ���
 * @param[out] pData �o�b�t�@
 * @return �T�C�Y
 */
UINT CComMidiIn32::Read(UINT8* pData)
{
	if (!m_buffer.empty())
	{
		*pData = m_buffer.front();
		m_buffer.pop_front();
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * ID �𓾂�
 * @param[in] lpMidiIn �f�o�C�X��
 * @param[out] pId ID
 * @retval true ����
 * @retval false ���s
 */
bool CComMidiIn32::GetId(LPCTSTR lpMidiIn, UINT* pId)
{
	const UINT nNum = ::midiInGetNumDevs();
	for (UINT i = 0; i < nNum; i++)
	{
		MIDIINCAPS mic;
		if (::midiInGetDevCaps(i, &mic, sizeof(mic)) != MMSYSERR_NOERROR)
		{
			continue;
		}
		if (!milstr_cmp(lpMidiIn, mic.szPname))
		{
			*pId = i;
			return true;
		}
	}
	return false;
}

/**
 * �C���X�^���X������
 * @param[in] hMidiIn �n���h��
 * @return �C���X�^���X
 */
CComMidiIn32* CComMidiIn32::GetInstance(HMIDIIN hMidiIn)
{
	std::map<HMIDIIN, CComMidiIn32*>::iterator it = sm_midiinMap.find(hMidiIn);
	if (it != sm_midiinMap.end())
	{
		return it->second;
	}
	return NULL;
}

/**
 * ���b�Z�[�W��M
 * @param[in] hMidiIn �n���h��
 * @param[in] nMessage ���b�Z�[�W
 */
void CComMidiIn32::RecvData(HMIDIIN hMidiIn, UINT nMessage)
{
	CComMidiIn32* pMidiIn32 = GetInstance(hMidiIn);
	if (pMidiIn32)
	{
		pMidiIn32->OnRecvData(nMessage);
	}
}

/**
 * ���b�Z�[�W��M
 * @param[in] nMessage ���b�Z�[�W
 */
void CComMidiIn32::OnRecvData(UINT nMessage)
{
	switch (nMessage & 0xf0)
	{
		case 0xc0:
		case 0xd0:
			m_buffer.push_back(static_cast<char>(nMessage));
			m_buffer.push_back(static_cast<char>(nMessage >> 8));
			break;

		case 0x80:
		case 0x90:
		case 0xa0:
		case 0xb0:
		case 0xe0:
			m_buffer.push_back(static_cast<char>(nMessage));
			m_buffer.push_back(static_cast<char>(nMessage >> 8));
			m_buffer.push_back(static_cast<char>(nMessage >> 16));
			break;
	}
}

/**
 * ���b�Z�[�W��M
 * @param[in] hMidiIn �n���h��
 * @param[in] lpMidiHdr ���b�Z�[�W
 */
void CComMidiIn32::RecvExcv(HMIDIIN hMidiIn, MIDIHDR* lpMidiHdr)
{
	CComMidiIn32* pMidiIn32 = GetInstance(hMidiIn);
	if (pMidiIn32)
	{
		pMidiIn32->OnRecvExcv(lpMidiHdr);
	}
}

/**
 * ���b�Z�[�W��M
 * @param[in] lpMidiHdr ���b�Z�[�W
 */
void CComMidiIn32::OnRecvExcv(MIDIHDR* lpMidiHdr)
{
	for (DWORD i = 0; i < lpMidiHdr->dwBytesRecorded; i++)
	{
		m_buffer.push_back(lpMidiHdr->lpData[i]);
	}

	::midiInUnprepareHeader(m_hMidiIn, &m_midihdr, sizeof(m_midihdr));
	::midiInPrepareHeader(m_hMidiIn, &m_midihdr, sizeof(m_midihdr));
	::midiInAddBuffer(m_hMidiIn, &m_midihdr, sizeof(m_midihdr));
}
