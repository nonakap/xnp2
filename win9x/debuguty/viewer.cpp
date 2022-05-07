/**
 * @file	viewer.cpp
 * @brief	DebugUty �p�r���[�� �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "resource.h"
#include "np2.h"
#include "viewer.h"
#include "viewitem.h"
#include "cpucore.h"

//! �C���X�^���X
static CDebugUtyView* g_np2view[NP2VIEW_MAX];

//! �r���[ �N���X��
static const TCHAR s_szViewClass[] = TEXT("NP2-ViewWindow");

//! �t�H���g
static const TCHAR s_szViewFont[] = _T("�l�r �S�V�b�N");

//! �Ō��Tick
DWORD CDebugUtyView::sm_dwLastTick;

//! �`�F�b�N �}�N��
#define MFCHECK(bChecked) ((bChecked) ? MF_CHECKED : MF_UNCHECKED)

/**
 * ������
 * @param[in] hInstance �C���X�^���X
 */
void CDebugUtyView::Initialize(HINSTANCE hInstance)
{
	sm_dwLastTick = ::GetTickCount();

	ZeroMemory(g_np2view, sizeof(g_np2view));

	WNDCLASS np2vc;
	np2vc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	np2vc.lpfnWndProc = ::DefWindowProc;
	np2vc.cbClsExtra = 0;
	np2vc.cbWndExtra = 0;
	np2vc.hInstance = hInstance;
	np2vc.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	np2vc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	np2vc.hbrBackground = static_cast<HBRUSH>(NULL);
	np2vc.lpszMenuName = MAKEINTRESOURCE(IDR_VIEW);
	np2vc.lpszClassName = s_szViewClass;
	::RegisterClass(&np2vc);
}

/**
 * �V�����E�B���h�E���쐬����
 */
void CDebugUtyView::New()
{
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		CDebugUtyView* lpView = g_np2view[i];
		if (lpView != NULL)
		{
			continue;
		}

		CDebugUtyView* view = new CDebugUtyView;
		g_np2view[i] = view;

		if (view->CreateEx(0, s_szViewClass, NULL, WS_OVERLAPPEDWINDOW | WS_VSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0))
		{
			view->ShowWindow(SW_SHOWNORMAL);
			view->UpdateWindow();
		}
		break;
	}
}

/**
 * ���ׂĕ���
 */
void CDebugUtyView::AllClose()
{
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		CDebugUtyView* lpView = g_np2view[i];
		if (lpView != NULL)
		{
			lpView->DestroyWindow();
		}
	}
}

/**
 * ���ׂčX�V
 * @param[in] bForce �����I�ɍX�V����
 */
void CDebugUtyView::AllUpdate(bool bForce)
{
	const DWORD dwNow  = ::GetTickCount();
	if ((!bForce) || ((dwNow - sm_dwLastTick) >= 200))
	{
		sm_dwLastTick = dwNow;

		for (size_t i = 0; i < _countof(g_np2view); i++)
		{
			CDebugUtyView* lpView = g_np2view[i];
			if (lpView != NULL)
			{
				lpView->UpdateView();
			}
		}
	}
}

/**
 * �R���X�g���N�^
 */
CDebugUtyView::CDebugUtyView()
	: m_bActive(false)
	, m_nVPos(0)
	, m_nVLines(0)
	, m_nVPage(0)
	, m_nVMultiple(1)
	, m_lpItem(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CDebugUtyView::~CDebugUtyView()
{
	if (m_lpItem)
	{
		delete m_lpItem;
	}

	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		if (g_np2view[i] == this)
		{
			g_np2view[i] = NULL;
			UpdateActive();
			break;
		}
	}
}

/**
 * �L���v�V�����̍X�V
 */
void CDebugUtyView::UpdateCaption()
{
	int nIndex = -1;
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		if (g_np2view[i] == this)
		{
			nIndex = static_cast<int>(i);
			break;
		}
	}
	LPCTSTR lpMode = (m_lpItem->IsLocked()) ? TEXT("Locked") : TEXT("Realtime");

	TCHAR szTitle[256];
	wsprintf(szTitle, TEXT("%d.%s - NP2 Debug Utility"), nIndex + 1, lpMode);

	SetWindowText(szTitle);
}

/**
 * V �X�N���[���ʒu�̐ݒ�
 * @param[in] nPos �V�����ʒu
 */
void CDebugUtyView::SetVScrollPos(UINT nPos)
{
	if (m_nVPos != nPos)
	{
		m_nVPos = nPos;
		UpdateVScroll();
		Invalidate();
	}
}

/**
 * V �X�N���[���̐ݒ�
 * @param[in] nPos �V�����ʒu
 * @param[in] nLines ���C����
 */
void CDebugUtyView::SetVScroll(UINT nPos, UINT nLines)
{
	if ((m_nVPos != nPos) || (m_nVLines != nLines))
	{
		m_nVPos = nPos;
		m_nVLines = nLines;
		m_nVMultiple = ((nLines - 1) / 0xFFFF) + 1;
		UpdateVScroll();
		Invalidate();
	}
}

/**
 * V �X�N���[���o�[�̍X�V
 */
void CDebugUtyView::UpdateVScroll()
{
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = ((m_nVLines + m_nVMultiple - 1) / m_nVMultiple) - 1;
	si.nPos = m_nVPos / m_nVMultiple;
	si.nPage = m_nVPage / m_nVMultiple;
	SetScrollInfo(SB_VERT, &si, TRUE);
}

/**
 * �E�B���h�E �v���V�[�W��
 * @param[in] message ���b�Z�[�W
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @return ���U���g �R�[�h
 */
LRESULT CDebugUtyView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
			return OnCreate(reinterpret_cast<LPCREATESTRUCT>(lParam));

		case WM_PAINT:
			OnPaint();
			break;

		case WM_SIZE:
			OnSize(static_cast<UINT>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;

		case WM_VSCROLL:
			OnVScroll(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HWND>(lParam));
			break;

		case WM_ENTERMENULOOP:
			OnEnterMenuLoop(static_cast<BOOL>(wParam));
			break;

		case WM_ACTIVATE:
			OnActivate(LOWORD(wParam), reinterpret_cast<HWND>(lParam), HIWORD(wParam));
			break;

		case WM_CLOSE:
			DestroyWindow();
			break;

		default:
			return CWndProc::WindowProc(message, wParam, lParam);
	}
	return 0;
}

/**
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
 */
BOOL CDebugUtyView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT nID = LOWORD(wParam);
	switch (nID)
	{
		case IDM_VIEWWINNEW:
			New();
			break;

		case IDM_VIEWWINCLOSE:
			DestroyWindow();
			break;

		case IDM_VIEWWINALLCLOSE:
			AllClose();
			break;

		case IDM_VIEWMODEREG:
		case IDM_VIEWMODESEG:
		case IDM_VIEWMODE1MB:
		case IDM_VIEWMODEASM:
		case IDM_VIEWMODESND:
			SetMode(nID);
			break;

		case IDM_VIEWMODELOCK:
			if (!m_lpItem->IsLocked())
			{
				m_lpItem->Lock();
			}
			else
			{
				m_lpItem->Unlock();
			}
			UpdateCaption();
			Invalidate();
			break;


		default:
			return m_lpItem->OnCommand(wParam, lParam);
	}
	return TRUE;
}

/**
 * �E�B���h�E�̍쐬���ɁA���̃����o�[�֐����Ăяo���܂�
 * @param[in] lpCreateStruct
 * @retval 0 ����
 */
int CDebugUtyView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_lpItem = CDebugUtyItem::New(IDM_VIEWMODEREG, this);
	UpdateCaption();
	return 0;
}

/**
 * �t���[�����[�N�́A�E�B���h�E�̃T�C�Y���ς������ɂ��̃����o�[�֐����Ăт܂� 
 * @param[in] nType �T�C�Y�ύX�̎��
 * @param[in] cx �N���C�A���g�̈�̐V������
 * @param[in] cy �N���C�A���g�̈�̐V��������
 */
void CDebugUtyView::OnSize(UINT nType, int cx, int cy)
{
	m_nVPage = cy / 16;
	UpdateVScroll();
}

/**
 * �ĕ`��v�����ɁA���̃����o�[�֐����Ăяo���܂�
 */
void CDebugUtyView::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(&ps);

	const RECT& rect = ps.rcPaint;

	HDC hdcMem = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
	hBitmap = static_cast<HBITMAP>(::SelectObject(hdcMem, hBitmap));


	HBRUSH hBrush = ::CreateSolidBrush(RGB(0, 0, 64));
	::FillRect(hdcMem, &rect, hBrush);
	::DeleteObject(hBrush);

	HFONT hFont = ::CreateFont(16, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, s_szViewFont);
	::SetTextColor(hdcMem, 0xffffff);
	::SetBkColor(hdcMem, 0x400000);
	hFont = static_cast<HFONT>(::SelectObject(hdcMem, hFont));

		m_lpItem->OnPaint(hdcMem, rect);

	::DeleteObject(SelectObject(hdcMem, hFont));

	::BitBlt(hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hdcMem, rect.left, rect.top, SRCCOPY);
	::DeleteObject(::SelectObject(hdcMem, hBitmap));
	::DeleteDC(hdcMem);

	EndPaint(&ps);
}

/**
 * �t���[�����[�N�́A���[�U�[���E�B���h�E�ɐ����X�N���[�� �o�[���N���b�N����ƁA���̃����o�[�֐����Ăяo���܂�
 * @param[in] nSBCode ���[�U�[�̗v���������X�N���[�� �o�[ �R�[�h���w�肵�܂�
 * @param[in] nPos ���݂̃X�N���[�� �{�b�N�X�̈ʒu
 * @param[in] hwndScrollBar �X�N���[�� �o�[ �R���g���[��
 */
void CDebugUtyView::OnVScroll(UINT nSBCode, UINT nPos, HWND hwndScrollBar)
{
	UINT32 nNewPos = m_nVPos;
	switch (nSBCode)
	{
		case SB_LINEUP:
			if (nNewPos)
			{
				nNewPos--;
			}
			break;

		case SB_LINEDOWN:
			if ((nNewPos + m_nVPage) < m_nVLines)
			{
				nNewPos++;
			}
			break;

		case SB_PAGEUP:
			if (nNewPos > m_nVPage)
			{
				nNewPos -= m_nVPage;
			}
			else
			{
				nNewPos = 0;
			}
			break;

		case SB_PAGEDOWN:
			nNewPos += m_nVPage;
			if (nNewPos > (m_nVLines - m_nVPage))
			{
				nNewPos = m_nVLines - m_nVPage;
			}
			break;

		case SB_THUMBTRACK:
			nNewPos = nPos * m_nVMultiple;
			break;
	}
	SetVScrollPos(nNewPos);
}

/**
 * The framework calls this member function when a menu modal loop has been entered
 * @param[in] bIsTrackPopupMenu Specifies whether the menu involved is a popup menu
 */
void CDebugUtyView::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
	HMENU hMenu = GetMenu();
	if (hMenu == NULL)
	{
		return;
	}
	::CheckMenuItem(hMenu, IDM_VIEWMODELOCK, MF_BYCOMMAND | MFCHECK(m_lpItem->IsLocked()));

	const UINT nID = m_lpItem->GetID();
#if 1
	::CheckMenuRadioItem(hMenu, IDM_VIEWMODEREG, IDM_VIEWMODESND, nID, MF_BYCOMMAND);
#else
	::CheckMenuItem(hMenu, IDM_VIEWMODEREG, MF_BYCOMMAND | MFCHECK(nID == IDM_VIEWMODEREG));
	::CheckMenuItem(hMenu, IDM_VIEWMODESEG, MF_BYCOMMAND | MFCHECK(nID == IDM_VIEWMODESEG));
	::CheckMenuItem(hMenu, IDM_VIEWMODE1MB, MF_BYCOMMAND | MFCHECK(nID == IDM_VIEWMODE1MB));
	::CheckMenuItem(hMenu, IDM_VIEWMODEASM, MF_BYCOMMAND | MFCHECK(nID == IDM_VIEWMODEASM));
	::CheckMenuItem(hMenu, IDM_VIEWMODESND, MF_BYCOMMAND | MFCHECK(nID == IDM_VIEWMODESND));
#endif

	HMENU hSubMenu = ::GetSubMenu(hMenu, 2);
	if (hSubMenu)
	{
		SetSegmentItem(hSubMenu, IDM_SEGCS, TEXT("CS"), CPU_CS);
		SetSegmentItem(hSubMenu, IDM_SEGDS, TEXT("DS"), CPU_DS);
		SetSegmentItem(hSubMenu, IDM_SEGES, TEXT("ES"), CPU_ES);
		SetSegmentItem(hSubMenu, IDM_SEGSS, TEXT("SS"), CPU_SS);
		DrawMenuBar();
	}
}

/**
 * �t���[�����[�N�� CWnd �̃I�u�W�F�N�g���A�N�e�B�u���܂��͔�A�N�e�B�u�����ꂽ�Ƃ��ɂ��̃����o�[�֐����Ăяo����܂�
 * @param[in] nState �A�N�e�B�u�ɂȂ��Ă��邩��A�N�e�B�u�ɂȂ��Ă��邩���w�肵�܂�
 * @param[in] hwndOther �A�N�e�B�u�܂��͔�A�N�e�B�u�ɂȂ�E�B���h�E �n���h��
 * @param[in] bMinimized �A�N�e�B�u�܂��͔�A�N�e�B�u�ɂȂ�E�B���h�E�̍ŏ����̏�Ԃ��w�肵�܂�
 */
void CDebugUtyView::OnActivate(UINT nState, HWND hwndOther, BOOL bMinimized)
{
	m_bActive = (nState != WA_INACTIVE);
	UpdateActive();
}

/**
 * Called by the default OnNcDestroy member function after the window has been destroyed.
 */
void CDebugUtyView::PostNcDestroy()
{
	delete this;
}

/**
 * ���[�h�ύX
 * @param[in] nID ���[�h
 */
void CDebugUtyView::SetMode(UINT nID)
{
	if (m_lpItem->GetID() != nID)
	{
		CDebugUtyItem* lpItem = m_lpItem;
		m_lpItem = CDebugUtyItem::New(nID, this, lpItem);
		delete lpItem;
		Invalidate();
	}
}

/**
 * �r���[�X�V
 */
void CDebugUtyView::UpdateView()
{
	if (m_lpItem->Update())
	{
		Invalidate();
	}
}

/**
 * ���j���[ �A�C�e�����X�V
 * @param[in] hMenu ���j���[ �n���h��
 * @param[in] nId ���j���[ ID
 * @param[in] lpSegment �Z�O�����g��
 * @param[in] nSegment �Z�O�����g�l
 */
void CDebugUtyView::SetSegmentItem(HMENU hMenu, int nId, LPCTSTR lpSegment, UINT nSegment)
{
	TCHAR szString[32];
	wsprintf(szString, TEXT("Seg = &%s [%04x]"), lpSegment, nSegment);
	::ModifyMenu(hMenu, nId, MF_BYCOMMAND | MF_STRING, nId, szString);
}

/**
 * �A�N�e�B�u �t���O���X�V
 */
void CDebugUtyView::UpdateActive()
{
	bool bActive = false;
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		const CDebugUtyView* lpView = g_np2view[i];
		if ((lpView != NULL) && (lpView->m_bActive))
		{
			bActive = true;
		}
	}

	if (bActive)
	{
		np2break |= NP2BREAK_DEBUG;
	}
	else
	{
		np2break &= ~NP2BREAK_DEBUG;
	}
	np2active_renewal();
}
