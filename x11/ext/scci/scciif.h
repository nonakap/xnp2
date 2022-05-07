/**
 * @file	scciif.h
 * @brief	SCCI �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "../externalchip.h"

namespace scci
{
	class SoundChip;
	class SoundInterfaceManager;
}

/**
 * @brief SCCI �A�N�Z�X �N���X
 */
class CScciIf
{
public:
	CScciIf();
	~CScciIf();
	bool Initialize();
	void Deinitialize();
	void Reset();
	IExternalChip* GetInterface(IExternalChip::ChipType nChipType, UINT nClock);

private:
	scci::SoundInterfaceManager* m_pManager;	/*!< �}�l�[�W�� */

	/**
	 * @brief �`�b�v �N���X
	 */
	class Chip : public IExternalChip
	{
	public:
		Chip(CScciIf* pScciIf, scci::SoundChip* pSoundChip);
		virtual ~Chip();
		operator scci::SoundChip*();
		virtual ChipType GetChipType();
		virtual void Reset();
		virtual void WriteRegister(UINT nAddr, UINT8 cData);
		virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

	private:
		CScciIf* m_pScciIf;				/*!< �e�C���X�^���X */
		scci::SoundChip* m_pSoundChip;	/*!< �`�b�v �C���X�^���X */
	};

	void Detach(Chip* pChip);
	friend class Chip;
};
