/**
 * @file	cmmidiout32.cpp
 * @brief	MIDI OUT win32 �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "cmmidiout32.h"
#include "cmmidi.h"

#if !defined(__GNUC__)
#pragma comment(lib, "winmm.lib")
#endif	// !defined(__GNUC__)

/**
 * �C���X�^���X���쐬
 * @param[in] lpMidiOut �f�o�C�X��
 * @return �C���X�^���X
 */
CComMidiOut32* CComMidiOut32::CreateInstance(LPCTSTR lpMidiOut)
{
	UINT nId;
	if (!GetId(lpMidiOut, &nId))
	{
		return NULL;
	}

	HMIDIOUT hMidiOut = NULL;
	if (::midiOutOpen(&hMidiOut, nId, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR)
	{
		return NULL;
	}
	return new CComMidiOut32(hMidiOut);
}

/**
 * �R���X�g���N�^
 * @param[in] hMidiOut �n���h��
 */
CComMidiOut32::CComMidiOut32(HMIDIOUT hMidiOut)
	: m_hMidiOut(hMidiOut)
	, m_bWaitingSentExclusive(false)
{
	ZeroMemory(&m_midihdr, sizeof(m_midihdr));
	::midiOutReset(m_hMidiOut);
}

/**
 * �f�X�g���N�^
 */
CComMidiOut32::~CComMidiOut32()
{
	WaitSentExclusive();
	::midiOutReset(m_hMidiOut);
	::midiOutClose(m_hMidiOut);
}

/**
 * �G�N�X�N���[�V�u���M������҂�
 */
void CComMidiOut32::WaitSentExclusive()
{
	if (m_bWaitingSentExclusive)
	{
		m_bWaitingSentExclusive = false;
		while (midiOutUnprepareHeader(m_hMidiOut, &m_midihdr, sizeof(m_midihdr)) == MIDIERR_STILLPLAYING)
		{
		}
	}
}

/**
 * �V���[�g ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 */
void CComMidiOut32::Short(UINT32 nMessage)
{
	WaitSentExclusive();
	::midiOutShortMsg(m_hMidiOut, nMessage);
}

/**
 * �����O ���b�Z�[�W
 * @param[in] lpMessage ���b�Z�[�W �|�C���^
 * @param[in] cbMessage ���b�Z�[�W �T�C�Y
 */
void CComMidiOut32::Long(const UINT8* lpMessage, UINT cbMessage)
{
	if (cbMessage == 0)
	{
		return;
	}

	WaitSentExclusive();

	m_excvbuf.resize(cbMessage);
	CopyMemory(&m_excvbuf[0], lpMessage, cbMessage);

	m_midihdr.lpData = &m_excvbuf[0];
	m_midihdr.dwFlags = 0;
	m_midihdr.dwBufferLength = cbMessage;
	::midiOutPrepareHeader(m_hMidiOut, &m_midihdr, sizeof(m_midihdr));
	::midiOutLongMsg(m_hMidiOut, &m_midihdr, sizeof(m_midihdr));
	m_bWaitingSentExclusive = true;
}

/**
 * ID �𓾂�
 * @param[in] lpMidiOut �f�o�C�X��
 * @param[out] pId ID
 * @retval true ����
 * @retval false ���s
 */
bool CComMidiOut32::GetId(LPCTSTR lpMidiOut, UINT* pId)
{
	const UINT nNum = ::midiOutGetNumDevs();
	for (UINT i = 0; i < nNum; i++)
	{
		MIDIOUTCAPS moc;
		if (midiOutGetDevCaps(i, &moc, sizeof(moc)) != MMSYSERR_NOERROR)
		{
			continue;
		}
		if (!milstr_cmp(lpMidiOut, moc.szPname))
		{
			*pId = i;
			return true;
		}
	}

	if (!milstr_cmp(lpMidiOut, cmmidi_midimapper))
	{
		*pId = MIDI_MAPPER;
		return true;
	}
	return false;
}
