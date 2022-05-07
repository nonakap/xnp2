/**
 * @file	viewmem.h
 * @brief	DebugUty �p�������ǂݍ��݃N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief �������ǂݍ���
 */
struct DebugUtyViewMemory
{
	UINT8	vram;		//!< VRAM �o���N
	UINT8	itf;		//!< ITF �o���N
	UINT8	A20;		//!< A20 ���C��

	DebugUtyViewMemory();
	void Update();
	void Read(UINT32 nAddress, LPVOID lpBuffer, UINT32 cbBuffer) const;
};
