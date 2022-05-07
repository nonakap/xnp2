/**
 * @file	cmmidiout32.h
 * @brief	MIDI OUT win32 �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <vector>
#include "cmmidiout.h"

/**
 * @brief MIDI OUT win32 �N���X
 */
class CComMidiOut32 : public CComMidiOut
{
public:
	static CComMidiOut32* CreateInstance(LPCTSTR lpMidiOut);

	CComMidiOut32(HMIDIOUT hMidiOut);
	virtual ~CComMidiOut32();
	virtual void Short(UINT32 nMessage);
	virtual void Long(const UINT8* lpMessage, UINT cbMessage);
	static bool GetId(LPCTSTR lpMidiOut, UINT* pId);

private:
	HMIDIOUT m_hMidiOut;					/*!< MIDIOUT �n���h�� */
	MIDIHDR m_midihdr;						/*!< MIDIHDR */
	bool m_bWaitingSentExclusive;			/*!< �G�N�X�N���[�V�����M�� */
	std::vector<char> m_excvbuf;			/*!< �G�N�X�N���[�V�� �o�b�t�@ */

	void WaitSentExclusive();
};
