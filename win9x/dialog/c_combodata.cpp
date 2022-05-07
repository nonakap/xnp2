/**
 * @file	c_combodata.cpp
 * @brief	�R���{ �f�[�^ �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "c_combodata.h"

/**
 * �ǉ�
 * @param[in] lpValues �l�̔z��
 * @param[in] cchValues �l�̐�
 */
void CComboData::Add(const UINT32* lpValues, UINT cchValues)
{
	for (UINT i = 0; i < cchValues; i++)
	{
		Add(lpValues[i]);
	}
}

/**
 * �ǉ�
 * @param[in] lpValues �l�̔z��
 * @param[in] cchValues �l�̐�
 */
void CComboData::Add(const Value* lpValues, UINT cchValues)
{
	for (UINT i = 0; i < cchValues; i++)
	{
		Add(lpValues[i].nNumber, lpValues[i].nItemData);
	}
}

/**
 * �ǉ�
 * @param[in] lpEntries �G���g���̔z��
 * @param[in] cchEntries �G���g���̐�
 */
void CComboData::Add(const Entry* lpEntries, UINT cchEntries)
{
	for (UINT i = 0; i < cchEntries; i++)
	{
		std::tstring rString(LoadTString(lpEntries[i].lpcszString));
		Add(rString.c_str(), lpEntries[i].nItemData);
	}
}

/**
 * �ǉ�
 * @param[in] nValue �l
 * @return �C���f�b�N�X
 */
int CComboData::Add(UINT32 nValue)
{
	return Add(nValue, nValue);
}

/**
 * �ǉ�
 * @param[in] nValue �l
 * @param[in] nItemData �f�[�^
 * @return �C���f�b�N�X
 */
int CComboData::Add(UINT32 nValue, UINT32 nItemData)
{
	TCHAR szStr[16];
	wsprintf(szStr, TEXT("%u"), nValue);
	return Add(szStr, nItemData);
}

/**
 * �ǉ�
 * @param[in] lpString �\����
 * @param[in] nItemData �f�[�^
 * @return �C���f�b�N�X
 */
int CComboData::Add(LPCTSTR lpString, UINT32 nItemData)
{
	const int nIndex = AddString(lpString);
	if (nIndex >= 0)
	{
		SetItemData(nIndex, static_cast<DWORD_PTR>(nItemData));
	}
	return nIndex;
}

/**
 * �A�C�e������
 * @param[in] nValue �l
 * @return �C���f�b�N�X
 */
int CComboData::FindItemData(UINT32 nValue) const
{
	const int nItems = GetCount();
	for (int i = 0; i < nItems; i++)
	{
		if (GetItemData(i) == nValue)
		{
			return i;
		}
	}
	return CB_ERR;
}

/**
 * �J�[�\���ݒ�
 * @param[in] nValue �l
 * @retval true ����
 * @retval false ���s
 */
bool CComboData::SetCurItemData(UINT32 nValue)
{
	const int nIndex = FindItemData(nValue);
	if (nIndex == CB_ERR)
	{
		return false;
	}
	SetCurSel(nIndex);
	return true;
}

/**
 * �J�[�\���̒l���擾
 * @param[in] nDefault �f�t�H���g�l
 * @return �l
 */
UINT32 CComboData::GetCurItemData(UINT32 nDefault) const
{
	const int nIndex = GetCurSel();
	if (nIndex >= 0)
	{
		return static_cast<UINT32>(GetItemData(nIndex));
	}
	return nDefault;
}
