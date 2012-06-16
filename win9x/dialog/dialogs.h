
struct tagFileSelectParam
{
	LPTSTR	lpszTitle;
	LPTSTR	lpszDefExt;
	LPTSTR	lpszFilter;
	int		nFilterIndex;
};
typedef struct tagFileSelectParam		FSPARAM;
typedef struct tagFileSelectParam		*PFSPARAM;
typedef const struct tagFileSelectParam	*PCFSPARAM;

struct tagCBParam
{
	LPCTSTR	lpcszString;
	int		nItemData;
};
typedef struct tagCBParam		CBPARAM;
typedef struct tagCBParam		*PCBPARAM;
typedef const struct tagCBParam	*PCCBPARAM;

struct tagCBNParam
{
	UINT	uValue;
	int		nItemData;
};
typedef struct tagCBNParam			CBNPARAM;
typedef struct tagCBNParam			*PCBNPARAM;
typedef const struct tagCBNParam	*PCCBNPARAM;


#define	SetDlgItemCheck(a, b, c)	\
			SendDlgItemMessage((a), (b), BM_SETCHECK, (c), 0)

#define	GetDlgItemCheck(a, b)		\
			(((int)SendDlgItemMessage((a), (b), BM_GETCHECK, 0, 0)) != 0)

#define	AVE(a, b)					\
			(((a) + (b)) / 2)

#define	SETLISTSTR(a, b, c)			\
			dlgs_setliststr((a), (b), (c), NELEMENTS((c)))

#define	SETnLISTSTR(a, b, c, d)		\
			dlgs_setliststr((a), (b), (c), (d))

#define	SETLISTUINT32(a, b, c)		\
			dlgs_setlistuint32((a), (b), (c), NELEMENTS((c)))

void dlgs_enablebyautocheck(HWND hWnd, UINT uID, UINT uCheckID);
void dlgs_disablebyautocheck(HWND hWnd, UINT uID, UINT uCheckID);

BOOL dlgs_openfile(HWND hWnd, PCFSPARAM pcParam,
									OEMCHAR *pszPath, UINT uSize, int *puRO);
BOOL dlgs_createfile(HWND hWnd, PCFSPARAM pcParam,
												OEMCHAR *pszPath, UINT uSize);
BOOL dlgs_createfilenum(HWND hWnd, PCFSPARAM pcParam,
												OEMCHAR *pszPath, UINT uSize);

void dlgs_browsemimpidef(HWND hWnd, UINT16 res);

void dlgs_setliststr(HWND hWnd, UINT16 res, const TCHAR **item, UINT items);
void dlgs_setlistuint32(HWND hWnd, UINT16 res, const UINT32 *item, UINT items);

void dlgs_setcbitem(HWND hWnd, UINT uID, PCCBPARAM pcItem, UINT uItems);
void dlgs_setcbnumber(HWND hWnd, UINT uID, PCCBNPARAM pcItem, UINT uItems);
void dlgs_setcbcur(HWND hWnd, UINT uID, int nItemData);
int dlgs_getcbcur(HWND hWnd, UINT uID, int nDefault);

void dlgs_setlistmidiout(HWND hWnd, UINT16 res, const OEMCHAR *defname);
void dlgs_setlistmidiin(HWND hWnd, UINT16 res, const OEMCHAR *defname);

void dlgs_drawbmp(HDC hdc, UINT8 *bmp);

BOOL dlgs_getitemrect(HWND hWnd, UINT uID, RECT *pRect);

