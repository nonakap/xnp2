/**
 * @file	viewitem.cpp
 * @brief	DebugUty �p�r���[���N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "resource.h"
#include "np2.h"
#include "viewitem.h"
#include "viewer.h"
#include "viewmem.h"
#include "viewreg.h"
#include "viewseg.h"
#include "view1mb.h"
#include "viewasm.h"
#include "viewsnd.h"

/**
 * �C���X�^���X�쐬
 * @param[in] nID �r���[ ID
 * @param[in] lpView �r���[�� �C���X�^���X
 * @param[in] lpItem ��ƂȂ�A�C�e��
 * @return �C���X�^���X
 */
CDebugUtyItem* CDebugUtyItem::New(UINT nID, CDebugUtyView* lpView, const CDebugUtyItem* lpItem)
{
	CDebugUtyItem* lpNewItem = NULL;
	switch (nID)
	{
		case IDM_VIEWMODEREG:
			lpNewItem = new CDebugUtyReg(lpView);
			break;

		case IDM_VIEWMODESEG:
			lpNewItem = new CDebugUtySeg(lpView);
			break;

		case IDM_VIEWMODE1MB:
			lpNewItem = new CDebugUty1MB(lpView);
			break;

		case IDM_VIEWMODEASM:
			lpNewItem = new CDebugUtyAsm(lpView);
			break;

		case IDM_VIEWMODESND:
			lpNewItem = new CDebugUtySnd(lpView);
			break;
	}
	if (lpNewItem == NULL)
	{
		lpNewItem = new CDebugUtyItem(lpView, nID);
	}
	lpNewItem->Initialize(lpItem);
	return lpNewItem;
}

/**
 * �R���X�g���N�^
 * @param[in] lpView �r���[�� �C���X�^���X
 * @param[in] nID �r���[ ID
 */
CDebugUtyItem::CDebugUtyItem(CDebugUtyView* lpView, UINT nID)
	: m_lpView(lpView)
	, m_nID(nID)
{
}

/**
 * �f�X�g���N�^
 */
CDebugUtyItem::~CDebugUtyItem()
{
}

/**
 * ������
 * @param[in] lpItem ���t�@�����X
 */
void CDebugUtyItem::Initialize(const CDebugUtyItem* lpItem)
{
	m_lpView->SetVScroll(0, 0);
}

/**
 * �X�V
 * @retval true �X�V����
 * @retval false �X�V�Ȃ�
 */
bool CDebugUtyItem::Update()
{
	return false;
}

/**
 * ���b�N
 * @retval true ����
 * @retval false ���s
 */
bool CDebugUtyItem::Lock()
{
	return false;
}

/**
 * �A�����b�N
 */
void CDebugUtyItem::Unlock()
{
}

/**
 * ���b�N��?
 * @retval true ���b�N���ł���
 * @retval false ���b�N���łȂ�
 */
bool CDebugUtyItem::IsLocked()
{
	return false;
}

/**
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
 * @retval FALSE �A�v���P�[�V���������̃��b�Z�[�W���������Ȃ�����
 */
BOOL CDebugUtyItem::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

/**
 * �`��
 * @param[in] hDC �f�o�C�X �R���e�L�X�g
 * @param[in] rect �̈�
 */
void CDebugUtyItem::OnPaint(HDC hDC, const RECT& rect)
{
}
