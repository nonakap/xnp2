/**
 * @file	externalopna.h
 * @brief	�O�� OPNA ���t�N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "externalpsg.h"

/**
 * @brief �O�� OPNA ���t�N���X
 */
class CExternalOpna : public CExternalPsg
{
public:
	CExternalOpna(IExternalChip* pChip);
	virtual ~CExternalOpna();
	bool HasPsg() const;
	bool HasRhythm() const;
	bool HasADPCM() const;
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);

protected:
	bool m_bHasPsg;						/*!< PSG */
	bool m_bHasExtend;					/*!< Extend */
	bool m_bHasRhythm;					/*!< Rhythm */
	bool m_bHasADPCM;					/*!< ADPCM */
	UINT8 m_cMode;						/*!< ���[�h */
	UINT8 m_cAlgorithm[8];				/*!< �A���S���Y�� �e�[�u�� */
	UINT8 m_cTtl[8 * 4];				/*!< TTL �e�[�u�� */

	virtual void Mute(bool bMute) const;
	void SetVolume(UINT nChannel, int nVolume) const;
};

/**
 * PSG �������Ă���?
 * @retval true �L��
 * @retval false ����
 */
inline bool CExternalOpna::HasPsg() const
{
	return m_bHasPsg;
}

/**
 * Rhythm �������Ă���?
 * @retval true �L��
 * @retval false ����
 */
inline bool CExternalOpna::HasRhythm() const
{
	return m_bHasRhythm;
}

/**
 * ADPCM �̃o�b�t�@�������Ă���?
 * @retval true �L��
 * @retval false ����
 */
inline bool CExternalOpna::HasADPCM() const
{
	return m_bHasADPCM;
}
