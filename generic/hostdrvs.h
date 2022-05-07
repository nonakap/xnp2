/**
 * @file	hostdrvs.h
 * @brief	Interface of host-drive
 */

#pragma once

#if defined(SUPPORT_HOSTDRV)

#include "hostdrv.h"
#include "dosio.h"

/**
 * dos error codes : see int2159-BX0000
 */
enum
{
	ERR_NOERROR				= 0x00,
	ERR_FILENOTFOUND		= 0x02,		/*!< File not found */
	ERR_PATHNOTFOUND		= 0x03,		/*!< Path not found */
	ERR_NOHANDLESLEFT		= 0x04,		/*!< No handles left */
	ERR_ACCESSDENIED		= 0x05,
	ERR_INVALDACCESSMODE	= 0x0c,		/*!< Invalid access mode */
	ERR_ATTEMPTEDCURRDIR	= 0x10,
	ERR_NOMOREFILES			= 0x12,
	ERR_DISKWRITEPROTECTED	= 0x13,
	ERR_WRITEFAULT			= 0x1d,
	ERR_READFAULT			= 0x1e
};

/**
 * @brief DOS �t�@�C�����
 */
struct tagHostDrvFile
{
	char	fcbname[11];	/*!< FCB �� */
	UINT	caps;			/*!< ���t���O */
	UINT32	size;			/*!< �T�C�Y */
	UINT32	attr;			/*!< ���� */
	DOSDATE	date;			/*!< ���t */
	DOSTIME	time;			/*!< ���� */
};
typedef struct tagHostDrvFile HDRVFILE;		/*!< ��` */

/**
 * @brief �t�@�C�� ���X�g���
 */
struct tagHostDrvList
{
	HDRVFILE file;					/*!< DOS �t�@�C����� */
	OEMCHAR szFilename[MAX_PATH];	/*!< �t�@�C���� */
};
typedef struct tagHostDrvList _HDRVLST;		/*!< ��` */
typedef struct tagHostDrvList *HDRVLST;		/*!< ��` */

/**
 * @brief �p�X���
 */
struct tagHostDrvPath
{
	HDRVFILE file;				/*!< DOS �t�@�C����� */
	OEMCHAR szPath[MAX_PATH];	/*!< �p�X */
};
typedef struct tagHostDrvPath HDRVPATH;		/*!< ��` */

LISTARRAY hostdrvs_getpathlist(const HDRVPATH *phdp, const char *lpMask, UINT nAttr);
UINT hostdrvs_getrealdir(HDRVPATH *phdp, char *lpFcbname, const char *lpDosPath);
UINT hostdrvs_appendname(HDRVPATH *phdp, const char *lpFcbname);
UINT hostdrvs_getrealpath(HDRVPATH *phdp, const char *lpDosPath);
void hostdrvs_fhdlallclose(LISTARRAY fileArray);
HDRVHANDLE hostdrvs_fhdlsea(LISTARRAY fileArray);

#endif	/* defined(SUPPORT_HOSTDRV) */
