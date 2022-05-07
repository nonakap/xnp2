/**
 * @file	PropProc.h
 * @brief	�v���p�e�B �V�[�g �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <vector>
#include "DlgProc.h"
#include "tstring.h"

/**
 * @brief �v���p�e�B �V�[�g �y�[�W
 */
class CPropPageProc : public CDlgProc
{
public:
	PROPSHEETPAGE m_psp;			//!< �v���p�e�B �V�[�g �y�[�W�\����

public:
	CPropPageProc(UINT nIDTemplate, UINT nIDCaption = 0);
	CPropPageProc(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);
	virtual ~CPropPageProc();
	void Construct(UINT nIDTemplate, UINT nIDCaption = 0);
	void Construct(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);

protected:
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnApply();
	virtual void OnReset();
	virtual void OnOK();
	virtual void OnCancel();

private:
	LPTSTR m_lpCaption;				//!< �L���v�V����
	static UINT CALLBACK PropPageCallback(HWND hWnd, UINT message, LPPROPSHEETPAGE pPropPage);
};

/**
 * @brief �v���p�e�B �V�[�g
 */
class CPropSheetProc /* : public CWnd */
{
public:
	PROPSHEETHEADER m_psh;					//!< �v���p�e�B �V�[�g �w�b�_�\����

public:
	CPropSheetProc();
	CPropSheetProc(UINT nIDCaption, HWND hwndParent = NULL, UINT iSelectPage = 0);
	CPropSheetProc(LPCTSTR pszCaption, HWND hwndParent = NULL, UINT iSelectPage = 0);
	INT_PTR DoModal();
	void AddPage(CPropPageProc* pPage);

protected:
	std::vector<CPropPageProc*> m_pages;	//!< The array of CPropPageProc pointers
	std::tstring m_strCaption;				//!< The caption

	void CommonConstruct(HWND hwndParent, UINT iSelectPage);
};
