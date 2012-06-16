
#if defined(TARGET_MONA)
#define	FILEH				HANDLE
#define	FILEH_INVALID		NULL
#define	FLISTH				long
#define	FLISTH_INVALID		0
#else
#define	FILEH				HANDLE
#define	FILEH_INVALID		(INVALID_HANDLE_VALUE)
#define	FLISTH				HANDLE
#define	FLISTH_INVALID		(INVALID_HANDLE_VALUE)
#endif

enum {
	FSEEK_SET	= 0,
	FSEEK_CUR	= 1,
	FSEEK_END	= 2
};

enum {
	FILEATTR_READONLY	= 0x01,
	FILEATTR_HIDDEN		= 0x02,
	FILEATTR_SYSTEM		= 0x04,
	FILEATTR_VOLUME		= 0x08,
	FILEATTR_DIRECTORY	= 0x10,
	FILEATTR_ARCHIVE	= 0x20
};

enum {
	FLICAPS_SIZE		= 0x0001,
	FLICAPS_ATTR		= 0x0002,
	FLICAPS_DATE		= 0x0004,
	FLICAPS_TIME		= 0x0008
};

typedef struct {
	UINT16	year;		/* cx */
	UINT8	month;		/* dh */
	UINT8	day;		/* dl */
} DOSDATE;

typedef struct {
	UINT8	hour;		/* ch */
	UINT8	minute;		/* cl */
	UINT8	second;		/* dh */
} DOSTIME;

typedef struct {
	UINT	caps;
	UINT32	size;
	UINT32	attr;
	DOSDATE	date;
	DOSTIME	time;
	char	path[MAX_PATH];
} FLINFO;


#ifdef __cplusplus
extern "C" {
#endif

/* DOSIO:関数の準備 */
void dosio_init(void);
void dosio_term(void);

/* ファイル操作 */
FILEH file_open(const char *path);
FILEH file_open_rb(const char *path);
FILEH file_create(const char *path);
long file_seek(FILEH handle, long pointer, int method);
UINT file_read(FILEH handle, void *data, UINT length);
UINT file_write(FILEH handle, const void *data, UINT length);
short file_close(FILEH handle);
UINT file_getsize(FILEH handle);
short file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime);
short file_delete(const char *path);
short file_attr(const char *path);
short file_dircreate(const char *path);

/* カレントファイル操作 */
void file_setcd(const char *exepath);
char *file_getcd(const char *path);
FILEH file_open_c(const char *path);
FILEH file_open_rb_c(const char *path);
FILEH file_create_c(const char *path);
short file_delete_c(const char *path);
short file_attr_c(const char *path);

FLISTH file_list1st(const char *dir, FLINFO *fli);
BOOL file_listnext(FLISTH hdl, FLINFO *fli);
void file_listclose(FLISTH hdl);

#define file_cpyname(p, n, m)	milstr_ncpy(p, n, m)
#if defined(WIN32)
#define file_cmpname(p, n)		milstr_cmp(p, n)
#else
#define file_cmpname(p, n)		strcmp(p, n)
#endif
void file_catname(char *path, const char *name, int maxlen);
char *file_getname(const char *path);
void file_cutname(char *path);
char *file_getext(const char *path);
void file_cutext(char *path);
void file_cutseparator(char *path);
void file_setseparator(char *path, int maxlen);

#ifdef	__cplusplus
}
#endif

