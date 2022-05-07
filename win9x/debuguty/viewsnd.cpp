/**
 * @file	viewsnd.cpp
 * @brief	�T�E���h ���W�X�^�\���N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "strres.h"
#include "resource.h"
#include "np2.h"
#include "viewsnd.h"
#include "viewer.h"
#include "pccore.h"
#include "iocore.h"
#include "sound.h"
#include "fmboard.h"

/**
 * @brief �\���A�C�e��
 */
struct SoundRegisterTable
{
	LPCTSTR lpString;		//!< ������
	UINT16 wAddress;		//!< �A�h���X
	UINT16 wMask;			//!< �\���}�X�N
};

//! �e�[�u��
static const SoundRegisterTable s_table[] =
{
	{TEXT("Sound-Board I"), 0, 0},
	{NULL, 0x0000, 0xffff},
	{NULL, 0x0010, 0x3f07},
	{NULL, 0x0020, 0x07f6},
	{NULL, 0x0030, 0x7777},
	{NULL, 0x0040, 0x7777},
	{NULL, 0x0050, 0x7777},
	{NULL, 0x0060, 0x7777},
	{NULL, 0x0070, 0x7777},
	{NULL, 0x0080, 0x7777},
	{NULL, 0x0090, 0x7777},
	{NULL, 0x00a0, 0x7777},
	{NULL, 0x00b0, 0x0077},
	{str_null, 0, 0},
	{NULL, 0x0100, 0xffff},
	{NULL, 0x0110, 0x0001},
	{NULL, 0x0130, 0x7777},
	{NULL, 0x0140, 0x7777},
	{NULL, 0x0150, 0x7777},
	{NULL, 0x0160, 0x7777},
	{NULL, 0x0170, 0x7777},
	{NULL, 0x0180, 0x7777},
	{NULL, 0x0190, 0x7777},
	{NULL, 0x01a0, 0x7777},
	{NULL, 0x01b0, 0x0077},
#if 0
	{str_null, 0, 0},
	{TEXT("Sound-Board II"), 0, 0},
	{NULL, 0x0200, 0xffff},
	{NULL, 0x0220, 0x07e6},
	{NULL, 0x0230, 0x7777},
	{NULL, 0x0240, 0x7777},
	{NULL, 0x0250, 0x7777},
	{NULL, 0x0260, 0x7777},
	{NULL, 0x0270, 0x7777},
	{NULL, 0x0280, 0x7777},
	{NULL, 0x0290, 0x7777},
	{NULL, 0x02a0, 0x7777},
	{NULL, 0x02b0, 0x0077},
	{str_null, 0, 0},
	{NULL, 0x0230, 0x7777},
	{NULL, 0x0240, 0x7777},
	{NULL, 0x0250, 0x7777},
	{NULL, 0x0260, 0x7777},
	{NULL, 0x0270, 0x7777},
	{NULL, 0x0280, 0x7777},
	{NULL, 0x0290, 0x7777},
	{NULL, 0x02a0, 0x7777},
	{NULL, 0x02b0, 0x0077}
#endif
};

/**
 * �R���X�g���N�^
 * @param[in] lpView �r���[�� �C���X�^���X
 */
CDebugUtySnd::CDebugUtySnd(CDebugUtyView* lpView)
	: CDebugUtyItem(lpView, IDM_VIEWMODESND)
{
}

/**
 * �f�X�g���N�^
 */
CDebugUtySnd::~CDebugUtySnd()
{
}

/**
 * ������
 * @param[in] lpItem ��ƂȂ�A�C�e��
 */
void CDebugUtySnd::Initialize(const CDebugUtyItem* lpItem)
{
	m_lpView->SetVScroll(0, _countof(s_table));
}

/**
 * �X�V
 * @retval true �X�V����
 * @retval false �X�V�Ȃ�
 */
bool CDebugUtySnd::Update()
{
	return m_buffer.empty();
}

/**
 * ���b�N
 * @retval true ����
 * @retval false ���s
 */
bool CDebugUtySnd::Lock()
{
	m_buffer.resize(0x200);
	CopyMemory(&m_buffer.at(0), g_opna[0].s.reg, 0x200);
	return true;
}

/**
 * �A�����b�N
 */
void CDebugUtySnd::Unlock()
{
	m_buffer.clear();
}

/**
 * ���b�N��?
 * @retval true ���b�N���ł���
 * @retval false ���b�N���łȂ�
 */
bool CDebugUtySnd::IsLocked()
{
	return (!m_buffer.empty());
}

/**
 * �`��
 * @param[in] hDC �f�o�C�X �R���e�L�X�g
 * @param[in] rect �̈�
 */
void CDebugUtySnd::OnPaint(HDC hDC, const RECT& rect)
{
	UINT nIndex = m_lpView->GetVScrollPos();
	for (int y = 0; (y < rect.bottom) && (nIndex < _countof(s_table)); y += 16, nIndex++)
	{
		const SoundRegisterTable& item = s_table[nIndex];
		if (item.lpString)
		{
			::TextOut(hDC, 0, y, item.lpString, ::lstrlen(item.lpString));
		}
		else
		{
			TCHAR szTmp[8];
			::wsprintf(szTmp, TEXT("%04x"), item.wAddress);
			TextOut(hDC, 8, y, szTmp, 4);

			const unsigned char* p = NULL;
			if (!m_buffer.empty())
			{
				p = &m_buffer.at(item.wAddress);
			}
			else
			{
				p = g_opna[0].s.reg + item.wAddress;
			}
			for (int x = 0; x < 16; x++)
			{
				if (item.wMask & (1 << x))
				{
					TCHAR szTmp[4];
					::wsprintf(szTmp, TEXT("%02X"), p[x]);
					::TextOut(hDC, ((x * 3) + 6) * 8, y, szTmp, 2);
				}
			}
		}
	}
}
