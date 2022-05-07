/**
 * @file	cmmidioutvst.h
 * @brief	MIDI OUT VST �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#if defined(SUPPORT_VSTi)

#include "cmmidiout.h"
#include "sound.h"
#include "vsthost\vstbuffer.h"
#include "vsthost\vsteditwnd.h"
#include "vsthost\vsteffect.h"
#include "vsthost\vstmidievent.h"

/**
 * @brief MIDI OUT VST �N���X
 */
class CComMidiOutVst : public CComMidiOut
{
public:
	static bool IsEnabled();
	static CComMidiOutVst* CreateInstance();

	CComMidiOutVst();
	virtual ~CComMidiOutVst();
	virtual void Short(UINT32 nMessage);
	virtual void Long(const UINT8* lpMessage, UINT cbMessage);

private:
	UINT m_nBlockSize;			/*!< �u���b�N �T�C�Y */
	UINT m_nIndex;				/*!< �ǂݎ��C���f�b�N�X */
	CVstEffect m_effect;		/*!< �G�t�F�N�g */
	CVstEditWnd m_wnd;			/*!< �E�B���h�E */
	CVstMidiEvent m_event;		/*!< �C�x���g */
	CVstBuffer m_input;			/*!< ���̓o�b�t�@ */
	CVstBuffer m_output;		/*!< �o�̓o�b�t�@ */

	bool Initialize(LPCTSTR lpPath);
	static void SOUNDCALL GetPcm(CComMidiOutVst*, SINT32* lpBuffer, UINT nBufferCount);
	void Process32(SINT32* lpBuffer, UINT nBufferCount);
};

#endif	// defined(SUPPORT_VSTi)
