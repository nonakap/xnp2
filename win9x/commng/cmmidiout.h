/**
 * @file	cmmidiout.h
 * @brief	MIDI OUT ���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief MIDI OUT ���N���X
 */
class CComMidiOut
{
public:
	/**
	 * �f�X�g���N�^
	 */
	virtual ~CComMidiOut()
	{
	}

	/**
	 * �V���[�g ���b�Z�[�W
	 * @param[in] nMessage ���b�Z�[�W
	 */
	virtual void Short(UINT32 nMessage)
	{
	}

	/**
	 * �����O ���b�Z�[�W
	 * @param[in] lpMessage ���b�Z�[�W �|�C���^
	 * @param[in] cbMessage ���b�Z�[�W �T�C�Y
	 */
	virtual void Long(const UINT8* lpMessage, UINT cbMessage)
	{
	}
};
