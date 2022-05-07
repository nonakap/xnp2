/**
 *	@file	np2arg.cpp
 *	@brief	引数情報クラスの動作の定義を行います
 */

#include "compiler.h"
#include "np2arg.h"
#include "dosio.h"

#define	MAXARG		32				//!< 最大引数エントリ数
#define	ARG_BASE	1				//!< win32 の lpszCmdLine の場合の開始エントリ

//! 唯一のインスタンスです
Np2Arg Np2Arg::sm_instance;

/**
 * コンストラクタ
 */
Np2Arg::Np2Arg()
{
	ZeroMemory(this, sizeof(*this));
}

/**
 * デストラクタ
 */
Np2Arg::~Np2Arg()
{
	free(m_lpArg);
}

/**
 * パース
 */
void Np2Arg::Parse()
{
	// 引数読み出し
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
 * ディスク情報をクリア
 */
void Np2Arg::ClearDisk()
{
	ZeroMemory(m_lpDisk, sizeof(m_lpDisk));
}
