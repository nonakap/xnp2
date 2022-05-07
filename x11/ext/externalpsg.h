/**
 * @file	externalpsg.h
 * @brief	�O�� PSG ���t�N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "externalchip.h"

/**
 * @brief �O�� PSG ���t�N���X
 */
class CExternalPsg : public IExternalChip
{
public:
	CExternalPsg(IExternalChip* pChip);
	virtual ~CExternalPsg();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter);

protected:
	IExternalChip* m_pChip;				//!< �`�b�v
	UINT8 m_cPsgMix;					//!< PSG �~�L�T�[

	virtual void Mute(bool bMute) const;
	void WriteRegisterInner(UINT nAddr, UINT8 cData) const;
};
