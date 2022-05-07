/**
 * @file	c_midi.h
 * @brief	MIDI �R���g���[�� �N���X�Q�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "misc\DlgProc.h"

/**
 * @brief MIDI �f�o�C�X �N���X
 */
class CComboMidiDevice : public CComboBoxProc
{
public:
	virtual void PreSubclassWindow();
	void EnumerateMidiIn();
	void EnumerateMidiOut();
	void SetCurString(LPCTSTR lpDevice);
};

/**
 * @brief MIDI ���W���[�� �N���X
 */
class CComboMidiModule : public CComboBoxProc
{
public:
	virtual void PreSubclassWindow();
};

/**
 * @brief MIMPI �t�@�C��
 */
class CEditMimpiFile : public CWndProc
{
public:
	void Browse();
};
