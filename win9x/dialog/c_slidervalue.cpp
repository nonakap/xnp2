/**
 * @file	c_slidervalue.cpp
 * @brief	�l�t���X���C�_�[ �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "c_slidervalue.h"

/**
 * �R���X�g���N�^
 */
CSliderValue::CSliderValue()
	: m_nStaticId(0)
{
}

/**
 * �͈͂̐ݒ�
 * @param[in] nMin �ŏ��l
 * @param[in] nMax �ő�l
 * @param[in] bRedraw �ĕ`��t���O
 */
void CSliderValue::SetRange(int nMin, int nMax, BOOL bRedraw)
{
	SetRangeMin(nMin, FALSE);
	SetRangeMax(nMax, bRedraw);
}

/**
 * �l�̐ݒ�
 * @param[in] nPos �l
 */
void CSliderValue::SetPos(int nPos)
{
	CSliderProc::SetPos(nPos);
	UpdateValue();
}

/**
 * �l�̍X�V
 */
void CSliderValue::UpdateValue()
{
	if (m_nStaticId)
	{
		GetParent().SetDlgItemInt(m_nStaticId, GetPos(), TRUE);
	}
}
