/**
 * @file	WndBase.h
 * @brief	�E�B���h�E���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief �E�B���h�E���N���X
 */
class CWndBase
{
public:
	HWND m_hWnd;            /*!< must be first data member */

	CWndBase(HWND hWnd = NULL);
	CWndBase& operator=(HWND hWnd);
	void Attach(HWND hWnd);
	HWND Detach();
//	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu);
//	BOOL DestroyWindow();

	// Attributes
	operator HWND() const;
	DWORD GetStyle() const;
	BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);

	// Message Functions
	LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	// Window Text Functions
	BOOL SetWindowText(LPCTSTR lpString);
	int GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const;
	int GetWindowTextLength() const;

	// Font Functions
	void SetFont(HFONT hFont, BOOL bRedraw = TRUE);

	// Menu Functions (non-child windows only)
	HMENU GetMenu() const;
	BOOL DrawMenuBar();
	HMENU GetSystemMenu(BOOL bRevert) const;

	// Window Size and Position Functions
	void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);

	// Window Size and Position Functions
	BOOL GetWindowRect(LPRECT lpRect) const;
	BOOL GetClientRect(LPRECT lpRect) const;

	// Coordinate Mapping Functions
	BOOL ClientToScreen(LPPOINT lpPoint) const;
	int MapWindowPoints(HWND hWndTo, LPPOINT lpPoint, UINT nCount) const;

	// Update and Painting Functions
	HDC BeginPaint(LPPAINTSTRUCT lpPaint);
	void EndPaint(LPPAINTSTRUCT lpPaint);
	BOOL UpdateWindow();
	BOOL Invalidate(BOOL bErase = TRUE);
	BOOL InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);
	BOOL ShowWindow(int nCmdShow);

	// Window State Functions
	BOOL EnableWindow(BOOL bEnable = TRUE);
	HWND SetFocus();

	// Dialog-Box Item Functions
	BOOL CheckDlgButton(int nIDButton, UINT nCheck);
	UINT GetDlgItemInt(int nID, BOOL* lpTrans = NULL, BOOL bSigned = TRUE) const;
	UINT GetDlgItemText(int nID, LPTSTR lpStr, int nMaxCount) const;
	UINT IsDlgButtonChecked(int nIDButton) const;
	LRESULT SendDlgItemMessage(int nID, UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL SetDlgItemInt(int nID, UINT nValue, BOOL bSigned = TRUE);
	BOOL SetDlgItemText(int nID, LPCTSTR lpszString);

	// Window Access Functions
	CWndBase GetParent() const;

	// Window Tree Access
	CWndBase GetDlgItem(int nID) const;

	// Misc. Operations
	int SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE);
	BOOL IsWindow() const;
};

/**
 * �R���X�g���N�^
 * @param[in] hWnd �E�B���h�E �n���h��
 */
inline CWndBase::CWndBase(HWND hWnd)
	: m_hWnd(hWnd)
{
}

/**
 * �I�y���[�^
 * @param[in] hWnd �E�B���h�E �n���h��
 * @return �C���X�^���X
 */
inline CWndBase& CWndBase::operator=(HWND hWnd)
{
	m_hWnd = hWnd;
	return *this;
}

/**
 * �A�^�b�`
 * @param[in] hWnd �E�B���h�E �n���h��
 */
inline void CWndBase::Attach(HWND hWnd)
{
	m_hWnd = hWnd;
}

/**
 * �f�^�b�`
 * @return �ȑO�̃C���X�^���X
 */
inline HWND CWndBase::Detach()
{
	HWND hWnd = m_hWnd;
	m_hWnd = NULL;
	return hWnd;
}

/**
 * HWND �I�y���[�^
 * @return HWND
 */
inline CWndBase::operator HWND() const
{
	return m_hWnd;
}

/**
 * ���݂̃E�B���h�E �X�^�C����Ԃ��܂�
 * @return �E�B���h�E�̃X�^�C��
 */
inline DWORD CWndBase::GetStyle() const
{
	return static_cast<DWORD>(::GetWindowLong(m_hWnd, GWL_STYLE));
}

/**
 * �E�B���h�E �X�^�C����ύX���܂�
 * @param[in] dwRemove �X�^�C���̃��r�W�������ɍ폜����E�B���h�E �X�^�C�����w�肵�܂�
 * @param[in] dwAdd �X�^�C���̃��r�W�������ɒǉ�����E�B���h�E �X�^�C�����w�肵�܂�
 * @param[in] nFlags SetWindowPos�ɓn���t���O
 * @retval TRUE �ύX���ꂽ
 * @retval FALSE �ύX����Ȃ�����
 */
inline BOOL CWndBase::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	const DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	const DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if (dwStyle == dwNewStyle)
	{
		return FALSE;
	}
	::SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);
	if (nFlags != 0)
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}
	return TRUE;
}

/**
 * ���̃E�B���h�E�Ɏw�肳�ꂽ���b�Z�[�W�𑗐M���܂�
 * @param[in] message ���M����郁�b�Z�[�W���w�肵�܂�
 * @param[in] wParam �ǉ��̃��b�Z�[�W�ˑ������w�肵�܂�
 * @param[in] lParam �ǉ��̃��b�Z�[�W�ˑ������w�肵�܂�
 * @return ���b�Z�[�W�̏����̌���
 */
inline LRESULT CWndBase::SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::SendMessage(m_hWnd, message, wParam, lParam);
}

/**
 * ���b�Z�[�W���E�B���h�E�̃��b�Z�[�W �L���[�ɒu���A�Ή�����E�B���h�E�����b�Z�[�W����������̂�҂����ɕԂ���܂�
 * @param[in] message �|�X�g���郁�b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̕t�������w�肵�܂�
 * @param[in] lParam ���b�Z�[�W�̕t�������w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::PostMessage(m_hWnd, message, wParam, lParam);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̃^�C�g���o�[�̃e�L�X�g��ύX���܂�
 * @param[in] lpString �V�����E�B���h�E�^�C�g���܂��̓R���g���[���̃e�L�X�g�Ƃ��Ďg����ANULL �ŏI��镶����ւ̃|�C���^���w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::SetWindowText(LPCTSTR lpString)
{
	return ::SetWindowText(m_hWnd, lpString);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̃^�C�g���o�[�̃e�L�X�g���o�b�t�@�փR�s�[���܂�
 * @param[in] lpszStringBuf �o�b�t�@�ւ̃|�C���^���w�肵�܂��B���̃o�b�t�@�Ƀe�L�X�g���i�[����܂�
 * @param[in] nMaxCount �o�b�t�@�ɃR�s�[���镶���̍ő吔���w�肵�܂��B�e�L�X�g�̂��̃T�C�Y�𒴂��镔���́A�؂�̂Ă��܂��BNULL ���������Ɋ܂߂��܂�
 * @return �R�s�[���ꂽ������̕��������Ԃ�܂� (�I�[�� NULL �����͊܂߂��܂���)
 */
inline int CWndBase::GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
{
	return ::GetWindowText(m_hWnd, lpszStringBuf, nMaxCount);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̃^�C�g���o�[�e�L�X�g�̕�������Ԃ��܂�
 * @return �֐�����������ƁA�e�L�X�g�̕��������Ԃ�܂�
 */
inline int CWndBase::GetWindowTextLength() const
{
	return ::GetWindowTextLength(m_hWnd);
}

/**
 * �w�肵���t�H���g���g�p���܂�
 * @param[in] hFont �t�H���g �n���h��
 * @param[in] bRedraw ���b�Z�[�W��������������ɃE�B���h�E���ĕ`�悷��ꍇ�� TRUE
 */
inline void CWndBase::SetFont(HFONT hFont, BOOL bRedraw)
{
	::SendMessage(m_hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), bRedraw);
}

/**
 * �w�肳�ꂽ�E�B���h�E�Ɋ��蓖�Ă��Ă��郁�j���[�̃n���h�����擾���܂�
 * @return ���j���[�̃n���h�����Ԃ�܂�
 */
inline HMENU CWndBase::GetMenu() const
{
	return ::GetMenu(m_hWnd);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̃��j���[�o�[���ĕ`�悵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::DrawMenuBar()
{
	return ::DrawMenuBar(m_hWnd);
}

/**
 * �w�肳�ꂽ�E�B���h�E�Ɋ��蓖�Ă��Ă���V�X�e�� ���j���[�̃n���h�����擾���܂�
 * @param[in] bRevert ���s�����A�N�V�������w�肵�܂�
 * @return ���j���[�̃n���h�����Ԃ�܂�
 */
inline HMENU CWndBase::GetSystemMenu(BOOL bRevert) const
{
	return ::GetSystemMenu(m_hWnd, bRevert);
}

/**
 * �ʒu�ƃT�C�Y��ύX���܂�
 * @param[in] x �����̐V�����ʒu���w�肵�܂�
 * @param[in] y �㑤�̐V�����ʒu���w�肵�܂�
 * @param[in] nWidth �V���������w�肵�܂�
 * @param[in] nHeight �V�����������w�肵�܂�
 * @param[in] bRepaint �ĕ`�悷��K�v�����邩�ǂ������w�肵�܂�
 * @
 */
inline void CWndBase::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint)
{
	::MoveWindow(m_hWnd, x, y, nWidth, nHeight, bRepaint);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̍���[�ƉE���[�̍��W���X�N���[�����W�Ŏ擾���܂�
 * @param[out] lpRect �\���̂ւ̃|�C���^���w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::GetWindowRect(LPRECT lpRect) const
{
	return ::GetWindowRect(m_hWnd, lpRect);
}

/**
 * lpRect���w���\���� CWnd �̃N���C�A���g�̈�̃N���C�A���g���W���R�s�[���܂�
 * @param[out] lpRect �\���̂ւ̃|�C���^���w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::GetClientRect(LPRECT lpRect) const
{
	return ::GetClientRect(m_hWnd, lpRect);
}

/**
 * �w�肳�ꂽ�_���A�N���C�A���g���W����X�N���[�����W�֕ϊ����܂�
 * @param[in,out] lpPoint �ϊ��Ώۂ̃N���C�A���g���W��ێ����Ă���A1 �� �\���̂ւ̃|�C���^���w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::ClientToScreen(LPPOINT lpPoint) const
{
	return ::ClientToScreen(m_hWnd, lpPoint);
}

/**
 * �����̓_���A����E�B���h�E����Ƃ�����W��Ԃ���A���̃E�B���h�E����Ƃ�����W��Ԃ֕ϊ��i�}�b�v�j���܂�
 * @param[in] hWndTo �ϊ���̓_��ێ�����i�ϊ���j�E�B���h�E�̃n���h�����w�肵�܂�
 * @param[in,out] lpPoint �ϊ��Ώۂ̓_�̍��W��ێ����Ă��� �\���̂���Ȃ� 1 �̔z��ւ̃|�C���^���w�肵�܂�
 * @param[in] nCount lpPoint �p�����[�^�ŁA������ POINT �\���̂���Ȃ� 1 �̔z��ւ̃|�C���^���w�肵���ꍇ�A�z����� POINT �\���̂̐����w�肵�܂�
 * @return �֐�����������ƁA�e�_�̈ړ����������� 32 �r�b�g�l���Ԃ�܂�
 */
inline int CWndBase::MapWindowPoints(HWND hWndTo, LPPOINT lpPoint, UINT nCount) const
{
	return ::MapWindowPoints(m_hWnd, hWndTo, lpPoint, nCount);
}

/**
 * �`����J�n���܂�
 * @param[out] lpPaint �`����ւ̃|�C���^���w�肵�܂�
 * @return �f�o�C�X �R���e�L�X�g
 */
inline HDC CWndBase::BeginPaint(LPPAINTSTRUCT lpPaint)
{
	return ::BeginPaint(m_hWnd, lpPaint);
}

/**
 * �`��̏I�����܂�
 * @param[in] lpPaint �`����ւ̃|�C���^���w�肵�܂�
 */
inline void CWndBase::EndPaint(LPPAINTSTRUCT lpPaint)
{
	::EndPaint(m_hWnd, lpPaint);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̍X�V���[�W��������ł͂Ȃ��ꍇ�A�E�B���h�E�� ���b�Z�[�W�𑗐M���A���̃E�B���h�E�̃N���C�A���g�̈���X�V���܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::UpdateWindow()
{
	return ::UpdateWindow(m_hWnd);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̂��ׂĂ��X�V���[�W�����ɂ��܂�
 * @param[in] bErase �X�V���[�W��������������Ƃ��ɁA�X�V���[�W�������̔w�i���������邩�ǂ������w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::Invalidate(BOOL bErase)
{
	return ::InvalidateRect(m_hWnd, NULL, bErase);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̍X�V���[�W������1�̒����`��ǉ����܂�
 * @param[in] lpRect �X�V���[�W�����֒ǉ������������`�̃N���C�A���g���W��ێ�����1�̍\���̂ւ̃|�C���^���w�肵�܂�
 * @param[in] bErase �X�V���[�W��������������Ƃ��ɁA�X�V���[�W�������̔w�i���������邩�ǂ������w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::InvalidateRect(LPCRECT lpRect, BOOL bErase)
{
	return ::InvalidateRect(m_hWnd, lpRect, bErase);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̕\����Ԃ�ݒ肵�܂�
 * @param[in] nCmdShow �E�B���h�E�̕\�����@���w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::ShowWindow(int nCmdShow)
{
	return ::ShowWindow(m_hWnd, nCmdShow);
}

/**
 * �w�肳�ꂽ�E�B���h�E�܂��̓R���g���[���ŁA�}�E�X���͂ƃL�[�{�[�h���͂�L���܂��͖����ɂ��܂�
 * @param[in] bEnable �E�B���h�E��L���ɂ��邩�����ɂ��邩���w�肵�܂�
 * @retval TRUE �E�B���h�E�����ɖ����ɂȂ��Ă���
 * @retval FALSE �E�B���h�E�������ɂȂ��Ă��Ȃ�����
 */
inline BOOL CWndBase::EnableWindow(BOOL bEnable)
{
	return ::EnableWindow(m_hWnd, bEnable);
}

/**
 * ���̓t�H�[�J�X��v�����܂�
 * @return ���O�ɓ��̓t�H�[�J�X�������Ă����E�B���h�E �n���h��
 */
inline HWND CWndBase::SetFocus()
{
	return ::SetFocus(m_hWnd);
}

/**
 * �{�^���R���g���[���̃`�F�b�N��Ԃ�ύX���܂�
 * @param[in] nIDButton ��Ԃ�ύX�������{�^���̎��ʎq���w�肵�܂�
 * @param[in] nCheck �{�^���̃`�F�b�N��Ԃ��w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::CheckDlgButton(int nIDButton, UINT nCheck)
{
	return ::CheckDlgButton(m_hWnd, nIDButton, nCheck);
}

/**
 * �_�C�A���O�{�b�N�X���̎w�肳�ꂽ�R���g���[���̃e�L�X�g���A�����l�֕ϊ����܂�
 * @param[in] nID �ϊ��������e�L�X�g�����R���g���[���̎��ʎq���w�肵�܂�
 * @param[in] lpTrans ���������s�̒l���󂯎��ϐ��ւ̃|�C���^���w�肵�܂�
 * @param[in] bSigned �e�L�X�g�𕄍��t���Ƃ��Ĉ����ĕ����t���̒l��Ԃ����ǂ������w�肵�܂�
 * @return �R���g���[���e�L�X�g�ɑ������鐮���l���Ԃ�܂�
 */
inline UINT CWndBase::GetDlgItemInt(int nID, BOOL* lpTrans, BOOL bSigned) const
{
	return ::GetDlgItemInt(m_hWnd, nID, lpTrans, bSigned);
}

/**
 * �_�C�A���O�{�b�N�X���̎w�肳�ꂽ�R���g���[���Ɋ֘A�t�����Ă���^�C�g���܂��̓e�L�X�g���擾���܂�
 * @param[in] nID �擾�������^�C�g���܂��̓e�L�X�g��ێ����Ă���R���g���[���̎��ʎq���w�肵�܂�
 * @param[out] lpStr �^�C�g���܂��̓e�L�X�g���󂯎��o�b�t�@�ւ̃|�C���^���w�肵�܂�
 * @param[in] nMaxCount lpStr �p�����[�^���w���o�b�t�@�փR�s�[����镶����̍ő�̒����� TCHAR �P�ʂŎw�肵�܂�
 * @return �o�b�t�@�փR�s�[���ꂽ������̒����i �I�[�� NULL ���܂܂Ȃ��j�� TCHAR �P�ʂŕԂ�܂�
 */
inline UINT CWndBase::GetDlgItemText(int nID, LPTSTR lpStr, int nMaxCount) const
{
	return ::GetDlgItemText(m_hWnd, nID, lpStr, nMaxCount);
}

/**
 * �{�^���R���g���[���̃`�F�b�N��Ԃ��擾���܂�
 * @param[in] nIDButton �{�^���R���g���[���̎��ʎq���w�肵�܂�
 * @return �`�F�b�N���
 */
inline UINT CWndBase::IsDlgButtonChecked(int nIDButton) const
{
	return ::IsDlgButtonChecked(m_hWnd, nIDButton);
}

/**
 * �_�C�A���O�{�b�N�X���̎w�肳�ꂽ�R���g���[���փ��b�Z�[�W�𑗐M���܂��B
 * @param[in] nID ���b�Z�[�W���󂯎��R���g���[���̎��ʎq���w�肵�܂�
 * @param[in] message ���M���������b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̒ǉ������w�肵�܂�
 * @param[in] lParam ���b�Z�[�W�̒ǉ������w�肵�܂�
 * @return ���b�Z�[�W�����̌��ʂ��Ԃ�܂�
 */
inline LRESULT CWndBase::SendDlgItemMessage(int nID, UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::SendDlgItemMessage(m_hWnd, nID, message, wParam, lParam);
}

/**
 * �w�肳�ꂽ�����l�𕶎���֕ϊ����A�_�C�A���O�{�b�N�X���̃R���g���[���Ƀe�L�X�g�Ƃ��Đݒ肵�܂�
 * @param[in] nID �ύX�����������R���g���[���̎��ʎq���w�肵�܂�
 * @param[in] nValue �����l���w�肵�܂�
 * @param[in] bSigned nValue �p�����[�^�̒l�������t�����ǂ������w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::SetDlgItemInt(int nID, UINT nValue, BOOL bSigned)
{
	return ::SetDlgItemInt(m_hWnd, nID, nValue, bSigned);
}

/**
 * �_�C�A���O�{�b�N�X���̃R���g���[���̃^�C�g���܂��̓e�L�X�g��ݒ肵�܂�
 * @param[in] nID �e�L�X�g��ݒ肵�����R���g���[���̎��ʎq���w�肵�܂�
 * @param[in] lpszString �R���g���[���փR�s�[�������e�L�X�g��ێ�����ANULL �ŏI��镶����ւ̃|�C���^���w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::SetDlgItemText(int nID, LPCTSTR lpszString)
{
	return ::SetDlgItemText(m_hWnd, nID, lpszString);
}

/**
 * �w�肳�ꂽ�q�E�B���h�E�̐e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h����Ԃ��܂�
 * @return �e�E�B���h�E�̃n���h��
 */
inline CWndBase CWndBase::GetParent() const
{
	return CWndBase(::GetParent(m_hWnd));
}

/**
 * �w�肳�ꂽ�_�C�A���O�{�b�N�X���̃R���g���[���̃n���h�����擾���܂�
 * @param[in] nID �n���h�����擾�������R���g���[���̎��ʎq���w�肵�܂�
 * @return �E�B���h�E
 */
inline CWndBase CWndBase::GetDlgItem(int nID) const
{
	return CWndBase(::GetDlgItem(m_hWnd, nID));
}

/**
 * �X�N���[���o�[�̂��܂��܂ȃp�����[�^��ݒ肵�܂�
 * @param[in] nBar �p�����[�^��ݒ肷��ׂ��X�N���[���o�[�̃^�C�v���w�肵�܂�
 * @param[in] lpScrollInfo �ݒ肷��ׂ�����ێ����Ă���A1�̍\���̂ւ̃|�C���^���w�肵�܂�
 * @param[in] bRedraw �X�N���[���o�[���ĕ`�悷�邩�ǂ������w�肵�܂�
 * @return �X�N���[���o�[�̌��݂̃X�N���[���ʒu���Ԃ�܂�
 */
inline int CWndBase::SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw)
{
	return ::SetScrollInfo(m_hWnd, nBar, lpScrollInfo, bRedraw);
}

/**
 * �E�B���h�E�����݂��Ă��邩�ǂ����𒲂ׂ܂�
 * @return �w�肵���E�B���h�E�n���h�������E�B���h�E�����݂��Ă���ꍇ�́A0 �ȊO�̒l���Ԃ�܂�
 */
inline BOOL CWndBase::IsWindow() const
{
	return ::IsWindow(m_hWnd);
}
