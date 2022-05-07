/**
 * @file	cmmidioutmt32sound.cpp
 * @brief	MIDI OUT MT32Sound �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "cmmidioutmt32sound.h"

#if defined(MT32SOUND_DLL)

#include "ext\mt32snd.h"

/**
 * �C���X�^���X���쐬
 * @return �C���X�^���X
 */
CComMidiOutMT32Sound* CComMidiOutMT32Sound::CreateInstance()
{
	MT32Sound* pMT32Sound = MT32Sound::GetInstance();
	if (!pMT32Sound->Open())
	{
		return NULL;
	}
	return new CComMidiOutMT32Sound(pMT32Sound);
}

/**
 * �R���X�g���N�^
 * @param[in] pMT32Sound �n���h��
 */
CComMidiOutMT32Sound::CComMidiOutMT32Sound(MT32Sound* pMT32Sound)
	: m_pMT32Sound(pMT32Sound)
{
	::sound_streamregist(m_pMT32Sound, reinterpret_cast<SOUNDCB>(GetPcm));
}

/**
 * �f�X�g���N�^
 */
CComMidiOutMT32Sound::~CComMidiOutMT32Sound()
{
	m_pMT32Sound->Close();
}

/**
 * �V���[�g ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 */
void CComMidiOutMT32Sound::Short(UINT32 nMessage)
{
	sound_sync();
	m_pMT32Sound->ShortMsg(nMessage);
}

/**
 * �����O ���b�Z�[�W
 * @param[in] lpMessage ���b�Z�[�W �|�C���^
 * @param[in] cbMessage ���b�Z�[�W �T�C�Y
 */
void CComMidiOutMT32Sound::Long(const UINT8* lpMessage, UINT cbMessage)
{
	sound_sync();
	m_pMT32Sound->LongMsg(lpMessage, cbMessage);
}

/**
 * �v���Z�X
 * @param[in] pMT32Sound �n���h��
 * @param[out] lpBuffer �o�b�t�@
 * @param[in] nBufferCount �T���v����
 */
void SOUNDCALL CComMidiOutMT32Sound::GetPcm(MT32Sound* pMT32Sound, SINT32* lpBuffer, UINT nBufferCount)
{
	pMT32Sound->Mix(lpBuffer, nBufferCount);
}

#endif	// defined(MT32SOUND_DLL)
