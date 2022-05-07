/**
 * @file	externalopm.h
 * @brief	�O�� OPM ���t�N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "externalchip.h"

/**
 * @brief �O�� OPM ���t�N���X
 */
class CExternalOpm : public IExternalChip
{
public:
	CExternalOpm(IExternalChip* pChip);
	virtual ~CExternalOpm();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter);

protected:
	IExternalChip* m_pChip;				/*!< �`�b�v */
	UINT8 m_cAlgorithm[8];				/*!< �A���S���Y�� �e�[�u�� */
	UINT8 m_cTtl[8 * 4];				/*!< TTL �e�[�u�� */

	void Mute(bool bMute) const;
	void WriteRegisterInner(UINT nAddr, UINT8 cData) const;
	void SetVolume(UINT nChannel, int nVolume) const;
};
