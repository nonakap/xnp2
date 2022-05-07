/**
 * @file	externalopl3.h
 * @brief	�O�� OPL3 ���t�N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "externalchip.h"

/**
 * @brief �O�� OPL3 ���t�N���X
 */
class CExternalOpl3 : public IExternalChip
{
public:
	CExternalOpl3(IExternalChip* pChip);
	virtual ~CExternalOpl3();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter);

protected:
	IExternalChip* m_pChip;				/*!< �`�b�v*/
	UINT8 m_cKon[2][16];				/*!< KON �e�[�u�� */

	void Mute(bool bMute);
	void WriteRegisterInner(UINT nAddr, UINT8 cData) const;
};
