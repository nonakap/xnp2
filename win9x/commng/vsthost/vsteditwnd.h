/**
 * @file	vsteditwnd.h
 * @brief	VST edit �E�B���h�E �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <map>
#include "vsteditwndbase.h"

class CVstEffect;

/**
 * @brief VST effect �E�B���h�E �N���X
 */
class CVstEditWnd : public IVstEditWnd
{
public:
	static bool Initialize(HINSTANCE hInstance);
	static void OnIdle();

	CVstEditWnd();
	virtual ~CVstEditWnd();
	bool Create(CVstEffect* pEffect, LPCTSTR lpszWindowName, DWORD dwStyle, LONG x = CW_USEDEFAULT, LONG y = CW_USEDEFAULT);
	void Destroy();
	virtual bool OnResize(int nWidth, int nHeight);
	virtual bool OnUpdateDisplay();

	HWND GetHWnd();
	CVstEffect* GetEffect();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	static HINSTANCE sm_hInstance;						/*!< The instance */
	static std::map<HWND, CVstEditWnd*> sm_wndMap;	/*!< Wnd map */
	HWND m_hWnd;										/*!< The handle of window */
	CVstEffect* m_pEffect;								/*!< Effect */

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

/**
 * �E�B���h�E �n���h���𓾂�
 * @return �E�B���h�E �n���h��
 */
inline HWND CVstEditWnd::GetHWnd()
{
	return m_hWnd;
}

/**
 * �G�t�F�N�g�𓾂�
 * @return �G�t�F�N�g
 */
inline CVstEffect* CVstEditWnd::GetEffect()
{
	return m_pEffect;
}
