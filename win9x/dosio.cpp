/**
 *	@file	dosio.cpp
 *	@brief	�t�@�C�� �A�N�Z�X�֐��Q�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "dosio.h"

//! �J�����g �p�X �o�b�t�@
static OEMCHAR curpath[MAX_PATH];

//! �t�@�C�����|�C���^
static OEMCHAR *curfilep = curpath;

/**
 * ������
 */
void dosio_init(void)
{
}

/**
 * ���
 */
void dosio_term(void)
{
}

/**
 * �t�@�C�����J���܂�
 * @param[in] lpPathName �t�@�C����
 * @return �t�@�C�� �n���h��
 */
FILEH DOSIOCALL file_open(const OEMCHAR* lpPathName)
{
	FILEH hFile = ::CreateFile(lpPathName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = ::CreateFile(lpPathName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	return hFile;
}

/**
 * ���[�h �I�����[�Ńt�@�C�����J���܂�
 * @param[in] lpPathName �t�@�C����
 * @return �t�@�C�� �n���h��
 */
FILEH DOSIOCALL file_open_rb(const OEMCHAR* lpPathName)
{
	return ::CreateFile(lpPathName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

/**
 * �t�@�C�����쐬���܂�
 * @param[in] lpPathName �t�@�C����
 * @return �t�@�C�� �n���h��
 */
FILEH DOSIOCALL file_create(const OEMCHAR* lpPathName)
{
	return ::CreateFile(lpPathName, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

/**
 * �t�@�C���̃V�[�N
 * @param[in] hFile �t�@�C�� �n���h��
 * @param[in] pointer �ړ����ׂ��o�C�g��
 * @param[in] method �J�n�_
 * @return �t�@�C���̈ʒu
 */
long DOSIOCALL file_seek(FILEH hFile, long pointer, int method)
{
	return static_cast<long>(::SetFilePointer(hFile, pointer, 0, method));
}

/**
 * �t�@�C���ǂݍ���
 * @param[in] hFile �t�@�C�� �n���h��
 * @param[out] lpBuffer �o�b�t�@
 * @param[in] cbBuffer �o�b�t�@ �T�C�Y
 * @return �ǂݍ��݃T�C�Y
 */
UINT DOSIOCALL file_read(FILEH hFile, void* lpBuffer, UINT cbBuffer)
{
	DWORD dwReadSize;
	if (::ReadFile(hFile, lpBuffer, cbBuffer, &dwReadSize, NULL))
	{
		return dwReadSize;
	}
	return 0;
}

/**
 * �t�@�C����������
 * @param[in] hFile �t�@�C�� �n���h��
 * @param[in] lpBuffer �o�b�t�@
 * @param[in] cbBuffer �o�b�t�@ �T�C�Y
 * @return �������݃T�C�Y
 */
UINT DOSIOCALL file_write(FILEH hFile, const void* lpBuffer, UINT cbBuffer)
{
	if (cbBuffer != 0)
	{
		DWORD dwWrittenSize;
		if (::WriteFile(hFile, lpBuffer, cbBuffer, &dwWrittenSize, NULL))
		{
			return dwWrittenSize;
		}
	}
	else
	{
		::SetEndOfFile(hFile);
	}
	return 0;
}

/**
 * �t�@�C�� �n���h�������
 * @param[in] hFile �t�@�C�� �n���h��
 * @retval 0 ����
 */
short DOSIOCALL file_close(FILEH hFile)
{
	::CloseHandle(hFile);
	return 0;
}

/**
 * �t�@�C�� �T�C�Y�𓾂�
 * @param[in] hFile �t�@�C�� �n���h��
 * @return �t�@�C�� �T�C�Y
 */
UINT DOSIOCALL file_getsize(FILEH hFile)
{
	return ::GetFileSize(hFile, NULL);
}

/**
 * FILETIME �� DOSDATE/DOSTIME �ɕϊ�
 * @param[in] ft �t�@�C�� �^�C��
 * @param[out] dosdate DOSDATE
 * @param[out] dostime DOSTIME
 * @retval true ����
 * @retval false ���s
 */
static bool convertDateTime(const FILETIME& ft, DOSDATE* dosdate, DOSTIME* dostime)
{
	FILETIME ftLocalTime;
	if (!::FileTimeToLocalFileTime(&ft, &ftLocalTime))
	{
		return false;
	}

	SYSTEMTIME st;
	if (!::FileTimeToSystemTime(&ftLocalTime, &st))
	{
		return false;
	}

	if (dosdate)
	{
		dosdate->year = st.wYear;
		dosdate->month = static_cast<UINT8>(st.wMonth);
		dosdate->day = static_cast<UINT8>(st.wDay);
	}
	if (dostime)
	{
		dostime->hour = static_cast<UINT8>(st.wHour);
		dostime->minute = static_cast<UINT8>(st.wMinute);
		dostime->second = static_cast<UINT8>(st.wSecond);
	}
	return true;
}

/**
 * �t�@�C���̃^�C�� �X�^���v�𓾂�
 * @param[in] hFile �t�@�C�� �n���h��
 * @param[out] dosdate DOSDATE
 * @param[out] dostime DOSTIME
 * @retval 0 ����
 * @retval -1 ���s
 */
short DOSIOCALL file_getdatetime(FILEH hFile, DOSDATE* dosdate, DOSTIME* dostime)
{
	FILETIME ft;
	if (!::GetFileTime(hFile, NULL, NULL, &ft))
	{
		return -1;
	}
	return (convertDateTime(ft, dosdate, dostime)) ? 0 : -1;
}

/**
 * �t�@�C���̍폜
 * @param[in] lpPathName �t�@�C����
 * @retval 0 ����
 * @retval -1 ���s
 */
short DOSIOCALL file_delete(const OEMCHAR* lpPathName)
{
	return (::DeleteFile(lpPathName)) ? 0 : -1;
}

/**
 * �t�@�C���̑����𓾂�
 * @param[in] lpPathName �t�@�C����
 * @return �t�@�C������
 */
short DOSIOCALL file_attr(const OEMCHAR* lpPathName)
{
	return static_cast<short>(::GetFileAttributes(lpPathName));
}

/**
 * �t�@�C���̈ړ�
 * @param[in] lpExistFile �t�@�C����
 * @param[in] lpNewFile �t�@�C����
 * @retval 0 ����
 * @retval -1 ���s
 */
short DOSIOCALL file_rename(const OEMCHAR* lpExistFile, const OEMCHAR* lpNewFile)
{
	return (::MoveFile(lpExistFile, lpNewFile)) ? 0 : -1;
}

/**
 * �f�B���N�g���쐬
 * @param[in] lpPathName �p�X
 * @retval 0 ����
 * @retval -1 ���s
 */
short DOSIOCALL file_dircreate(const OEMCHAR* lpPathName)
{
	return (::CreateDirectory(lpPathName, NULL)) ? 0 : -1;
}

/**
 * �f�B���N�g���폜
 * @param[in] lpPathName �p�X
 * @retval 0 ����
 * @retval -1 ���s
 */
short DOSIOCALL file_dirdelete(const OEMCHAR* lpPathName)
{
	return (::RemoveDirectory(lpPathName)) ? 0 : -1;
}



// ---- �J�����g�t�@�C������

/**
 * �J�����g �p�X�ݒ�
 * @param[in] lpPathName �J�����g �t�@�C����
 */
void DOSIOCALL file_setcd(const OEMCHAR* lpPathName)
{
	file_cpyname(curpath, lpPathName, NELEMENTS(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

/**
 * �J�����g �p�X�擾
 * @param[in] lpFilename �t�@�C����
 * @return �p�X
 */
OEMCHAR* DOSIOCALL file_getcd(const OEMCHAR* lpFilename)
{
	file_cpyname(curfilep, lpFilename, NELEMENTS(curpath) - (int)(curfilep - curpath));
	return curpath;
}

/**
 * �J�����g �t�@�C�����J���܂�
 * @param[in] lpFilename �t�@�C����
 * @return �t�@�C�� �n���h��
 */
FILEH DOSIOCALL file_open_c(const OEMCHAR* lpFilename)
{
	return file_open(file_getcd(lpFilename));
}

/**
 * ���[�h �I�����[�ŃJ�����g �t�@�C�����J���܂�
 * @param[in] lpFilename �t�@�C����
 * @return �t�@�C�� �n���h��
 */

FILEH DOSIOCALL file_open_rb_c(const OEMCHAR* lpFilename)
{
	return file_open_rb(file_getcd(lpFilename));
}

/**
 * �J�����g �t�@�C�����쐬���܂�
 * @param[in] lpFilename �t�@�C����
 * @return �t�@�C�� �n���h��
 */
FILEH DOSIOCALL file_create_c(const OEMCHAR* lpFilename)
{
	return file_create(file_getcd(lpFilename));
}

/**
 * �J�����g �t�@�C���̍폜
 * @param[in] lpFilename �t�@�C����
 * @retval 0 ����
 * @retval -1 ���s
 */
short DOSIOCALL file_delete_c(const OEMCHAR* lpFilename)
{
	return file_delete(file_getcd(lpFilename));
}

/**
 * �J�����g �t�@�C���̑����𓾂�
 * @param[in] lpFilename �t�@�C����
 * @return �t�@�C������
 */
short DOSIOCALL file_attr_c(const OEMCHAR* lpFilename)
{
	return file_attr(file_getcd(lpFilename));
}



// ---- �t�@�C������

/**
 * WIN32_FIND_DATA �� FLINFO �ɕϊ�
 * @param[in] w32fd WIN32_FIND_DATA
 * @param[out] fli FLINFO
 * @retval true ����
 * @retval false ���s
 */
static bool DOSIOCALL setFLInfo(const WIN32_FIND_DATA& w32fd, FLINFO *fli)
{
#if !defined(_WIN32_WCE)
	if ((w32fd.dwFileAttributes & FILEATTR_DIRECTORY) && (w32fd.cFileName[0] == '.'))
	{
		return false;
	}
#endif	// !defined(_WIN32_WCE)

	if (fli)
	{
		fli->caps = FLICAPS_SIZE | FLICAPS_ATTR | FLICAPS_DATE | FLICAPS_TIME;
		fli->size = w32fd.nFileSizeLow;
		fli->attr = w32fd.dwFileAttributes;
		convertDateTime(w32fd.ftLastWriteTime, &fli->date, &fli->time);
		file_cpyname(fli->path, w32fd.cFileName, NELEMENTS(fli->path));
	}
	return true;
}

/**
 * �t�@�C���̌���
 * @param[in] lpPathName �p�X
 * @param[out] fli ��������
 * @return �t�@�C�������n���h��
 */
FLISTH DOSIOCALL file_list1st(const OEMCHAR* lpPathName, FLINFO* fli)
{
	static const OEMCHAR s_szWildCard[] = OEMTEXT("*.*");

	OEMCHAR szPath[MAX_PATH];
	file_cpyname(szPath, lpPathName, NELEMENTS(szPath));
	file_setseparator(szPath, NELEMENTS(szPath));
	file_catname(szPath, s_szWildCard, NELEMENTS(szPath));

	WIN32_FIND_DATA w32fd;
	HANDLE hFile = ::FindFirstFile(szPath, &w32fd);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (setFLInfo(w32fd, fli))
			{
				return hFile;
			}
		} while(::FindNextFile(hFile, &w32fd));
		::FindClose(hFile);
	}
	return FLISTH_INVALID;
}

/**
 * �t�@�C���̌���
 * @param[in] hList �t�@�C�������n���h��
 * @param[out] fli ��������
 * @retval SUCCESS ����
 * @retval FAILURE ���s
 */
BRESULT DOSIOCALL file_listnext(FLISTH hList, FLINFO* fli)
{
	WIN32_FIND_DATA w32fd;
	while (::FindNextFile(hList, &w32fd))
	{
		if (setFLInfo(w32fd, fli))
		{
			return SUCCESS;
		}
	}
	return FAILURE;
}

/**
 * �t�@�C�������n���h�������
 * @param[in] hList �t�@�C�������n���h��
 */
void DOSIOCALL file_listclose(FLISTH hList)
{
	::FindClose(hList);
}



// ---- �t�@�C��������

/**
 * �t�@�C�����̃|�C���^�𓾂�
 * @param[in] lpPathName �p�X
 * @return �|�C���^
 */
OEMCHAR* DOSIOCALL file_getname(const OEMCHAR* lpPathName)
{
	const OEMCHAR* ret = lpPathName;
	while (1 /* EVER */)
	{
		const int cch = milstr_charsize(lpPathName);
		if (cch == 0)
		{
			break;
		}
		else if ((cch == 1) && ((*lpPathName == '\\') || (*lpPathName == '/') || (*lpPathName == ':')))
		{
			ret = lpPathName + 1;
		}
		lpPathName += cch;
	}
	return const_cast<OEMCHAR*>(ret);
}

/**
 * �t�@�C�������폜
 * @param[in,out] lpPathName �p�X
 */
void DOSIOCALL file_cutname(OEMCHAR* lpPathName)
{
	OEMCHAR* p = file_getname(lpPathName);
	p[0] = '\0';
}

/**
 * �g���q�̃|�C���^�𓾂�
 * @param[in] lpPathName �p�X
 * @return �|�C���^
 */
OEMCHAR* DOSIOCALL file_getext(const OEMCHAR* lpPathName)
{
	const OEMCHAR* p = file_getname(lpPathName);
	const OEMCHAR* q = NULL;
	while (1 /* EVER */)
	{
		const int cch = milstr_charsize(p);
		if (cch == 0)
		{
			break;
		}
		else if ((cch == 1) && (*p == '.'))
		{
			q = p + 1;
		}
		p += cch;
	}
	if (q == NULL)
	{
		q = p;
	}
	return const_cast<OEMCHAR*>(q);
}

/**
 * �g���q���폜
 * @param[in,out] lpPathName �p�X
 */
void DOSIOCALL file_cutext(OEMCHAR* lpPathName)
{
	OEMCHAR* p = file_getname(lpPathName);
	OEMCHAR* q = NULL;
	while (1 /* EVER */)
	{
		const int cch = milstr_charsize(p);
		if (cch == 0)
		{
			break;
		}
		else if ((cch == 1) && (*p == '.'))
		{
			q = p;
		}
		p += cch;
	}
	if (q)
	{
		*q = '\0';
	}
}

/**
 * �p�X �Z�p���[�^���폜
 * @param[in,out] lpPathName �p�X
 */
void DOSIOCALL file_cutseparator(OEMCHAR* lpPathName)
{
	const int pos = OEMSTRLEN(lpPathName) - 1;
	if ((pos > 0) &&								// 2�����ȏ�Ł[
		(lpPathName[pos] == '\\') &&				// �P�c�� \ �Ł[
		(!milstr_kanji2nd(lpPathName, pos)) &&		// ������2�o�C�g�ڂ���Ȃ��ā[
		((pos != 1) || (lpPathName[0] != '\\')) &&	// '\\' �ł͂Ȃ��ā[
		((pos != 2) || (lpPathName[1] != ':')))		// '?:\' �ł͂Ȃ�������
	{
		lpPathName[pos] = '\0';
	}
}

/**
 * �p�X �Z�p���[�^��ǉ�
 * @param[in,out] lpPathName �p�X
 * @param[in] cchPathName �o�b�t�@��
 */
void DOSIOCALL file_setseparator(OEMCHAR* lpPathName, int cchPathName)
{
	const int pos = OEMSTRLEN(lpPathName) - 1;
	if ((pos < 0) ||
		((pos == 1) && (lpPathName[1] == ':')) ||
		((lpPathName[pos] == '\\') && (!milstr_kanji2nd(lpPathName, pos))) ||
		((pos + 2) >= cchPathName))
	{
		return;
	}
	lpPathName[pos + 1] = '\\';
	lpPathName[pos + 2] = '\0';
}
