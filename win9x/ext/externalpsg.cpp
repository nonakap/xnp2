/**
 * @file	externalpsg.cpp
 * @brief	�O�� PSG ���t�N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "externalpsg.h"

/**
 * �R���X�g���N�^
 * @param[in] pChip �`�b�v
 */
CExternalPsg::CExternalPsg(IExternalChip* pChip)
	: m_pChip(pChip)
	, m_cPsgMix(0x3f)
{
}

/**
 * �f�X�g���N�^
 */
CExternalPsg::~CExternalPsg()
{
	delete m_pChip;
}

/**
 * �`�b�v �^�C�v�𓾂�
 * @return �`�b�v �^�C�v
 */
IExternalChip::ChipType CExternalPsg::GetChipType()
{
	return m_pChip->GetChipType();
}

/**
 * �������Z�b�g
 */
void CExternalPsg::Reset()
{
	m_cPsgMix = 0x3f;
	m_pChip->Reset();
}

/**
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CExternalPsg::WriteRegister(UINT nAddr, UINT8 cData)
{
	if (nAddr < 0x0e)
	{
		if (nAddr == 0x07)
		{
			// psg mix
			cData &= 0x3f;
			if (m_cPsgMix == cData)
			{
				return;
			}
			m_cPsgMix = cData;
		}
		WriteRegisterInner(nAddr, cData);
	}
}

/**
 * ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 * @param[in] nParameter �p�����[�^
 * @return ����
 */
INTPTR CExternalPsg::Message(UINT nMessage, INTPTR nParameter)
{
	switch (nMessage)
	{
		case kMute:
			Mute(nParameter != 0);
			break;
	}
	return 0;
}

/**
 * �~���[�g
 * @param[in] bMute �~���[�g
 */
void CExternalPsg::Mute(bool bMute) const
{
	WriteRegisterInner(0x07, (bMute) ? 0x3f : m_cPsgMix);
}

/**
 * ���W�X�^��������(����)
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CExternalPsg::WriteRegisterInner(UINT nAddr, UINT8 cData) const
{
	m_pChip->WriteRegister(nAddr, cData);
}
