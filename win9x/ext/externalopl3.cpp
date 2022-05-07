/**
 * @file	externalopl3.cpp
 * @brief	�O�� OPL3 ���t�N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "externalopl3.h"

/**
 * �R���X�g���N�^
 * @param[in] pChip �`�b�v
 */
CExternalOpl3::CExternalOpl3(IExternalChip* pChip)
	: m_pChip(pChip)
{
	memset(m_cKon, 0x00, sizeof(m_cKon));
}

/**
 * �f�X�g���N�^
 */
CExternalOpl3::~CExternalOpl3()
{
	delete m_pChip;
}

/**
 * �`�b�v �^�C�v�𓾂�
 * @return �`�b�v �^�C�v
 */
IExternalChip::ChipType CExternalOpl3::GetChipType()
{
	return m_pChip->GetChipType();
}

/**
 * �������Z�b�g
 */
void CExternalOpl3::Reset()
{
	m_pChip->Reset();
}

/**
 * ���W�X�^��������
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CExternalOpl3::WriteRegister(UINT nAddr, UINT8 cData)
{
	if ((nAddr & 0xf0) == 0xb0)
	{
		m_cKon[(nAddr & 0x100) >> 8][nAddr & 0x0f] = cData;
	}
	WriteRegisterInner(nAddr, cData);
}

/**
 * ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 * @param[in] nParameter �p�����[�^
 * @return ����
 */
INTPTR CExternalOpl3::Message(UINT nMessage, INTPTR nParameter)
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
void CExternalOpl3::Mute(bool bMute)
{
	if (bMute)
	{
		for (UINT i = 0; i < 2; i++)
		{
			for (UINT j = 0; j < 9; j++)
			{
				if (m_cKon[i][j] & 0x20)
				{
					m_cKon[i][j] &= 0xdf;
					WriteRegisterInner((i * 0x100) + j + 0xb0, m_cKon[i][j]);
				}
			}
		}
	}
}

/**
 * ���W�X�^��������(����)
 * @param[in] nAddr �A�h���X
 * @param[in] cData �f�[�^
 */
void CExternalOpl3::WriteRegisterInner(UINT nAddr, UINT8 cData) const
{
	m_pChip->WriteRegister(nAddr, cData);
}
