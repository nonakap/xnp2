/**
 * @file	mdbgwnd.h
 * @brief	������ �f�o�K �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)

#include "dd2.h"
#include "subwnd.h"

/**
 * @brief ������ �f�o�K
 */
class CMemDebugWnd : public CSubWndBase
{
public:
	static CMemDebugWnd* GetInstance();
	static void Initialize();
	static void Deinitialize();
	CMemDebugWnd();
	virtual ~CMemDebugWnd();
	void Create();
	void OnIdle();

protected:
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	void OnDestroy();
	void OnPaint();

private:
	static CMemDebugWnd sm_instance;		//!< �C���X�^���X
	DD2Surface m_dd2;						//!< DirectDraw2 �C���X�^���X
	int m_nWidth;							//!< ��
	int m_nHeight;							//!< ����
	void OnDraw(BOOL redraw);
	static void mdpalcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp);
};

/**
 * �C���X�^���X��Ԃ�
 * @return �C���X�^���X
 */
inline CMemDebugWnd* CMemDebugWnd::GetInstance()
{
	return &sm_instance;
}

#define mdbgwin_initialize		CMemDebugWnd::Initialize
#define mdbgwin_create			CMemDebugWnd::GetInstance()->Create
#define mdbgwin_destroy			CMemDebugWnd::GetInstance()->DestroyWindow
#define mdbgwin_process			CMemDebugWnd::GetInstance()->OnIdle
#define mdbgwin_gethwnd			CMemDebugWnd::GetInstance()->GetSafeHwnd
void mdbgwin_readini();
void mdbgwin_writeini();

#else

#define mdbgwin_initialize()
#define mdbgwin_create(i)
#define mdbgwin_destroy()
#define mdbgwin_process()
#define mdbgwin_gethwnd()		(NULL)
#define mdbgwin_readini()
#define mdbgwin_writeini()

#endif
