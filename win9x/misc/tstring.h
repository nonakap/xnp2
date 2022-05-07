/**
 * @file	tstring.h
 * @brief	������N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <string>

namespace std
{
#ifdef _UNICODE
typedef wstring			tstring;				//!< tchar string �^��`
#else	// _UNICODE
typedef string			tstring;				//!< tchar string�^��`
#endif	// _UNICODE
}

std::tstring LoadTString(UINT uID);
std::tstring LoadTString(LPCTSTR lpString);
