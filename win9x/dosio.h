/**
 *	@file	dosio.h
 *	@brief	�t�@�C�� �A�N�Z�X�֐��Q�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/*! �R�[���K�� */
#define	DOSIOCALL	__stdcall


#define FILEH				HANDLE						/*!< �t�@�C�� �n���h�� */
#define FILEH_INVALID		(INVALID_HANDLE_VALUE)		/*!< �t�@�C�� �G���[�l */

#define FLISTH				HANDLE						/*!< �t�@�C�������n���h�� */
#define FLISTH_INVALID		(INVALID_HANDLE_VALUE)		/*!< �t�@�C�������G���[�l */

/**
 * �t�@�C�� �|�C���^�ړ��̊J�n�_
 */
enum
{
	FSEEK_SET	= 0,				/*!< �t�@�C���̐擪 */
	FSEEK_CUR	= 1,				/*!< ���݂̈ʒu */
	FSEEK_END	= 2					/*!< �t�@�C���̏I��� */
};

/**
 * �t�@�C������
 */
enum
{
	FILEATTR_READONLY	= 0x01,		/*!< �ǂݎ���p */
	FILEATTR_HIDDEN		= 0x02,		/*!< �B���t�@�C�� */
	FILEATTR_SYSTEM		= 0x04,		/*!< �V�X�e�� �t�@�C�� */
	FILEATTR_VOLUME		= 0x08,		/*!< ���H�����[�� */
	FILEATTR_DIRECTORY	= 0x10,		/*!< �f�B���N�g�� */
	FILEATTR_ARCHIVE	= 0x20		/*!< �A�[�J�C�u �t�@�C�� */
};

/**
 * �t�@�C�������t���O
 */
enum
{
	FLICAPS_SIZE		= 0x0001,	/*!< �T�C�Y */
	FLICAPS_ATTR		= 0x0002,	/*!< ���� */
	FLICAPS_DATE		= 0x0004,	/*!< ���t */
	FLICAPS_TIME		= 0x0008	/*!< ���� */
};

/**
 * @brief DOSDATE �\����
 */
struct _dosdate
{
	UINT16	year;			/*!< cx �N */
	UINT8	month;			/*!< dh �� */
	UINT8	day;			/*!< dl �� */
};
typedef struct _dosdate		DOSDATE;		/*!< DOSDATE ��` */

/**
 * @brief DOSTIME �\����
 */
struct _dostime
{
	UINT8	hour;			/*!< ch �� */
	UINT8	minute;			/*!< cl �� */
	UINT8	second;			/*!< dh �b */
};
typedef struct _dostime		DOSTIME;		/*!< DOSTIME ��` */

/**
 * @brief �t�@�C����������
 */
struct _flinfo
{
	UINT	caps;			/*!< �t���O */
	UINT32	size;			/*!< �T�C�Y */
	UINT32	attr;			/*!< ���� */
	DOSDATE	date;			/*!< ���t */
	DOSTIME	time;			/*!< ���� */
	OEMCHAR	path[MAX_PATH];	/*!< �t�@�C���� */
};
typedef struct _flinfo		FLINFO;			/*!< FLINFO ��` */

/* DOSIO:�֐��̏��� */
void dosio_init(void);
void dosio_term(void);

#ifdef __cplusplus
extern "C"
{
#endif

/* �t�@�C������ */
FILEH DOSIOCALL file_open(const OEMCHAR* lpPathName);
FILEH DOSIOCALL file_open_rb(const OEMCHAR* lpPathName);
FILEH DOSIOCALL file_create(const OEMCHAR* lpPathName);
long DOSIOCALL file_seek(FILEH hFile, long pointer, int method);
UINT DOSIOCALL file_read(FILEH hFile, void *data, UINT length);
UINT DOSIOCALL file_write(FILEH hFile, const void *data, UINT length);
short DOSIOCALL file_close(FILEH hFile);
UINT DOSIOCALL file_getsize(FILEH hFile);
short DOSIOCALL file_getdatetime(FILEH hFile, DOSDATE* dosdate, DOSTIME* dostime);
short DOSIOCALL file_delete(const OEMCHAR* lpPathName);
short DOSIOCALL file_attr(const OEMCHAR* lpPathName);
short DOSIOCALL file_rename(const OEMCHAR* lpExistFile, const OEMCHAR* lpNewFile);
short DOSIOCALL file_dircreate(const OEMCHAR* lpPathName);
short DOSIOCALL file_dirdelete(const OEMCHAR* lpPathName);

/* �J�����g�t�@�C������ */
void DOSIOCALL file_setcd(const OEMCHAR* lpPathName);
OEMCHAR* DOSIOCALL file_getcd(const OEMCHAR* lpPathName);
FILEH DOSIOCALL file_open_c(const OEMCHAR* lpFilename);
FILEH DOSIOCALL file_open_rb_c(const OEMCHAR* lpFilename);
FILEH DOSIOCALL file_create_c(const OEMCHAR* lpFilename);
short DOSIOCALL file_delete_c(const OEMCHAR* lpFilename);
short DOSIOCALL file_attr_c(const OEMCHAR* lpFilename);

/* �t�@�C������ */
FLISTH DOSIOCALL file_list1st(const OEMCHAR* lpPathName, FLINFO* fli);
BRESULT DOSIOCALL file_listnext(FLISTH hList, FLINFO* fli);
void DOSIOCALL file_listclose(FLISTH hList);

#define file_cpyname(a, b, c)	milstr_ncpy(a, b, c)		/*!< �t�@�C�����R�s�[ */
#define file_catname(a, b, c)	milstr_ncat(a, b, c)		/*!< �t�@�C�����ǉ� */
#define file_cmpname(a, b)		milstr_cmp(a, b)			/*!< �t�@�C������r */
OEMCHAR* DOSIOCALL file_getname(const OEMCHAR* lpPathName);
void DOSIOCALL file_cutname(OEMCHAR* lpPathName);
OEMCHAR* DOSIOCALL file_getext(const OEMCHAR* lpPathName);
void DOSIOCALL file_cutext(OEMCHAR* lpPathName);
void DOSIOCALL file_cutseparator(OEMCHAR* lpPathName);
void DOSIOCALL file_setseparator(OEMCHAR* lpPathName, int cchPathName);

#ifdef __cplusplus
}
#endif

#define file_createex(p, t)		file_create(p)				/*!< �t�@�C���쐬 */
#define file_createex_c(p, t)	file_create_c(p)			/*!< �t�@�C���쐬 */
