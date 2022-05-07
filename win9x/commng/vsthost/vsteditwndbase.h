/**
 * @file	vsteditwndbase.h
 * @brief	VST edit �E�B���h�E���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief VST edit �E�B���h�E���N���X
 */
class IVstEditWnd
{
public:
	virtual bool OnResize(int nWidth, int nHeight) = 0;
	virtual bool OnUpdateDisplay() = 0;

};
