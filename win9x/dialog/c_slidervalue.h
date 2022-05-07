/**
 * @file	c_slidervalue.h
 * @brief	�l�t���X���C�_�[ �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "misc/DlgProc.h"

/**
 * @brief �X���C�_�[ �N���X
 */
class CSliderValue : public CSliderProc
{
public:
	CSliderValue();
	void SetRange(int nMin, int nMax, BOOL bRedraw = FALSE);
	void SetPos(int nPos);
	void SetStaticId(UINT nId);
	void UpdateValue();

private:
	UINT m_nStaticId;			/*!< �l�R���g���[�� */
};

/**
 * �l�R���g���[���̎w��
 * @param[in] nId �R���g���[�� ID
 */
inline void CSliderValue::SetStaticId(UINT nId)
{
	m_nStaticId = nId;
}
