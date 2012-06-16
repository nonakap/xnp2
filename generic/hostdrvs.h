
typedef struct {
	char	fcbname[11];
	UINT8	exist;
	UINT	caps;
	UINT32	size;
	UINT32	attr;
	DOSDATE	date;
	DOSTIME	time;
} HDRVDIR;

typedef struct {
	HDRVDIR	di;
	OEMCHAR	realname[MAX_PATH];
} _HDRVLST, *HDRVLST;

typedef struct {
	HDRVDIR	di;
	OEMCHAR	path[MAX_PATH];
} HDRVPATH;


// 一覧取得
LISTARRAY hostdrvs_getpathlist(const OEMCHAR *realpath);

// ホスト側のフォルダを得る
BOOL hostdrvs_getrealdir(OEMCHAR *path, int size, char *fcb, char *dospath);

// ホスト側のファイル名を得る
BOOL hostdrvs_getrealpath(HDRVPATH *hdp, char *dospath);

// ホスト側のファイル名を作る
BOOL hostdrvs_newrealpath(HDRVPATH *hdp, char *dospath);

// ファイルハンドルリスト
void hostdrvs_fhdlallclose(LISTARRAY fhdl);
HDRVFILE hostdrvs_fhdlsea(LISTARRAY fhdl);

