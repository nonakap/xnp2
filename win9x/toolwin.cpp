#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"winloc.h"
#include	"oemtext.h"
#include	"dosio.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"toolwin.h"
#include	"ini.h"
#include	"np2class.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"diskdrv.h"


extern WINLOCEX np2_winlocexallwin(HWND base);


enum {
	IDC_TOOLHDDACC			= 0,
	IDC_TOOLFDD1ACC,
	IDC_TOOLFDD1LIST,
	IDC_TOOLFDD1BROWSE,
	IDC_TOOLFDD1EJECT,
	IDC_TOOLFDD2ACC,
	IDC_TOOLFDD2LIST,
	IDC_TOOLFDD2BROWSE,
	IDC_TOOLFDD2EJECT,
	IDC_TOOLRESET,
	IDC_TOOLPOWER,
	IDC_MAXITEMS,

	IDC_BASE				= 3000
};

enum {
	TCTL_STATIC				= 0,
	TCTL_BUTTON				= 1,
	TCTL_DDLIST				= 2
};

typedef struct {
	OEMCHAR	main[MAX_PATH];
	OEMCHAR	font[64];
	SINT32	fontsize;
	UINT32	color1;
	UINT32	color2;
} TOOLSKIN;

typedef struct {
	UINT		tctl;
const OEMCHAR	*text;
	short		posx;
	short		posy;
	short		width;
	short		height;
	short		extend;
	short		padding;
} SUBITEM;

typedef struct {
	HWND			hwnd;
	WINLOCEX		wlex;
	HBITMAP			hbmp;
	UINT8			fddaccess[2];
	UINT8			hddaccess;
	UINT8			_padding;
	int				winflg;
	int				wingx;
	int				wingy;
	int				wintx;
	int				winty;
	UINT			parentcn;
	int				parentx;
	int				parenty;
	HFONT			hfont;
	HDC				hdcfont;
	HBRUSH			access[2];
	HWND			sub[IDC_MAXITEMS];
	SUBCLASSPROC	subproc[IDC_MAXITEMS];
} TOOLWIN;

#define	GTWLP_FOCUS		(NP2GWLP_SIZE + (0 * sizeof(LONG_PTR)))
#define	GTWLP_SIZE		(NP2GWLP_SIZE + (1 * sizeof(LONG_PTR)))

		NP2TOOL		np2tool;
static	TOOLSKIN	toolskin;
static	SUBITEM		subitem[IDC_MAXITEMS];
static	TOOLWIN		toolwin;

#include	"toolwin.res"


typedef struct {
	UINT16	idc;
	UINT8	*counter;
} DISKACC;

static const UINT8 fddlist[FDDLIST_DRV] = {
					IDC_TOOLFDD1LIST, IDC_TOOLFDD2LIST};

static const DISKACC diskacc[3] = {
					{IDC_TOOLFDD1ACC,	&toolwin.fddaccess[0]},
					{IDC_TOOLFDD2ACC,	&toolwin.fddaccess[1]},
					{IDC_TOOLHDDACC,	&toolwin.hddaccess}};


// ----

static HBITMAP skinload(const OEMCHAR *path) {

	OEMCHAR	fname[MAX_PATH];
	UINT	i;
	HBITMAP	ret;

	ZeroMemory(&toolskin, sizeof(toolskin));
	toolskin.fontsize = 12;
	milstr_ncpy(toolskin.font, str_deffont, NELEMENTS(toolskin.font));
	toolskin.color1 = 0x600000;
	toolskin.color2 = 0xff0000;
	if (path) {
		ini_read(path, skintitle, skinini1, NELEMENTS(skinini1));
	}
	if (toolskin.main[0]) {
		ZeroMemory(subitem, sizeof(defsubitem));
		for (i=0; i<IDC_MAXITEMS; i++) {
			subitem[i].tctl = defsubitem[i].tctl;
			subitem[i].text = defsubitem[i].text;
		}
	}
	else {
		CopyMemory(subitem, defsubitem, sizeof(defsubitem));
	}
	if (path) {
		ini_read(path, skintitle, skinini2, NELEMENTS(skinini2));
	}
	if (toolskin.main[0]) {
		milstr_ncpy(fname, path, NELEMENTS(fname));
		file_cutname(fname);
		file_catname(fname, toolskin.main, NELEMENTS(fname));
#if defined(OSLANG_UTF8)
		TCHAR tchr[MAX_PATH];
		oemtotchar(tchr, NELEMENTS(tchr), fname, -1);
#else
		const TCHAR *tchr = fname;
#endif
		ret = (HBITMAP)LoadImage(g_hInstance, tchr, IMAGE_BITMAP,
													0, 0, LR_LOADFROMFILE);
		if (ret != NULL) {
			return(ret);
		}
	}
	return(LoadBitmap(g_hInstance, _T("NP2TOOL")));
}


// ----

static void calctextsize(OEMCHAR *path, int leng, const OEMCHAR *p, int width) {

	HDC		hdc;
	SIZE	cur;
	OEMCHAR	work[MAX_PATH];
	int		l;
	SIZE	tail;
	int		pos;
	int		step;

	milstr_ncpy(path, p, leng);
	hdc = toolwin.hdcfont;
	GetTextExtentPoint32(hdc, p, OEMSTRLEN(p), &cur);
	if (cur.cx < width) {
		return;
	}

	file_cutname(path);
	file_cutseparator(path);
	file_cutname(path);
	file_cutseparator(path);
	l = OEMSTRLEN(path);
	work[0] = '\0';
	if (l) {
		milstr_ncpy(work, str_browse, NELEMENTS(work));
	}
	milstr_ncat(work, p + l, NELEMENTS(work));
	GetTextExtentPoint32(hdc, work, OEMSTRLEN(work), &tail);
	pos = 0;
	while(pos < l) {
#if defined(_UNICODE)
		step = 1;
#else
		step = (IsDBCSLeadByte((BYTE)p[pos]))?2:1;
#endif
		GetTextExtentPoint32(hdc, p, pos + step, &cur);
		if (cur.cx + tail.cx >= width) {
			break;
		}
		pos += step;
	}
	if (pos < leng) {
		path[pos] = '\0';
	}
	milstr_ncat(path, work, leng);
}

static void setlist(HWND hwnd, const TOOLFDD *fdd, UINT sel) {

	RECT		rc;
	int			width;
	OEMCHAR		basedir[MAX_PATH];
	UINT		i;
const OEMCHAR	*p;
	OEMCHAR		dir[MAX_PATH];
const OEMCHAR	*q;

	SendMessage(hwnd, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	GetClientRect(hwnd, &rc);
	width = rc.right - rc.left - 6;			// border size?
	basedir[0] = '\0';
	if (sel < fdd->cnt) {
		milstr_ncpy(basedir, fdd->name[fdd->pos[sel]], NELEMENTS(basedir));
		file_cutname(basedir);
	}
	for (i=0; i<fdd->cnt; i++) {
		p = fdd->name[fdd->pos[i]];
		milstr_ncpy(dir, p, NELEMENTS(dir));
		file_cutname(dir);
		if (!file_cmpname(basedir, dir)) {
			q = file_getname(p);
		}
		else {
			calctextsize(dir, NELEMENTS(dir), p, width);
			q = dir;
		}
		SendMessage(hwnd, CB_INSERTSTRING, (WPARAM)i, (LPARAM)q);
		p += NELEMENTS(fdd->name[0]);
	}
	if (sel < fdd->cnt) {
		SendMessage(hwnd, CB_SETCURSEL, (WPARAM)sel, (LPARAM)0);
	}
}

static void sellist(UINT drv) {

	HWND	hwnd;
	TOOLFDD	*fdd;
	UINT	sel;

	if (drv >= FDDLIST_DRV) {
		return;
	}
	hwnd = toolwin.sub[fddlist[drv]];
	fdd = np2tool.fdd + drv;
	sel = (UINT)SendMessage(hwnd, CB_GETCURSEL, 0, 0);
	if (sel < fdd->cnt) {
		diskdrv_setfdd(drv, fdd->name[fdd->pos[sel]], 0);
		fdd->insert = 1;
		setlist(hwnd, fdd, sel);
	}
}

static void remakefddlist(HWND hwnd, TOOLFDD *fdd) {

	OEMCHAR	*p;
	UINT	cnt;
	OEMCHAR	*q;
	OEMCHAR	*fname[FDDLIST_MAX];
	UINT	i;
	UINT	j;
	UINT	sel;

	p = fdd->name[0];
	for (cnt=0; cnt<FDDLIST_MAX; cnt++) {
		if (p[0] == '\0') {
			break;
		}
		q = file_getname(p);
		fname[cnt] = q;
		for (i=0; i<cnt; i++) {
			if (file_cmpname(q, fname[fdd->pos[i]]) < 0) {
				break;
			}
		}
		for (j=cnt; j>i; j--) {
			fdd->pos[j] = fdd->pos[j-1];
		}
		fdd->pos[i] = cnt;
		p += NELEMENTS(fdd->name[0]);
	}
	fdd->cnt = cnt;
	sel = (UINT)-1;
	if (fdd->insert) {
		for (i=0; i<cnt; i++) {
			if (fdd->pos[i] == 0) {
				sel = i;
				break;
			}
		}
	}
	setlist(hwnd, fdd, sel);
}

static void accdraw(HWND hWnd, UINT8 count) {

	HDC			hdc;
	PAINTSTRUCT	ps;
	RECT		rc;
	HBRUSH		hbrush;

	hdc = BeginPaint(hWnd, &ps);
	GetClientRect(hWnd, &rc);
	hbrush = (HBRUSH)SelectObject(hdc, toolwin.access[(count)?1:0]);
	PatBlt(hdc, rc.left, rc.top, rc.right, rc.bottom, PATCOPY);
	SelectObject(hdc, hbrush);
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK twsub(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	UINT	idc;
	int		dir;
	UINT	newidc;
	int		files;
	OEMCHAR	fname[MAX_PATH];

	idc = GetWindowLong(hWnd, GWL_ID) - IDC_BASE;
	if (idc >= IDC_MAXITEMS) {
		return(0);
	}
	if (msg == WM_KEYDOWN) {
		if ((short)wp == VK_TAB) {
			dir = (GetKeyState(VK_SHIFT) >= 0)?1:-1;
			newidc = idc;
			do {
				newidc += dir;
				if (newidc >= IDC_MAXITEMS) {
					newidc = (dir >= 0)?0:(IDC_MAXITEMS - 1);
				}
				if ((toolwin.sub[newidc] != NULL) &&
					(subitem[newidc].tctl != TCTL_STATIC)) {
					SetFocus(toolwin.sub[newidc]);
					break;
				}
			} while(idc != newidc);
		}
		else if ((short)wp == VK_RETURN) {
			if (subitem[idc].tctl == TCTL_BUTTON) {
				return(CallWindowProc(toolwin.subproc[idc],
										hWnd, WM_KEYDOWN, VK_SPACE, 0));
			}
		}
	}
	else if (msg == WM_DROPFILES) {
   	    files = DragQueryFile((HDROP)wp, (UINT)-1, NULL, 0);
		if (files == 1) {
#if defined(OSLANG_UTF8)
			TCHAR tchr[MAX_PATH];
			DragQueryFile((HDROP)wp, 0, tchr, NELEMENTS(tchr));
			tchartooem(fname, NELEMENTS(fname), tchr, -1);
#else
			DragQueryFile((HDROP)wp, 0, fname, NELEMENTS(fname));
#endif
			if (idc == IDC_TOOLFDD1LIST) {
				diskdrv_setfdd(0, fname, 0);
				toolwin_setfdd(0, fname);
			}
			else if (idc == IDC_TOOLFDD2LIST) {
				diskdrv_setfdd(1, fname, 0);
				toolwin_setfdd(1, fname);
			}
		}
		DragFinish((HDROP)wp);
		return(FALSE);
	}
	else if (msg == WM_PAINT) {
		if (idc == IDC_TOOLHDDACC) {
			accdraw(hWnd, toolwin.hddaccess);
			return(FALSE);
		}
		else if (idc == IDC_TOOLFDD1ACC) {
			accdraw(hWnd, toolwin.fddaccess[0]);
			return(FALSE);
		}
		else if (idc == IDC_TOOLFDD2ACC) {
			accdraw(hWnd, toolwin.fddaccess[1]);
			return(FALSE);
		}
	}
	else if (msg == WM_SETFOCUS) {
		SetWindowLongPtr(GetParent(hWnd), GTWLP_FOCUS, idc);
	}
	return(CallWindowProc(toolwin.subproc[idc], hWnd, msg, wp, lp));
}

static void toolwincreate(HWND hWnd) {

#if defined(OSLANG_UTF8)
	TCHAR fontface[64];
	oemtotchar(fontface, NELEMENTS(fontface), toolskin.font, -1);
#else
	const TCHAR *fontface = toolskin.font;
#endif
	toolwin.hfont = CreateFont(toolskin.fontsize, 0, 0, 0, 0, 0, 0, 0,
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, fontface);
	HDC hdc = GetDC(NULL);
	toolwin.hdcfont = CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);
	SelectObject(toolwin.hdcfont, toolwin.hfont);

	toolwin.access[0] = CreateSolidBrush(0x000060);
	toolwin.access[1] = CreateSolidBrush(0x0000ff);

	const SUBITEM *p = subitem;
	UINT i;
	for (i=0; i<IDC_MAXITEMS; i++) {
		HWND sub = NULL;
		const TCHAR *cls = NULL;
		DWORD style;
		switch(p->tctl) {
			case TCTL_STATIC:
				cls = str_static;
				style = 0;
				break;

			case TCTL_BUTTON:
				if (p->extend == 0) {
					cls = str_button;
					style = BS_PUSHBUTTON;
				}
				else if (p->extend == 1) {
					cls = str_button;
					style = BS_OWNERDRAW;
				}
				break;

			case TCTL_DDLIST:
				cls = str_combobox;
				style = CBS_DROPDOWNLIST | WS_VSCROLL;
				break;
		}
		if ((cls) && (p->width > 0) && (p->height > 0)) {
#if defined(OSLANG_UTF8)
			TCHAR ptext[64];
			oemtotchar(ptext, NELEMENTS(ptext), p->text, -1);
#else
			const TCHAR *ptext = p->text;
#endif
			sub = CreateWindow(cls, ptext, WS_CHILD | WS_VISIBLE | style,
							p->posx, p->posy, p->width, p->height,
							hWnd, (HMENU)(i + IDC_BASE), g_hInstance, NULL);
		}
		toolwin.sub[i] = sub;
		toolwin.subproc[i] = NULL;
		if (sub) {
			toolwin.subproc[i] = (SUBCLASSPROC)GetWindowLongPtr(sub,
															GWLP_WNDPROC);
			SetWindowLongPtr(sub, GWLP_WNDPROC, (LONG_PTR)twsub);
			SendMessage(sub, WM_SETFONT, (WPARAM)toolwin.hfont,
														MAKELPARAM(TRUE, 0));
		}
		p++;
	}
	for (i=0; i<FDDLIST_DRV; i++) {
		HWND sub = toolwin.sub[fddlist[i]];
		if (sub) {
			DragAcceptFiles(sub, TRUE);
			remakefddlist(sub, np2tool.fdd + i);
		}
	}
	for (i=0; i<IDC_MAXITEMS; i++) {
		if ((toolwin.sub[i]) && (subitem[i].tctl != TCTL_STATIC)) {
			break;
		}
	}
	SetWindowLongPtr(hWnd, GTWLP_FOCUS, i);
}

static void toolwindestroy(void) {

	UINT	i;
	HWND	sub;

	if (toolwin.hbmp) {
		for (i=0; i<IDC_MAXITEMS; i++) {
			sub = toolwin.sub[i];
			if (sub) {
				DestroyWindow(sub);
			}
		}
		DeleteObject(toolwin.access[0]);
		DeleteObject(toolwin.access[1]);
		DeleteObject(toolwin.hdcfont);
		DeleteObject(toolwin.hfont);
		DeleteObject(toolwin.hbmp);
		toolwin.hbmp = NULL;
	}
}

static void toolwinpaint(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;
	BITMAP		bmp;
	HDC			hmdc;

	hdc = BeginPaint(hWnd, &ps);
	if (toolwin.hbmp) {
		GetObject(toolwin.hbmp, sizeof(BITMAP), &bmp);
		hmdc = CreateCompatibleDC(hdc);
		SelectObject(hmdc, toolwin.hbmp);
		BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, SRCCOPY);
		DeleteDC(hmdc);
	}
	EndPaint(hWnd, &ps);
}

static void tooldrawbutton(HWND hWnd, UINT idc, LPDRAWITEMSTRUCT lpdis) {

	POINT	pt;
	HWND	sub;
	RECT	rect;
	HDC		hdc;
	HDC		hmdc;
	RECT	btn;

	idc -= IDC_BASE;
	if (idc >= IDC_MAXITEMS) {
		return;
	}
	pt.x = 0;
	pt.y = 0;
	ClientToScreen(hWnd, &pt);
	sub = toolwin.sub[idc];
	GetWindowRect(sub, &rect);
	btn.left = 0;
	btn.top = 0;
	btn.right = rect.right - rect.left;
	btn.bottom = rect.bottom - rect.top;
	hdc = lpdis->hDC;
	if (toolwin.hbmp) {
		hmdc = CreateCompatibleDC(hdc);
		SelectObject(hmdc, toolwin.hbmp);
		BitBlt(hdc, 0, 0, btn.right, btn.bottom,
					hmdc, rect.left - pt.x, rect.top - pt.y, SRCCOPY);
		DeleteDC(hmdc);
	}
	if (lpdis->itemState & ODS_FOCUS) {
		DrawFocusRect(hdc, &btn);
	}
}


// ----

static void setSkinMruMenu(HMENU hMenu)
{
	HMENU		hmenuSub;
const OEMCHAR	*pcszBase;
	UINT		uCount;
	OEMCHAR		*pszMru;
const OEMCHAR	*pcszMruList[SKINMRU_MAX];
	UINT		i;
	UINT		uID[SKINMRU_MAX];
	UINT		j;
	UINT		uFlag;

	for (i=0; i<SKINMRU_MAX; i++)
	{
		DeleteMenu(hMenu, IDM_TOOL_SKINMRU + i, MF_BYCOMMAND);
	}

	if (!menu_searchmenu(hMenu, IDM_TOOL_SKINDEF, &hmenuSub, NULL))
	{
		return;
	}

	pcszBase = np2tool.skin;

	CheckMenuItem(hMenu, IDM_TOOL_SKINDEF, MFCHECK(pcszBase[0] == '\0'));

	for (uCount=0; uCount<SKINMRU_MAX; uCount++)
	{
		pszMru = np2tool.skinmru[uCount];
		if (pszMru[0] == '\0')
		{
			break;
		}
		pszMru = file_getname(pszMru);
		for (i=0; i<uCount; i++)
		{
			if (file_cmpname(pszMru, pcszMruList[uID[i]]) < 0)
			{
				break;
			}
		}
		for (j=uCount; j>i; j--)
		{
			uID[j] = uID[j-1];
		}
		uID[i] = uCount;
		pcszMruList[uCount] = pszMru;
	}

	for (i=0; i<uCount; i++)
	{
		j = uID[i];
		uFlag = MFCHECK(!file_cmpname(pcszBase, np2tool.skinmru[j]));
#if defined(OSLANG_UTF8)
		TCHAR szPath[MAX_PATH];
		oemtotchar(szPath, NELEMENTS(szPath), pcszMruList[j], -1);
#else
		const TCHAR *szPath = pcszMruList[j];
#endif
		AppendMenu(hmenuSub, MF_STRING + uFlag, IDM_TOOL_SKINMRU + j, szPath);
	}
}

#if 0
static HMENU createskinmenu(void)
{
	HMENU		hMenu;
const OEMCHAR	*pcszBase;
	UINT		uCount;
	OEMCHAR		*pszMru;
const OEMCHAR	*pcszMruList[SKINMRU_MAX];
	UINT		i;
	UINT		uID[SKINMRU_MAX];
	UINT		j;
	UINT		uFlag;

	hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_TOOLWIN));

	pcszBase = np2tool.skin;

	CheckMenuItem(hMenu, IDM_TOOL_SKINDEF, MFCHECK(pcszBase[0] == '\0'));
	for (uCount=0; uCount<SKINMRU_MAX; uCount++)
	{
		pszMru = np2tool.skinmru[uCount];
		if (pszMru[0] == '\0')
		{
			break;
		}
		pszMru = file_getname(pszMru);
		for (i=0; i<uCount; i++)
		{
			if (!file_cmpname(pszMru, pcszMruList[uID[i]]))
			{
				break;
			}
		}
		for (j=uCount; j>i; j--)
		{
			uID[j] = uID[j-1];
		}
		uID[i] = uCount;
		pcszMruList[uCount] = pszMru;
	}
	for (i=0; i<uCount; i++)
	{
		j = uID[i];
		uFlag = MFCHECK(!file_cmpname(pcszBase, np2tool.skinmru[j]));
#if defined(OSLANG_UTF8)
		TCHAR szPath[MAX_PATH];
		oemtotchar(szPath, NELEMENTS(szPath), pcszMruList[j], -1);
#else
		const TCHAR *szPath = pcszMruList[j];
#endif
		AppendMenu(hMenu, MF_STRING + uFlag, IDM_TOOL_SKINMRU + j, szPath);
	}
	return hMenu;
}
#endif

static void skinchange(HWND hWnd)
{
const OEMCHAR	*p;
	UINT		i;
	HBITMAP		hbmp;
	BITMAP		bmp;
	WINLOCEX	wlex;

	p = np2tool.skin;
	if (p[0]) {
		for (i=0; i<(SKINMRU_MAX - 1); i++) {
			if (!file_cmpname(p, np2tool.skinmru[i])) {
				break;
			}
		}
		while(i > 0) {
			CopyMemory(np2tool.skinmru[i], np2tool.skinmru[i-1],
												sizeof(np2tool.skinmru[0]));
			i--;
		}
		file_cpyname(np2tool.skinmru[0], p, NELEMENTS(np2tool.skinmru[0]));
	}
	setSkinMruMenu(np2class_gethmenu(hWnd));
	setSkinMruMenu(GetSystemMenu(hWnd, FALSE));
	DrawMenuBar(hWnd);
	sysmng_update(SYS_UPDATEOSCFG);

	wlex = np2_winlocexallwin(g_hWndMain);
	winlocex_setholdwnd(wlex, hWnd);
	toolwindestroy();
	hbmp = skinload(np2tool.skin);
	if (hbmp == NULL) {
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		return;
	}
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	toolwin.hbmp = hbmp;
	winloc_setclientsize(hWnd, bmp.bmWidth, bmp.bmHeight);
	toolwincreate(hWnd);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}


// ----

static void openpopup(HWND hWnd, LPARAM lp)
{
	HMENU	hMenu;
	POINT	pt;

	hMenu = CreatePopupMenu();
	if (!winui_en)
	{
		menu_addmenu(hMenu, 0, np2class_gethmenu(g_hWndMain), FALSE);
	}
	menu_addmenures(hMenu, -1, IDR_TOOLWIN, FALSE);
	menu_addmenures(hMenu, -1, IDR_CLOSE, TRUE);
	setSkinMruMenu(hMenu);
	pt.x = LOWORD(lp);
	pt.y = HIWORD(lp);
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
	DestroyMenu(hMenu);
}

static void toolwin_oncreate(HWND hWnd)
{
	HMENU	hMenu;
	int		nCount;

	np2class_wmcreate(hWnd);
	setSkinMruMenu(np2class_gethmenu(hWnd));

	hMenu = GetSystemMenu(hWnd, FALSE);
	nCount = menu_addmenures(hMenu, 0, IDR_TOOLWIN, FALSE);
	if (nCount)
	{
		InsertMenu(hMenu, nCount, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		setSkinMruMenu(hMenu);
	}

	np2class_windowtype(hWnd, (np2tool.type & 1) << 1);
	toolwincreate(hWnd);
}

static LRESULT CALLBACK twproc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
	BOOL		r;
	UINT		idc;
	WINLOCEX	wlex;

	switch(uMsg) {
		case WM_CREATE:
			toolwin_oncreate(hWnd);
			break;

		case WM_SYSCOMMAND:
			switch(wp) {
				case IDM_TOOL_SKINSEL:
				case IDM_TOOL_SKINDEF:
				case IDM_TOOL_SKINMRU + 0:
				case IDM_TOOL_SKINMRU + 1:
				case IDM_TOOL_SKINMRU + 2:
				case IDM_TOOL_SKINMRU + 3:
					return(SendMessage(hWnd, WM_COMMAND, wp, lp));

				default:
					return(DefWindowProc(hWnd, uMsg, wp, lp));
			}
			break;

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDC_BASE + IDC_TOOLFDD1LIST:
					if (HIWORD(wp) == CBN_SELCHANGE) {
						sellist(0);
					}
					break;

				case IDC_BASE + IDC_TOOLFDD1BROWSE:
					if (!winui_en) {
						SendMessage(g_hWndMain, WM_COMMAND, IDM_FDD1OPEN, 0);
					}
					break;

				case IDC_BASE + IDC_TOOLFDD1EJECT:
					diskdrv_setfdd(0, NULL, 0);
					toolwin_setfdd(0, NULL);
					break;

				case IDC_BASE + IDC_TOOLFDD2LIST:
					if (HIWORD(wp) == CBN_SELCHANGE) {
						sellist(1);
					}
					break;

				case IDC_BASE + IDC_TOOLFDD2BROWSE:
					if (!winui_en) {
						SendMessage(g_hWndMain, WM_COMMAND, IDM_FDD2OPEN, 0);
					}
					break;

				case IDC_BASE + IDC_TOOLFDD2EJECT:
					diskdrv_setfdd(1, NULL, 0);
					toolwin_setfdd(1, NULL);
					break;

				case IDC_BASE + IDC_TOOLRESET:
					if (!winui_en) {
						SendMessage(g_hWndMain, WM_COMMAND, IDM_RESET, 0);
						SetForegroundWindow(g_hWndMain);
					}
					break;

				case IDC_BASE + IDC_TOOLPOWER:
					if (!winui_en) {
						SendMessage(g_hWndMain, WM_CLOSE, 0, 0L);
					}
					break;

				case IDM_TOOL_SKINSEL:
					soundmng_disable(SNDPROC_TOOL);
					r = dlgs_openfile(hWnd, &fpSkin, np2tool.skin,
											NELEMENTS(np2tool.skin), NULL);
					soundmng_enable(SNDPROC_TOOL);
					if (r) {
						skinchange(hWnd);
					}
					break;

				case IDM_TOOL_SKINDEF:
					np2tool.skin[0] = '\0';
					skinchange(hWnd);
					break;

				case IDM_TOOL_SKINMRU + 0:
				case IDM_TOOL_SKINMRU + 1:
				case IDM_TOOL_SKINMRU + 2:
				case IDM_TOOL_SKINMRU + 3:
					file_cpyname(np2tool.skin,
							np2tool.skinmru[LOWORD(wp) - IDM_TOOL_SKINMRU],
							NELEMENTS(np2tool.skin));
					skinchange(hWnd);
					break;

				case IDM_CLOSE:
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;

				default:
					if (!winui_en) {
						return(SendMessage(g_hWndMain, uMsg, wp, lp));
					}
					break;
			}
			break;

		case WM_KEYDOWN:						// TAB‚ð‰Ÿ‚µ‚½Žž‚É•œ‹A
			if ((short)wp == VK_TAB) {
				idc = (UINT)GetWindowLongPtr(hWnd, GTWLP_FOCUS);
				if (idc < IDC_MAXITEMS) {
					SetFocus(toolwin.sub[idc]);
				}
				return(0);
			}
			return(SendMessage(g_hWndMain, uMsg, wp, lp));

		case WM_KEYUP:
			if ((short)wp == VK_TAB) {
				return(0);
			}
			return(SendMessage(g_hWndMain, uMsg, wp, lp));

		case WM_PAINT:
			toolwinpaint(hWnd);
			break;

		case WM_DRAWITEM:
			tooldrawbutton(hWnd, (UINT)wp, (LPDRAWITEMSTRUCT)lp);
			break;

		case WM_ENTERMENULOOP:
			soundmng_disable(SNDPROC_TOOL);
			break;

		case WM_EXITMENULOOP:
			soundmng_enable(SNDPROC_TOOL);
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_TOOL);
			winlocex_destroy(toolwin.wlex);
			toolwin.wlex = np2_winlocexallwin(hWnd);
			break;

		case WM_MOVING:
			winlocex_moving(toolwin.wlex, (RECT *)lp);
			break;

		case WM_EXITSIZEMOVE:
			winlocex_destroy(toolwin.wlex);
			toolwin.wlex = NULL;
			soundmng_enable(SNDPROC_TOOL);
			break;

		case WM_MOVE:
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE))) {
				RECT rc;
				GetWindowRect(hWnd, &rc);
				np2tool.posx = rc.left;
				np2tool.posy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_CLOSE:
			sysmenu_settoolwin(0);
			sysmng_update(SYS_UPDATEOSCFG);
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			np2class_wmdestroy(hWnd);
			toolwindestroy();
			toolwin.hwnd = NULL;
			break;

		case WM_LBUTTONDOWN:
			if (np2tool.type & 1) {
				return(SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0L));
			}
			break;

		case WM_RBUTTONDOWN:
			openpopup(hWnd, lp);
			break;

		case WM_LBUTTONDBLCLK:
			np2tool.type ^= 1;
			wlex = np2_winlocexallwin(g_hWndMain);
			winlocex_setholdwnd(wlex, hWnd);
			np2class_windowtype(hWnd, (np2tool.type & 1) << 1);
			winlocex_move(wlex);
			winlocex_destroy(wlex);
			sysmng_update(SYS_UPDATEOSCFG);
			break;

		default:
			return(DefWindowProc(hWnd, uMsg, wp, lp));
	}
	return(0);
}

BOOL toolwin_initapp(HINSTANCE hInstance) {

	WNDCLASS wc;

	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = twproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = GTWLP_SIZE;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_TOOLWIN);
	wc.lpszClassName = np2toolclass;
	return(RegisterClass(&wc));
}

void toolwin_create(HINSTANCE hInstance) {

	HBITMAP	hbmp;
	BITMAP	bmp;
	TCHAR	szCaption[128];
	HWND	hWnd;

	if (toolwin.hwnd) {
		return;
	}
	ZeroMemory(&toolwin, sizeof(toolwin));
	hbmp = skinload(np2tool.skin);
	if (hbmp == NULL) {
		goto twope_err1;
	}
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	toolwin.hbmp = hbmp;

	loadstringresource(LOWORD(IDS_CAPTION_TOOL),
										szCaption, NELEMENTS(szCaption));
	hWnd = CreateWindow(np2toolclass, szCaption,
							WS_SYSMENU | WS_MINIMIZEBOX,
							np2tool.posx, np2tool.posy,
							bmp.bmWidth, bmp.bmHeight,
							NULL, NULL, hInstance, NULL);
	winloc_setclientsize(hWnd, bmp.bmWidth, bmp.bmHeight);
	toolwin.hwnd = hWnd;
	if (hWnd == NULL) {
		goto twope_err2;
	}
	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOWNOACTIVATE);
	SetForegroundWindow(g_hWndMain);
	return;

twope_err2:
	DeleteObject(hbmp);

twope_err1:
	sysmenu_settoolwin(0);
	sysmng_update(SYS_UPDATEOSCFG);
	return;
}

void toolwin_destroy(void) {

	if (toolwin.hwnd) {
		DestroyWindow(toolwin.hwnd);
	}
}

HWND toolwin_gethwnd(void) {

	return(toolwin.hwnd);
}

void toolwin_setfdd(UINT8 drv, const OEMCHAR *name) {

	TOOLFDD	*fdd;
	OEMCHAR	*q;
	OEMCHAR	*p;
	UINT	i;
	HWND	sub;

	if (drv >= FDDLIST_DRV) {
		return;
	}
	fdd = np2tool.fdd + drv;
	if ((name == NULL) || (name[0] == '\0')) {
		fdd->insert = 0;
	}
	else {
		fdd->insert = 1;
		q = fdd->name[0];
		for (i=0; i<(FDDLIST_MAX - 1); i++) {
			if (!file_cmpname(q, name)) {
				break;
			}
			q += NELEMENTS(fdd->name[0]);
		}
		p = q - NELEMENTS(fdd->name[0]);
		while(i > 0) {
			i--;
			CopyMemory(q, p, sizeof(fdd->name[0]));
			p -= NELEMENTS(fdd->name[0]);
			q -= NELEMENTS(fdd->name[0]);
		}
		file_cpyname(fdd->name[0], name, NELEMENTS(fdd->name[0]));
	}
	sysmng_update(SYS_UPDATEOSCFG);
	if (toolwin.hwnd != NULL) {
		sub = toolwin.sub[fddlist[drv]];
		if (sub) {
			remakefddlist(sub, fdd);
			SetForegroundWindow(g_hWndMain);
		}
	}
}

static void setdiskacc(UINT num, UINT8 count) {

const DISKACC	*acc;
	HWND		sub;

	if (toolwin.hwnd == NULL) {
		return;
	}
	if (num < NELEMENTS(diskacc)) {
		acc = diskacc + num;
		sub = NULL;
		if (*(acc->counter) == 0) {
			sub = toolwin.sub[acc->idc];
		}
		*(acc->counter) = count;
		if (sub) {
			InvalidateRect(sub, NULL, TRUE);
		}
	}
}

void toolwin_fddaccess(UINT8 drv) {

	if (drv < 2) {
		setdiskacc(drv, 20);
	}
}

void toolwin_hddaccess(UINT8 drv) {

	setdiskacc(2, 10);
}

void toolwin_draw(UINT8 frame) {

const DISKACC	*acc;
const DISKACC	*accterm;
	UINT8		counter;
	HWND		sub;

	if (toolwin.hwnd == NULL) {
		return;
	}
	if (!frame) {
		frame = 1;
	}
	acc = diskacc;
	accterm = acc + NELEMENTS(diskacc);
	while(acc < accterm) {
		counter = *acc->counter;
		if (counter) {
			if (counter <= frame) {
				*(acc->counter) = 0;
				sub = toolwin.sub[acc->idc];
				if (sub) {
					InvalidateRect(sub, NULL, TRUE);
				}
			}
			else {
				*(acc->counter) -= frame;
			}
		}
		acc++;
	}
}


// ----

static const OEMCHAR ini_title[] = OEMTEXT("NP2 tool");

static const PFTBL iniitem[] = {
	PFVAL("WindposX", PFTYPE_SINT32,	&np2tool.posx),
	PFVAL("WindposY", PFTYPE_SINT32,	&np2tool.posy),
	PFVAL("WindType", PFTYPE_BOOL,		&np2tool.type),
	PFSTR("SkinFile", PFTYPE_STR,		np2tool.skin),
	PFSTR("SkinMRU0", PFTYPE_STR,		np2tool.skinmru[0]),
	PFSTR("SkinMRU1", PFTYPE_STR,		np2tool.skinmru[1]),
	PFSTR("SkinMRU2", PFTYPE_STR,		np2tool.skinmru[2]),
	PFSTR("SkinMRU3", PFTYPE_STR,		np2tool.skinmru[3]),
	PFSTR("FD1NAME0", PFTYPE_STR,		np2tool.fdd[0].name[0]),
	PFSTR("FD1NAME1", PFTYPE_STR,		np2tool.fdd[0].name[1]),
	PFSTR("FD1NAME2", PFTYPE_STR,		np2tool.fdd[0].name[2]),
	PFSTR("FD1NAME3", PFTYPE_STR,		np2tool.fdd[0].name[3]),
	PFSTR("FD1NAME4", PFTYPE_STR,		np2tool.fdd[0].name[4]),
	PFSTR("FD1NAME5", PFTYPE_STR,		np2tool.fdd[0].name[5]),
	PFSTR("FD1NAME6", PFTYPE_STR,		np2tool.fdd[0].name[6]),
	PFSTR("FD1NAME7", PFTYPE_STR,		np2tool.fdd[0].name[7]),
	PFSTR("FD2NAME0", PFTYPE_STR,		np2tool.fdd[1].name[0]),
	PFSTR("FD2NAME1", PFTYPE_STR,		np2tool.fdd[1].name[1]),
	PFSTR("FD2NAME2", PFTYPE_STR,		np2tool.fdd[1].name[2]),
	PFSTR("FD2NAME3", PFTYPE_STR,		np2tool.fdd[1].name[3]),
	PFSTR("FD2NAME4", PFTYPE_STR,		np2tool.fdd[1].name[4]),
	PFSTR("FD2NAME5", PFTYPE_STR,		np2tool.fdd[1].name[5]),
	PFSTR("FD2NAME6", PFTYPE_STR,		np2tool.fdd[1].name[6]),
	PFSTR("FD2NAME7", PFTYPE_STR,		np2tool.fdd[1].name[7])};

void toolwin_readini(void) {

	OEMCHAR	path[MAX_PATH];

	ZeroMemory(&np2tool, sizeof(np2tool));
	np2tool.posx = CW_USEDEFAULT;
	np2tool.posy = CW_USEDEFAULT;
	np2tool.type = 1;
	initgetfile(path, NELEMENTS(path));
	ini_read(path, ini_title, iniitem, NELEMENTS(iniitem));
}

void toolwin_writeini(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_write(path, ini_title, iniitem, NELEMENTS(iniitem));
}

