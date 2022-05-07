/**
 * @file	WndProc.h
 * @brief	�v���V�[�W�� �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <map>
#include "WndBase.h"

/**
 * @brief �v���V�[�W�� �N���X
 */
class CWndProc : public CWndBase
{
public:
	static void Initialize(HINSTANCE hInstance);
	static void Deinitialize();
	static HINSTANCE GetInstanceHandle();
	static void SetResourceHandle(HINSTANCE hInstance);
	static HINSTANCE GetResourceHandle();
	static HINSTANCE FindResourceHandle(LPCTSTR lpszName, LPCTSTR lpszType);

	CWndProc();
	virtual ~CWndProc();

	operator HWND() const;
	HWND GetSafeHwnd() const;
	static CWndProc* FromHandlePermanent(HWND hWnd);
	BOOL Attach(HWND hWndNew);
	HWND Detach();

	virtual void PreSubclassWindow();
	BOOL SubclassWindow(HWND hWnd);
	BOOL SubclassDlgItem(UINT nID, CWndProc* pParent);
	HWND UnsubclassWindow();

	BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu, LPVOID lpParam = NULL);
	virtual BOOL DestroyWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnNcDestroy(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	LRESULT DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();

protected:
	static HINSTANCE sm_hInstance;		//!< �C���X�^���X �n���h��
	static HINSTANCE sm_hResource;		//!< ���\�[�X �n���h��
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static void HookWindowCreate(CWndProc* pWnd);
	static bool UnhookWindowCreate();

private:
	static DWORD sm_dwThreadId;						//!< �����̃X���b�h ID
	static HHOOK sm_hHookOldCbtFilter;				//!< �t�b�N �t�B���^�[
	static CWndProc* sm_pWndInit;					//!< ���������̃C���X�^���X
	static std::map<HWND, CWndProc*>* sm_pWndMap;	//!< �E�B���h�E �}�b�v
	WNDPROC m_pfnSuper;								//!< ���ʃv���V�[�W��
	static LRESULT CALLBACK CbtFilterHook(int nCode, WPARAM wParam, LPARAM lParam);
};

/**
 * �C���X�^���X �n���h�����擾
 * @return �C���X�^���X �n���h��
 */
inline HINSTANCE CWndProc::GetInstanceHandle()
{
	return sm_hInstance;
}

/**
 * ���\�[�X �n���h����ݒ�
 * @param[in] hInstance ���\�[�X �n���h��
 */
inline void CWndProc::SetResourceHandle(HINSTANCE hInstance)
{
	sm_hResource = hInstance;
}

/**
 * ���\�[�X �n���h�����擾
 * @return ���\�[�X �n���h��
 */
inline HINSTANCE CWndProc::GetResourceHandle()
{
	return sm_hResource;
}

/**
 * HWND �I�y���[�^
 * @return HWND
 */
inline CWndProc::operator HWND() const
{
	return m_hWnd;
}

/**
 * �E�B���h�E�̃E�B���h�E �n���h����Ԃ��܂�
 * @return �E�B���h�E �n���h��
 */
inline HWND CWndProc::GetSafeHwnd() const
{
	return (this != NULL) ? m_hWnd : NULL;
}

