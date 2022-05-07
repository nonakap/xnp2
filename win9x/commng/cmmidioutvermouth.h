/**
 * @file	cmmidioutvermouth.h
 * @brief	MIDI OUT Vermouth �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#if defined(VERMOUTH_LIB)

#include "cmmidiout.h"
#include "sound.h"
#include "sound\vermouth\vermouth.h"

/**
 * @brief MIDI OUT Vermouth�N���X
 */
class CComMidiOutVermouth : public CComMidiOut
{
public:
	static CComMidiOutVermouth* CreateInstance();

	CComMidiOutVermouth(MIDIHDL vermouth);
	virtual ~CComMidiOutVermouth();
	virtual void Short(UINT32 nMessage);
	virtual void Long(const UINT8* lpMessage, UINT cbMessage);

private:
	MIDIHDL m_vermouth;			/*!< The instance of vermouth */
	static void SOUNDCALL GetPcm(MIDIHDL vermouth, SINT32* lpBuffer, UINT nBufferCount);
};

#endif	// defined(VERMOUTH_LIB)
