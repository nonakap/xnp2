/**
 *	@file	np2arg.cpp
 *	@brief	�������N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "np2arg.h"
#include "dosio.h"

#define	MAXARG		32				//!< �ő�����G���g����
#define	ARG_BASE	1				//!< win32 �� lpszCmdLine �̏ꍇ�̊J�n�G���g��

//! �B��̃C���X�^���X�ł�
Np2Arg Np2Arg::sm_instance;

/**
 * �R���X�g���N�^
 */
Np2Arg::Np2Arg()
{
	ZeroMemory(this, sizeof(*this));
}

/**
 * �f�X�g���N�^
 */
Np2Arg::~Np2Arg()
{
	free(m_lpArg);
}

/**
 * �p�[�X
 */
void Np2Arg::Parse()
{
	// �����ǂݏo��
	free(m_lpArg);
	m_lpArg = _tcsdup(::GetCommandLine());

	LPTSTR argv[MAXARG];
	const int argc = ::milstr_getarg(m_lpArg, argv, _countof(argv));

	int nDrive = 0;

	for (int i = ARG_BASE; i < argc; i++)
	{
		LPCTSTR lpArg = argv[i];
		if ((lpArg[0] == TEXT('/')) || (lpArg[0] == TEXT('-')))
		{
			switch (_totlower(lpArg[1]))
			{
				case 'f':
					m_fFullscreen = true;
					break;

				case 'i':
					m_lpIniFile = &lpArg[2];
					break;
			}
		}
		else
		{
			LPCTSTR lpExt = ::file_getext(lpArg);
			if (::file_cmpname(lpExt, TEXT("ini")) == 0)
			{
				m_lpIniFile = lpArg;
			}
			else if (nDrive < _countof(m_lpDisk))
			{
				m_lpDisk[nDrive++] = lpArg;
			}
		}
	}
}

/**
 * �f�B�X�N�����N���A
 */
void Np2Arg::ClearDisk()
{
	ZeroMemory(m_lpDisk, sizeof(m_lpDisk));
}
