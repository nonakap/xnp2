#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"np2class.h"
#include	"pccore.h"


BOOL menu_searchmenu(HMENU hMenu, UINT uID, HMENU *phmenuRet, int *pnPos)
{
	int				nCount;
	int				i;
	MENUITEMINFO	mii;

	nCount = GetMenuItemCount(hMenu);
	for (i=0; i<nCount; i++)
	{
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_ID | MIIM_SUBMENU;
		if (GetMenuItemInfo(hMenu, i, TRUE, &mii))
		{
			if (mii.wID == uID)
			{
				if (phmenuRet)
				{
					*phmenuRet = hMenu;
				}
				if (pnPos)
				{
					*pnPos = i;
				}
				return TRUE;
			}
			else if ((mii.hSubMenu) &&
					(menu_searchmenu(mii.hSubMenu, uID, phmenuRet, pnPos)))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

#if 0
static BOOL searchsubmenu(HMENU hMenu, HMENU hmenuTarget,
												HMENU *phmenuRet, int *pnPos)
{
	int				nCount;
	int				i;
	MENUITEMINFO	mii;

	nCount = GetMenuItemCount(hMenu);
	for (i=0; i<nCount; i++)
	{
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_SUBMENU;
		if ((GetMenuItemInfo(hMenu, i, TRUE, &mii)) && (mii.hSubMenu))
		{
			if (mii.hSubMenu == hmenuTarget)
			{
				if (phmenuRet)
				{
					*phmenuRet = hMenu;
				}
				if (pnPos)
				{
					*pnPos = i;
				}
				return TRUE;
			}
			if (searchsubmenu(mii.hSubMenu, hmenuTarget, phmenuRet, pnPos))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif	// 0

// ‚±‚ê‚Á‚ÄAPI‚ ‚é‚Ì‚©H
int menu_addmenu(HMENU hMenu, int nPos, HMENU hmenuAdd, BOOL bSeparator)
{
	int				nCount;
	int				nAdded;
	int				i;
	MENUITEMINFO	mii;
	TCHAR			szString[128];
	HMENU			hmenuSub;

	if (nPos < 0)
	{
		nPos = GetMenuItemCount(hMenu);
	}
	nCount = GetMenuItemCount(hmenuAdd);
	nAdded = 0;
	for (i=0; i<nCount; i++)
	{
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU |
																	MIIM_DATA;
		mii.dwTypeData = szString;
		mii.cch = NELEMENTS(szString);
		if (GetMenuItemInfo(hmenuAdd, i, TRUE, &mii))
		{
			if (mii.hSubMenu)
			{
				hmenuSub = CreatePopupMenu();
				(void)menu_addmenu(hmenuSub, 0, mii.hSubMenu, FALSE);
				mii.hSubMenu = hmenuSub;
			}
			if (bSeparator)
			{
				bSeparator = FALSE;
				InsertMenu(hMenu, nPos + nAdded, MF_BYPOSITION | MF_SEPARATOR,
																	0, NULL);
				nAdded++;
			}
			InsertMenuItem(hMenu, nPos + nAdded, TRUE, &mii);
			nAdded++;
		}
	}
	return nAdded;
}

int menu_addmenures(HMENU hMenu, int nPos, UINT uID, BOOL bSeparator)
{
	int		nCount;
	HMENU	hmenuAdd;

	nCount = 0;
	hmenuAdd = LoadMenu(g_hInstance, MAKEINTRESOURCE(uID));
	if (hmenuAdd)
	{
		nCount = menu_addmenu(hMenu, nPos, hmenuAdd, bSeparator);
		DestroyMenu(hmenuAdd);
	}
	return nCount;
}

int menu_addmenubyid(HMENU hMenu, UINT uByID, UINT uID)
{
	int		nCount;
	HMENU	hmenuSub;
	int		nSubPos;

	nCount = 0;
	if (menu_searchmenu(hMenu, uByID, &hmenuSub, &nSubPos))
	{
		nCount = menu_addmenures(hmenuSub, nSubPos + 1, uID, FALSE);
	}
	return nCount;
}

BOOL menu_insertmenures(HMENU hMenu, int nPosition, UINT uFlags,
											UINT_PTR uIDNewItem, UINT uID)
{
	BOOL	bResult;
	TCHAR	szString[128];

	bResult = FALSE;
	if (loadstringresource(uID, szString, NELEMENTS(szString)))
	{
		bResult = InsertMenu(hMenu, nPosition, uFlags, uIDNewItem, szString);
	}
	return bResult;
}

void menu_addmenubar(HMENU popup, HMENU menubar)
{
	(void)menu_addmenu(popup, 0, menubar, FALSE);
}


// ----

void sysmenu_initialize(void)
{
	HMENU	hMenu;
	UINT	uPos;

	hMenu = GetSystemMenu(g_hWndMain, FALSE);
	uPos = 0;

#if defined(SUPPORT_KEYDISP)
	uPos += menu_addmenures(hMenu, uPos, IDR_SYSKEYDISP, FALSE);
#endif
#if defined(SUPPORT_SOFTKBD)
	uPos += menu_addmenures(hMenu, uPos, IDR_SYSSOFTKBD, FALSE);
#endif

	uPos += menu_addmenures(hMenu, uPos, IDR_SYS, FALSE);
	if (np2oscfg.I286SAVE)
	{
#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
		uPos += menu_addmenures(hMenu, uPos, IDR_SYSDEBUG32, FALSE);
#endif
		uPos += menu_addmenures(hMenu, uPos, IDR_SYSDEBUG, FALSE);
	}
}

void sysmenu_settoolwin(UINT8 value) {

	value &= 1;
	np2oscfg.toolwin = value;
	CheckMenuItem(GetSystemMenu(g_hWndMain, FALSE),
											IDM_TOOLWIN, MFCHECK(value));
}

void sysmenu_setkeydisp(UINT8 value) {

	value &= 1;
	np2oscfg.keydisp = value;
	CheckMenuItem(GetSystemMenu(g_hWndMain, FALSE),
											IDM_KEYDISP, MFCHECK(value));
}

void sysmenu_setwinsnap(UINT8 value) {

	value &= 1;
	np2oscfg.WINSNAP = value;
	CheckMenuItem(GetSystemMenu(g_hWndMain, FALSE),
											IDM_SNAPENABLE, MFCHECK(value));
}

void sysmenu_setbackground(UINT8 value) {

	HMENU	hmenu;

	np2oscfg.background &= 2;
	np2oscfg.background |= (value & 1);
	hmenu = GetSystemMenu(g_hWndMain, FALSE);
	if (value & 1) {
		CheckMenuItem(hmenu, IDM_BACKGROUND, MF_UNCHECKED);
		EnableMenuItem(hmenu, IDM_BGSOUND, MF_GRAYED);
	}
	else {
		CheckMenuItem(hmenu, IDM_BACKGROUND, MF_CHECKED);
		EnableMenuItem(hmenu, IDM_BGSOUND, MF_ENABLED);
	}
}

void sysmenu_setbgsound(UINT8 value) {

	np2oscfg.background &= 1;
	np2oscfg.background |= (value & 2);
	CheckMenuItem(GetSystemMenu(g_hWndMain, FALSE),
									IDM_BGSOUND, MFCHECK((value & 2) ^ 2));
}

void sysmenu_setscrnmul(UINT8 value) {

	HMENU	hmenu;

//	np2cfg.scrnmul = value;
	hmenu = GetSystemMenu(g_hWndMain, FALSE);
	CheckMenuItem(hmenu, IDM_SCRNMUL4, MFCHECK(value == 4));
	CheckMenuItem(hmenu, IDM_SCRNMUL6, MFCHECK(value == 6));
	CheckMenuItem(hmenu, IDM_SCRNMUL8, MFCHECK(value == 8));
	CheckMenuItem(hmenu, IDM_SCRNMUL10, MFCHECK(value == 10));
	CheckMenuItem(hmenu, IDM_SCRNMUL12, MFCHECK(value == 12));
	CheckMenuItem(hmenu, IDM_SCRNMUL16, MFCHECK(value == 16));
}


// ----

void xmenu_initialize(void)
{
	HMENU	hMenu;
	int		nPos;
	HMENU	hmenuSub;
	UINT	i;
	int		nSubPos;

	hMenu = np2class_gethmenu(g_hWndMain);

	nPos = 1;
#if defined(SUPPORT_STATSAVE)
	if (np2oscfg.statsave)
	{
		nPos += menu_addmenures(hMenu, nPos, IDR_STAT, FALSE);
	}
#endif

	for (i=0; i<4; i++)
	{
		if (np2cfg.fddequip & (1 << i))
		{
			nPos += menu_addmenures(hMenu, nPos, IDR_FDD1MENU + i, FALSE);
		}
	}

	hmenuSub = CreatePopupMenu();
	if (hmenuSub)
	{
		nSubPos = 0;
#if defined(SUPPORT_IDEIO)
		nSubPos += menu_addmenures(hmenuSub, nSubPos, IDR_IDEMENU, FALSE);
#else
		nSubPos += menu_addmenures(hmenuSub, nSubPos, IDR_SASIMENU, FALSE);
#endif
#if defined(SUPPORT_SCSI)
		nSubPos += menu_addmenures(hmenuSub, nSubPos, IDR_SCSIMENU, TRUE);
#endif
		menu_insertmenures(hMenu, nPos, MF_BYPOSITION | MF_POPUP,
												(UINT_PTR)hmenuSub, IDS_HDD);
	}

#if defined(SUPPORT_PX)
	(void)menu_addmenubyid(hMenu, IDM_SPARKBOARD, IDR_PXMENU);
#endif

#if defined(SUPPORT_WAVEREC)
	(void)menu_addmenubyid(hMenu, IDM_S98LOGGING, IDR_WAVEREC);
#endif

	if (np2oscfg.I286SAVE)
	{
#if defined(SUPPORT_PC9821)
		(void)menu_addmenubyid(hMenu, IDM_SSTP, IDR_CPUSAVE32);
#else	//	defined(SUPPORT_PC9821)
		(void)menu_addmenubyid(hMenu, IDM_SSTP, IDR_CPUSAVE16);
#endif	//	defined(SUPPORT_PC9821)
	}
}

void xmenu_disablewindow(void) {

	HMENU	hmenu;

	hmenu = np2class_gethmenu(g_hWndMain);
	EnableMenuItem(hmenu, IDM_WINDOW, MF_GRAYED);
	EnableMenuItem(hmenu, IDM_FULLSCREEN, MF_GRAYED);
}

void xmenu_setroltate(UINT8 value) {

	HMENU	hmenu;

	hmenu = np2class_gethmenu(g_hWndMain);
	CheckMenuItem(hmenu, IDM_ROLNORMAL, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_ROLLEFT, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_ROLRIGHT, MFCHECK(value == 2));
}

void xmenu_setdispmode(UINT8 value) {

	value &= 1;
	np2cfg.DISPSYNC = value;
	CheckMenuItem(np2class_gethmenu(g_hWndMain), IDM_DISPSYNC, MFCHECK(value));
}

void xmenu_setraster(UINT8 value) {

	value &= 1;
	np2cfg.RASTER = value;
	CheckMenuItem(np2class_gethmenu(g_hWndMain), IDM_RASTER, MFCHECK(value));
}

void xmenu_setwaitflg(UINT8 value) {

	value &= 1;
	np2oscfg.NOWAIT = value;
	CheckMenuItem(np2class_gethmenu(g_hWndMain), IDM_NOWAIT, MFCHECK(value));
}

void xmenu_setframe(UINT8 value) {

	HMENU	hmenu;

	np2oscfg.DRAW_SKIP = value;
	hmenu = np2class_gethmenu(g_hWndMain);
	CheckMenuItem(hmenu, IDM_AUTOFPS, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_60FPS, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_30FPS, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_20FPS, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_15FPS, MFCHECK(value == 4));
}

void xmenu_setkey(UINT8 value) {

	HMENU	hmenu;

	if (value >= 4) {
		value = 0;
	}
	np2cfg.KEY_MODE = value;
	hmenu = np2class_gethmenu(g_hWndMain);
	CheckMenuItem(hmenu, IDM_KEY, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_JOY1, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_JOY2, MFCHECK(value == 2));
}

void xmenu_setxshift(UINT8 value) {

	HMENU	hmenu;

	np2cfg.XSHIFT = value;
	hmenu = np2class_gethmenu(g_hWndMain);
	CheckMenuItem(hmenu, IDM_XSHIFT, MFCHECK(value & 1));
	CheckMenuItem(hmenu, IDM_XCTRL, MFCHECK(value & 2));
	CheckMenuItem(hmenu, IDM_XGRPH, MFCHECK(value & 4));
}

void xmenu_setf12copy(UINT8 value) {

	HMENU	hmenu;

	if (value > 6) {
		value = 0;
	}
	np2oscfg.F12COPY = value;
	hmenu = np2class_gethmenu(g_hWndMain);
	CheckMenuItem(hmenu, IDM_F12MOUSE, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_F12COPY, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_F12STOP, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_F12EQU, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_F12COMMA, MFCHECK(value == 4));
	CheckMenuItem(hmenu, IDM_USERKEY1, MFCHECK(value == 5));
	CheckMenuItem(hmenu, IDM_USERKEY2, MFCHECK(value == 6));
}

void xmenu_setbeepvol(UINT8 value) {

	HMENU	hmenu;

	value &= 3;
	np2cfg.BEEP_VOL = value;
	hmenu = np2class_gethmenu(g_hWndMain);
	CheckMenuItem(hmenu, IDM_BEEPOFF, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_BEEPLOW, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_BEEPMID, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_BEEPHIGH, MFCHECK(value == 3));
}

void xmenu_setsound(UINT8 value) {

	HMENU	hmenu;

	sysmng_update(SYS_UPDATESBOARD);
	np2cfg.SOUND_SW = value;
	hmenu = np2class_gethmenu(g_hWndMain);
	CheckMenuItem(hmenu, IDM_NOSOUND, MFCHECK(value == 0x00));
	CheckMenuItem(hmenu, IDM_PC9801_14, MFCHECK(value == 0x01));
	CheckMenuItem(hmenu, IDM_PC9801_26K, MFCHECK(value == 0x02));
	CheckMenuItem(hmenu, IDM_PC9801_86, MFCHECK(value == 0x04));
	CheckMenuItem(hmenu, IDM_PC9801_26_86, MFCHECK(value == 0x06));
	CheckMenuItem(hmenu, IDM_PC9801_86_CB, MFCHECK(value == 0x14));
	CheckMenuItem(hmenu, IDM_PC9801_118, MFCHECK(value == 0x08));
	CheckMenuItem(hmenu, IDM_SPEAKBOARD, MFCHECK(value == 0x20));
	CheckMenuItem(hmenu, IDM_SPARKBOARD, MFCHECK(value == 0x40));
	CheckMenuItem(hmenu, IDM_AMD98, MFCHECK(value == 0x80));
#if defined(SUPPORT_PX)
	CheckMenuItem(hmenu, IDM_PX1, MFCHECK(value == 0x30));
	CheckMenuItem(hmenu, IDM_PX2, MFCHECK(value == 0x50));
#endif	// defined(SUPPORT_PX)
}

void xmenu_setjastsound(UINT8 value) {

	value &= 1;
	np2oscfg.jastsnd = value;
	CheckMenuItem(np2class_gethmenu(g_hWndMain), IDM_JASTSOUND, MFCHECK(value));
}

void xmenu_setmotorflg(UINT8 value) {

	value &= 1;
	np2cfg.MOTOR = value;
	CheckMenuItem(np2class_gethmenu(g_hWndMain), IDM_SEEKSND, MFCHECK(value));
}

void xmenu_setextmem(UINT8 value) {

	HMENU	hmenu;

	sysmng_update(SYS_UPDATEMEMORY);
	np2cfg.EXTMEM = value;
	hmenu = np2class_gethmenu(g_hWndMain);
	CheckMenuItem(hmenu, IDM_MEM640, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_MEM16, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_MEM36, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_MEM76, MFCHECK(value == 7));
	CheckMenuItem(hmenu, IDM_MEM116, MFCHECK(value == 11));
	CheckMenuItem(hmenu, IDM_MEM136, MFCHECK(value == 13));
}

void xmenu_setmouse(UINT8 value) {

	value &= 1;
	np2oscfg.MOUSE_SW = value;
	CheckMenuItem(np2class_gethmenu(g_hWndMain), IDM_MOUSE, MFCHECK(value));
}

#if defined(SUPPORT_S98)
void xmenu_sets98logging(UINT8 value) {

	CheckMenuItem(np2class_gethmenu(g_hWndMain),
											IDM_S98LOGGING, MFCHECK(value));
}
#endif

#if defined(SUPPORT_WAVEREC)
void xmenu_setwaverec(UINT8 value) {

	CheckMenuItem(np2class_gethmenu(g_hWndMain),
											IDM_WAVEREC, MFCHECK(value));
}
#endif

void xmenu_setshortcut(UINT8 value) {

	HMENU	hmenu;

	np2oscfg.shortcut = value;
	hmenu = np2class_gethmenu(g_hWndMain);
	CheckMenuItem(hmenu, IDM_ALTENTER, MFCHECK(value & 1));
	CheckMenuItem(hmenu, IDM_ALTF4, MFCHECK(value & 2));
}

void xmenu_setdispclk(UINT8 value) {

	HMENU	hmenu;

	value &= 3;
	np2oscfg.DISPCLK = value;
	hmenu = np2class_gethmenu(g_hWndMain);
	CheckMenuItem(hmenu, IDM_DISPCLOCK, MFCHECK(value & 1));
	CheckMenuItem(hmenu, IDM_DISPFRAME, MFCHECK(value & 2));
	sysmng_workclockrenewal();
	sysmng_updatecaption(3);
}

void xmenu_setbtnmode(UINT8 value) {

	value &= 1;
	np2cfg.BTN_MODE = value;
	CheckMenuItem(np2class_gethmenu(g_hWndMain), IDM_JOYX, MFCHECK(value));
}

void xmenu_setbtnrapid(UINT8 value) {

	value &= 1;
	np2cfg.BTN_RAPID = value;
	CheckMenuItem(np2class_gethmenu(g_hWndMain), IDM_RAPID, MFCHECK(value));
}

void xmenu_setmsrapid(UINT8 value) {

	value &= 1;
	np2cfg.MOUSERAPID = value;
	CheckMenuItem(np2class_gethmenu(g_hWndMain), IDM_MSRAPID, MFCHECK(value));
}

void xmenu_setsstp(UINT8 value) {

	value &= 1;
	np2oscfg.sstp = value;
	CheckMenuItem(np2class_gethmenu(g_hWndMain), IDM_SSTP, MFCHECK(value));
}

