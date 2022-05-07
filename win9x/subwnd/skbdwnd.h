/**
 * @file	skbdwnd.h
 * @brief	�\�t�g�E�F�A �L�[�{�[�h �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#if defined(SUPPORT_SOFTKBD)

#include "dd2.h"
#include "subwnd.h"

/**
 * @brief �\�t�g�E�F�A �L�[�{�[�h
 */
class CSoftKeyboardWnd : public CSubWndBase
{
public:
	static CSoftKeyboardWnd* GetInstance();
	static void Initialize();
	static void Deinitialize();
	CSoftKeyboardWnd();
	virtual ~CSoftKeyboardWnd();
	void Create();
	void OnIdle();

protected:
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	void OnDestroy();
	void OnPaint();

private:
	static CSoftKeyboardWnd sm_instance;	//!< �C���X�^���X
	DD2Surface m_dd2;						//!< DirectDraw2 �C���X�^���X
	int m_nWidth;							//!< ��
	int m_nHeight;							//!< ����
	void OnDraw(BOOL redraw);
	static void skpalcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp);
};

/**
 * �C���X�^���X��Ԃ�
 * @return �C���X�^���X
 */
inline CSoftKeyboardWnd* CSoftKeyboardWnd::GetInstance()
{
	return &sm_instance;
}

#define skbdwin_initialize		CSoftKeyboardWnd::Initialize
#define skbdwin_deinitialize	CSoftKeyboardWnd::Deinitialize
#define skbdwin_create			CSoftKeyboardWnd::GetInstance()->Create
#define skbdwin_destroy			CSoftKeyboardWnd::GetInstance()->DestroyWindow
#define skbdwin_gethwnd			CSoftKeyboardWnd::GetInstance()->GetSafeHwnd
#define skbdwin_process			CSoftKeyboardWnd::GetInstance()->OnIdle
void skbdwin_readini();
void skbdwin_writeini();

#else

#define skbdwin_initialize()
#define skbdwin_deinitialize()
#define skbdwin_create()
#define skbdwin_destroy()
#define skbdwin_gethwnd()		(NULL)
#define skbdwin_process()
#define skbdwin_readini()
#define skbdwin_writeini()

#endif
