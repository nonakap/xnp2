/**
 * @file	kdispwnd.h
 * @brief	�L�[�{�[�h �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#if defined(SUPPORT_KEYDISP)

#include "dd2.h"
#include "subwnd.h"

/**
 * @brief �L�[�{�[�h
 */
class CKeyDisplayWnd : public CSubWndBase
{
public:
	static CKeyDisplayWnd* GetInstance();
	static void Initialize();
	static void Deinitialize();
	CKeyDisplayWnd();
	virtual ~CKeyDisplayWnd();
	void Create();
	void Draw(UINT8 cnt);

protected:
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	void OnDestroy();
	void OnRButtonDown(UINT nFlags, POINT point);
	void OnPaint();

private:
	static CKeyDisplayWnd sm_instance;		//!< �C���X�^���X
	DD2Surface m_dd2;						//!< DirectDraw2 �C���X�^���X
	void OnDraw(BOOL redraw);
	void OnResize();
	void SetDispMode(UINT8 mode);
};

/**
 * �C���X�^���X��Ԃ�
 * @return �C���X�^���X
 */
inline CKeyDisplayWnd* CKeyDisplayWnd::GetInstance()
{
	return &sm_instance;
}

#define kdispwin_initialize		CKeyDisplayWnd::Initialize
#define kdispwin_create			CKeyDisplayWnd::GetInstance()->Create
#define kdispwin_destroy		CKeyDisplayWnd::GetInstance()->DestroyWindow
#define kdispwin_gethwnd		CKeyDisplayWnd::GetInstance()->GetSafeHwnd
#define kdispwin_draw			CKeyDisplayWnd::GetInstance()->Draw
void kdispwin_readini();
void kdispwin_writeini();

#else

#define kdispwin_initialize()
#define kdispwin_create()
#define kdispwin_destroy()
#define kdispwin_gethwnd()		(NULL)
#define kdispwin_draw(c)
#define kdispwin_readini()
#define kdispwin_writeini()

#endif
