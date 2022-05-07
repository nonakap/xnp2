/**
 * @file	vstmidievent.h
 * @brief	VST MIDI �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <vector>
#include <pluginterfaces/vst2.x/aeffectx.h>

/**
 * @brief VST MIDI �N���X
 */
class CVstMidiEvent
{
public:
	CVstMidiEvent();
	~CVstMidiEvent();
	void Clear();
	void ShortMessage(UINT nTick, UINT nMessage);
	void LongMessage(UINT nTick, const void* lpMessage, UINT cbMessage);
	const VstEvents* GetEvents();

protected:

private:
	UINT m_nEvents;							/*!< �C�x���g�� */
	std::vector<unsigned char> m_header;	/*!< �w�b�_ */
	std::vector<unsigned char> m_event;		/*!< �C�x���g */
	void Add(const VstEvent* pEvent, const void* lpMessage = NULL, UINT cbMessage = 0);
};
