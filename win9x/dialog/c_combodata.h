/**
 * @file	c_combodata.h
 * @brief	�R���{ �f�[�^ �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "misc/DlgProc.h"

/**
 * @brief �R���{ �f�[�^ �N���X
 */
class CComboData : public CComboBoxProc
{
public:
	/**
	 * @brief �G���g���[
	 */
	struct Value
	{
		UINT32 nNumber;				/*!< ���l */
		UINT32 nItemData;			/*!< �l */
	};

	/**
	 * @brief �G���g���[
	 */
	struct Entry
	{
		LPCTSTR lpcszString;		/*!< ������ */
		UINT32 nItemData;			/*!< �l */
	};

	void Add(const UINT32* lpValues, UINT cchValues);
	void Add(const Value* lpValues, UINT cchValues);
	void Add(const Entry* lpEntries, UINT cchEntries);
	int Add(UINT32 nValue);
	int Add(UINT32 nValue, UINT32 nItemData);
	int Add(LPCTSTR lpString, UINT32 nItemData);
	int FindItemData(UINT32 nValue) const;
	bool SetCurItemData(UINT32 nValue);
	UINT32 GetCurItemData(UINT32 nDefault) const;
};
