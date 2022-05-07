/**
 * @file	cmmidioutvermouth.cpp
 * @brief	MIDI OUT Vermouth �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "cmmidioutvermouth.h"

#if defined(VERMOUTH_LIB)

//! �n���h��
extern MIDIMOD vermouth_module;

/**
 * �C���X�^���X���쐬
 * @return �C���X�^���X
 */
CComMidiOutVermouth* CComMidiOutVermouth::CreateInstance()
{
	MIDIHDL vermouth = midiout_create(vermouth_module, 512);
	if (vermouth == NULL)
	{
		return NULL;
	}
	return new CComMidiOutVermouth(vermouth);
}

/**
 * �R���X�g���N�^
 * @param[in] vermouth �n���h��
 */
CComMidiOutVermouth::CComMidiOutVermouth(MIDIHDL vermouth)
	: m_vermouth(vermouth)
{
	::sound_streamregist(m_vermouth, reinterpret_cast<SOUNDCB>(GetPcm));
}

/**
 * �f�X�g���N�^
 */
CComMidiOutVermouth::~CComMidiOutVermouth()
{
	::midiout_destroy(m_vermouth);
}

/**
 * �V���[�g ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 */
void CComMidiOutVermouth::Short(UINT32 nMessage)
{
	sound_sync();
	::midiout_shortmsg(m_vermouth, nMessage);
}

/**
 * �����O ���b�Z�[�W
 * @param[in] lpMessage ���b�Z�[�W �|�C���^
 * @param[in] cbMessage ���b�Z�[�W �T�C�Y
 */
void CComMidiOutVermouth::Long(const UINT8* lpMessage, UINT cbMessage)
{
	sound_sync();
	::midiout_longmsg(m_vermouth, lpMessage, cbMessage);
}

/**
 * �v���Z�X
 * @param[in] vermouth �n���h��
 * @param[out] lpBuffer �o�b�t�@
 * @param[in] nBufferCount �T���v����
 */
void SOUNDCALL CComMidiOutVermouth::GetPcm(MIDIHDL vermouth, SINT32* lpBuffer, UINT nBufferCount)
{
	while (nBufferCount)
	{
		UINT nSize = nBufferCount;
		const SINT32* ptr = ::midiout_get(vermouth, &nSize);
		if (ptr == NULL)
		{
			break;
		}
		nBufferCount -= nSize;
		do
		{
			lpBuffer[0] += ptr[0];
			lpBuffer[1] += ptr[1];
			ptr += 2;
			lpBuffer += 2;
		} while (--nSize);
	}
}

#endif	// defined(VERMOUTH_LIB)
