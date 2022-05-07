/**
 * @file	menu.cpp
 * @brief	メニューの宣言およびインターフェイスの定義をします
 */

#include "compiler.h"
#include "resource.h"
#include "menu.h"
#include "np2.h"
#include "scrnmng.h"
#include "sysmng.h"
#include "misc\tstring.h"
#include "misc\WndProc.h"
#include "pccore.h"
#if defined(SUPPORT_WAVEREC)
#include "sound\sound.h"
#endif
#if defined(SUPPORT_S98)
#include "sound\s98.h"
#endif

/**
 * メニュー内の指定された位置に、新しいメニュー項目を挿入します
 * @param[in] hMenu メニューのハンドル
 * @param[in] uItem 識別子または位置
 * @param[in] fByPosition uItem パラメータの意味
 * @param[in] hPopup 追加するメニュー
 * @return 追加した項目数
 */
UINT InsertMenuPopup(HMENU hMenu, UINT uItem, BOOL fByPosition, HMENU hPopup)
{
	int nCount = GetMenuItemCount(hPopup);
	UINT nAdded = 0;
	for (int i = 0; i < nCount; i++)
	{
		TCHAR szString[128];

		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_DATA;
		mii.dwTypeData = szString;
		mii.cch = _countof(szString);
		if (!GetMenuItemInfo(hPopup, i, TRUE, &mii))
		{
			continue;
		}
		if (mii.hSubMenu)
		{
			HMENU hSubMenu = CreatePopupMenu();
			InsertMenuPopup(hSubMenu, 0, TRUE, mii.hSubMenu);
			mii.hSubMenu = hSubMenu;
		}
		if (InsertMenuItem(hMenu, uItem, fByPosition, &mii))
		{
			nAdded++;
			if (fByPosition)
			{
				uItem++;
			}
		}
	}
	return nAdded;
}

/**
 * 新しいメニュー項目を追加します
 * @param[in] hMenu メニューのハンドル
 * @param[in] nMenuID 追加するメニュー ID
 * @return 追加した項目数
 */
UINT AppendMenuResource(HMENU hMenu, UINT nMenuID)
{
	return InsertMenuResource(hMenu, GetMenuItemCount(hMenu), TRUE, nMenuID);
}

/**
 * メニュー内の指定された位置に、新しいメニュー項目を挿入します
 * @param[in] hMenu メニューのハンドル
 * @param[in] uItem 識別子または位置
 * @param[in] fByPosition uItem パラメータの意味
 * @param[in] nMenuID 追加するメニュー ID
 * @return 追加した項目数
 */
UINT InsertMenuResource(HMENU hMenu, UINT uItem, BOOL fByPosition, UINT nMenuID)
{
	HMENU hSubMenu = LoadMenu(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(nMenuID));
	const UINT nAdded = InsertMenuPopup(hMenu, uItem, fByPosition, hSubMenu);
	::DestroyMenu(hSubMenu);
	return nAdded;
}

/**
 * メニューを追加する
 * @param[in] hMenu メニューのハンドル
 * @param[in] uFlags オプション
 * @param[in] uIDNewItem 識別子、メニュー、サブメニューのいずれか
 * @retval 関数が成功すると、0 以外の値が返ります
 */
static BOOL AppendMenuString(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem)
{
	std::tstring rString(LoadTString(static_cast<UINT>(uIDNewItem)));
	if (rString.empty())
	{
		return FALSE;
	}

	if (uFlags & MF_POPUP)
	{
		HMENU hSubMenu = LoadMenu(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(uIDNewItem));
		if (hSubMenu == NULL)
		{
			return FALSE;
		}
		uIDNewItem = reinterpret_cast<UINT_PTR>(hSubMenu);
	}
	return AppendMenu(hMenu, uFlags, uIDNewItem, rString.c_str());
}

/**
 * メニューを挿入する
 * @param[in] hMenu メニューのハンドル
 * @param[in] uItem 挿入するべきメニュー項目の直後に位置するメニュー項目の識別子または位置を指定します
 * @param[in] uFlags オプション
 * @param[in] uIDNewItem 識別子、メニュー、サブメニューのいずれか
 * @retval 関数が成功すると、0 以外の値が返ります
 */
static BOOL InsertMenuString(HMENU hMenu, UINT uItem, UINT uFlags, UINT_PTR uIDNewItem)
{
	std::tstring rString(LoadTString(static_cast<UINT>(uIDNewItem)));
	if (rString.empty())
	{
		return FALSE;
	}

	if (uFlags & MF_POPUP)
	{
		HMENU hSubMenu = LoadMenu(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(uIDNewItem));
		if (hSubMenu == NULL)
		{
			return FALSE;
		}
		uIDNewItem = reinterpret_cast<UINT_PTR>(hSubMenu);
	}
	return InsertMenu(hMenu, uItem, uFlags, uIDNewItem, rString.c_str());
}

/**
 * メニュー オーナーを検索
 * @param[in] hMenu メニュー ハンドル
 * @param[in] uItem メニュー アイテム
 * @param[out] pnPos 見つかった位置
 * @return メニュー ハンドル
 */
HMENU GetMenuOwner(HMENU hMenu, UINT uItem, int* pnPos)
{
	int nCount = GetMenuItemCount(hMenu);
	for (int i = 0; i < nCount; i++)
	{
		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_ID | MIIM_SUBMENU;
		if (GetMenuItemInfo(hMenu, i, TRUE, &mii))
		{
			if (mii.wID == uItem)
			{
				if (pnPos)
				{
					*pnPos = i;
				}
				return hMenu;
			}
			else if (mii.hSubMenu)
			{
				HMENU hFoundMenu = GetMenuOwner(mii.hSubMenu, uItem, pnPos);
				if (hFoundMenu)
				{
					return hFoundMenu;
				}
			}
		}
	}
	return NULL;
}



// ----

/**
 * システム メニュー初期化
 * @param[in] hMenu メニュー ハンドル
 */
void sysmenu_initialize(HMENU hMenu)
{
	UINT nPos = 0;

#if defined(SUPPORT_KEYDISP)
	nPos += InsertMenuResource(hMenu, nPos, TRUE, IDR_SYSKEYDISP);
#endif	// defined(SUPPORT_KEYDISP)

#if defined(SUPPORT_SOFTKBD)
	nPos += InsertMenuResource(hMenu, nPos, TRUE, IDR_SYSSOFTKBD);
#endif	// defined(SUPPORT_KEYDISP)

	nPos += InsertMenuResource(hMenu, nPos, TRUE, IDR_SYS);
	if (np2oscfg.I286SAVE)
	{
#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
		nPos += InsertMenuResource(hMenu, nPos, TRUE, IDR_SYSDEBUG32);
#endif	// defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)

		nPos += InsertMenuResource(hMenu, nPos, TRUE, IDR_SYSDEBUG);
	}
}

/**
 * システム メニュー更新
 * @param[in] hMenu メニュー ハンドル
 */
void sysmenu_update(HMENU hMenu)
{
	CheckMenuItem(hMenu, IDM_TOOLWIN, MF_BYCOMMAND | MFCHECK(np2oscfg.toolwin));
	CheckMenuItem(hMenu, IDM_KEYDISP, MF_BYCOMMAND | MFCHECK(np2oscfg.keydisp));
	CheckMenuItem(hMenu, IDM_SNAPENABLE, MF_BYCOMMAND | MFCHECK(np2oscfg.WINSNAP));

	const UINT8 background = np2oscfg.background ^ 3;
	EnableMenuItem(hMenu, IDM_BGSOUND, (background & 1) ? MF_ENABLED : MF_GRAYED);
	CheckMenuItem(hMenu, IDM_BACKGROUND, MF_BYCOMMAND | MFCHECK(background & 1));
	CheckMenuItem(hMenu, IDM_BGSOUND, MF_BYCOMMAND | MFCHECK(background & 2));

	const int scrnmul = scrnmng_getmultiple();
	CheckMenuItem(hMenu, IDM_SCRNMUL4, MF_BYCOMMAND | MFCHECK(scrnmul == 4));
	CheckMenuItem(hMenu, IDM_SCRNMUL6, MF_BYCOMMAND | MFCHECK(scrnmul == 6));
	CheckMenuItem(hMenu, IDM_SCRNMUL8, MF_BYCOMMAND | MFCHECK(scrnmul == 8));
	CheckMenuItem(hMenu, IDM_SCRNMUL10, MF_BYCOMMAND | MFCHECK(scrnmul == 10));
	CheckMenuItem(hMenu, IDM_SCRNMUL12, MF_BYCOMMAND | MFCHECK(scrnmul == 12));
	CheckMenuItem(hMenu, IDM_SCRNMUL16, MF_BYCOMMAND | MFCHECK(scrnmul == 16));
}



// ----

/**
 * メニュー初期化
 * @param[in] メニュー ハンドル
 */
void xmenu_initialize(HMENU hMenu)
{
	UINT nPos = 1;

#if defined(SUPPORT_STATSAVE)
	if (np2oscfg.statsave)
	{
		nPos += InsertMenuResource(hMenu, nPos, TRUE, IDR_STAT);
	}
#endif

	for (UINT i = 0; i < 4; i++)
	{
		if (np2cfg.fddequip & (1 << i))
		{
			nPos += InsertMenuResource(hMenu, nPos, TRUE, IDR_FDD1MENU + i);
		}
	}

	HMENU hMenuSub = CreatePopupMenu();
	if (hMenuSub)
	{
		UINT nSubPos = 0;
#if defined(SUPPORT_IDEIO)
		nSubPos += InsertMenuResource(hMenuSub, nSubPos, TRUE, IDR_IDEMENU);
#else	// defined(SUPPORT_IDEIO)
		nSubPos += InsertMenuResource(hMenuSub, nSubPos, TRUE, IDR_SASIMENU);
#endif	// defined(SUPPORT_IDEIO)

#if defined(SUPPORT_SCSI)
		if (nSubPos)
		{
			nSubPos += InsertMenu(hMenuSub, nSubPos, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		}
		nSubPos += InsertMenuResource(hMenuSub, nSubPos, TRUE, IDR_SCSIMENU);
#endif	// defined(SUPPORT_SCSI)

		std::tstring rString(LoadTString(IDS_HDD));
		InsertMenu(hMenu, nPos, MF_BYPOSITION | MF_POPUP, reinterpret_cast<INT_PTR>(hMenuSub), rString.c_str());
	}

#if defined(SUPPORT_PX)
	InsertMenuResource(hMenu, IDM_SOUNDORCHESTRA, FALSE, IDR_PXMENU);
#endif	// defined(SUPPORT_PX)

	if (np2oscfg.I286SAVE)
	{
#if defined(SUPPORT_PC9821)
		InsertMenuResource(hMenu, IDM_CALENDAR, FALSE, IDR_CPUSAVE32);
#else	//	defined(SUPPORT_PC9821)
		InsertMenuResource(hMenu, IDM_CALENDAR, FALSE, IDR_CPUSAVE16);
#endif	//	defined(SUPPORT_PC9821)
	}
}

/**
 * メニュー状態を更新する
 * @param[in] hMenu メニュー ハンドル
 */
void xmenu_update(HMENU hMenu)
{
	if (hMenu == NULL)
	{
		return;
	}

	// Screen
	const bool bFullScreen = ((g_scrnmode & SCRNMODE_FULLSCREEN) != 0);
	CheckMenuItem(hMenu, IDM_WINDOW, MF_BYCOMMAND | MFCHECK(!bFullScreen));
	CheckMenuItem(hMenu, IDM_FULLSCREEN, MF_BYCOMMAND | MFCHECK(bFullScreen));
	const UINT8 nRotateMode = g_scrnmode & SCRNMODE_ROTATEMASK;
	CheckMenuItem(hMenu, IDM_ROLNORMAL, MF_BYCOMMAND | MFCHECK(nRotateMode == 0));
	CheckMenuItem(hMenu, IDM_ROLLEFT, MF_BYCOMMAND | MFCHECK(nRotateMode == SCRNMODE_ROTATELEFT));
	CheckMenuItem(hMenu, IDM_ROLRIGHT, MF_BYCOMMAND | MFCHECK(nRotateMode == SCRNMODE_ROTATERIGHT));
	CheckMenuItem(hMenu, IDM_DISPSYNC, MF_BYCOMMAND | MFCHECK(np2cfg.DISPSYNC));
	CheckMenuItem(hMenu, IDM_RASTER, MF_BYCOMMAND | MFCHECK(np2cfg.RASTER));
	CheckMenuItem(hMenu, IDM_NOWAIT, MF_BYCOMMAND | MFCHECK(np2oscfg.NOWAIT));
	const UINT8 DRAW_SKIP = np2oscfg.DRAW_SKIP;
	CheckMenuItem(hMenu, IDM_AUTOFPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 0));
	CheckMenuItem(hMenu, IDM_60FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 1));
	CheckMenuItem(hMenu, IDM_30FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 2));
	CheckMenuItem(hMenu, IDM_20FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 3));
	CheckMenuItem(hMenu, IDM_15FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 4));

	// Device-Keyboard
	const UINT8 KEY_MODE = np2cfg.KEY_MODE;
	CheckMenuItem(hMenu, IDM_KEY, MF_BYCOMMAND | MFCHECK(KEY_MODE == 0));
	CheckMenuItem(hMenu, IDM_JOY1, MF_BYCOMMAND | MFCHECK(KEY_MODE == 1));
	CheckMenuItem(hMenu, IDM_JOY2, MF_BYCOMMAND | MFCHECK(KEY_MODE == 2));
	const UINT8 XSHIFT = np2cfg.XSHIFT;
	CheckMenuItem(hMenu, IDM_XSHIFT, MF_BYCOMMAND | MFCHECK(XSHIFT & 1));
	CheckMenuItem(hMenu, IDM_XCTRL, MF_BYCOMMAND | MFCHECK(XSHIFT & 2));
	CheckMenuItem(hMenu, IDM_XGRPH, MF_BYCOMMAND | MFCHECK(XSHIFT & 4));
	const UINT8 F12COPY = np2oscfg.F12COPY;
	CheckMenuItem(hMenu, IDM_F12MOUSE, MF_BYCOMMAND | MFCHECK(F12COPY == 0));
	CheckMenuItem(hMenu, IDM_F12COPY, MF_BYCOMMAND | MFCHECK(F12COPY == 1));
	CheckMenuItem(hMenu, IDM_F12STOP, MF_BYCOMMAND | MFCHECK(F12COPY == 2));
	CheckMenuItem(hMenu, IDM_F12EQU, MF_BYCOMMAND | MFCHECK(F12COPY == 3));
	CheckMenuItem(hMenu, IDM_F12COMMA, MF_BYCOMMAND | MFCHECK(F12COPY == 4));
	CheckMenuItem(hMenu, IDM_USERKEY1, MF_BYCOMMAND | MFCHECK(F12COPY == 5));
	CheckMenuItem(hMenu, IDM_USERKEY2, MF_BYCOMMAND | MFCHECK(F12COPY == 6));

	// Device-Sound
	const UINT8 BEEP_VOL = np2cfg.BEEP_VOL;
	CheckMenuItem(hMenu, IDM_BEEPOFF, MF_BYCOMMAND | MFCHECK(BEEP_VOL == 0));
	CheckMenuItem(hMenu, IDM_BEEPLOW, MF_BYCOMMAND | MFCHECK(BEEP_VOL == 1));
	CheckMenuItem(hMenu, IDM_BEEPMID, MF_BYCOMMAND | MFCHECK(BEEP_VOL == 2));
	CheckMenuItem(hMenu, IDM_BEEPHIGH, MF_BYCOMMAND | MFCHECK(BEEP_VOL == 3));
	const UINT8 SOUND_SW = np2cfg.SOUND_SW;
	CheckMenuItem(hMenu, IDM_NOSOUND, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x00));
	CheckMenuItem(hMenu, IDM_PC9801_14, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x01));
	CheckMenuItem(hMenu, IDM_PC9801_26K, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x02));
	CheckMenuItem(hMenu, IDM_PC9801_86, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x04));
	CheckMenuItem(hMenu, IDM_PC9801_26_86, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x06));
	CheckMenuItem(hMenu, IDM_PC9801_86_CB, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x14));
	CheckMenuItem(hMenu, IDM_PC9801_118, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x08));
	CheckMenuItem(hMenu, IDM_SPEAKBOARD, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x20));
	CheckMenuItem(hMenu, IDM_SPARKBOARD, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x40));
#if defined(SUPPORT_PX)
	CheckMenuItem(hMenu, IDM_PX1, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x30));
	CheckMenuItem(hMenu, IDM_PX2, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x50));
#endif	// defined(SUPPORT_PX)
	CheckMenuItem(hMenu, IDM_SOUNDORCHESTRA, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x32));
	CheckMenuItem(hMenu, IDM_SOUNDORCHESTRAV, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x82));
	CheckMenuItem(hMenu, IDM_AMD98, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x80));
	CheckMenuItem(hMenu, IDM_JASTSOUND, MF_BYCOMMAND | MFCHECK(np2oscfg.jastsnd));
	CheckMenuItem(hMenu, IDM_SEEKSND, MF_BYCOMMAND | MFCHECK(np2cfg.MOTOR));

	// Device-Memory
	const UINT8 EXTMEM = np2cfg.EXTMEM;
	CheckMenuItem(hMenu, IDM_MEM640, MF_BYCOMMAND | MFCHECK(EXTMEM == 0));
	CheckMenuItem(hMenu, IDM_MEM16, MF_BYCOMMAND | MFCHECK(EXTMEM == 1));
	CheckMenuItem(hMenu, IDM_MEM36, MF_BYCOMMAND | MFCHECK(EXTMEM == 3));
	CheckMenuItem(hMenu, IDM_MEM76, MF_BYCOMMAND | MFCHECK(EXTMEM == 7));
	CheckMenuItem(hMenu, IDM_MEM116, MF_BYCOMMAND | MFCHECK(EXTMEM == 11));
	CheckMenuItem(hMenu, IDM_MEM136, MF_BYCOMMAND | MFCHECK(EXTMEM == 13));

	// Device
	CheckMenuItem(hMenu, IDM_MOUSE, MF_BYCOMMAND | MFCHECK(np2oscfg.MOUSE_SW));

	// Other-ShortcutKey
	const UINT8 shortcut = np2oscfg.shortcut;
	CheckMenuItem(hMenu, IDM_ALTENTER, MF_BYCOMMAND | MFCHECK(shortcut & 1));
	CheckMenuItem(hMenu, IDM_ALTF4, MF_BYCOMMAND | MFCHECK(shortcut & 2));

	// Other
	bool bLogging = false;
#if defined(SUPPORT_S98)
	if (S98_isopened())
	{
		bLogging = true;
	}
#endif
#if defined(SUPPORT_WAVEREC)
	if (sound_isrecording())
	{
		bLogging = true;
	}
#endif
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_S98LOGGING, MFCHECK(bLogging));
	const UINT8 DISPCLK = np2oscfg.DISPCLK;
	CheckMenuItem(hMenu, IDM_DISPCLOCK, MF_BYCOMMAND | MFCHECK(DISPCLK & 1));
	CheckMenuItem(hMenu, IDM_DISPFRAME, MF_BYCOMMAND | MFCHECK(DISPCLK & 2));
	CheckMenuItem(hMenu, IDM_JOYX, MF_BYCOMMAND | MFCHECK(np2cfg.BTN_MODE));
	CheckMenuItem(hMenu, IDM_RAPID, MF_BYCOMMAND | MFCHECK(np2cfg.BTN_RAPID));
	CheckMenuItem(hMenu, IDM_MSRAPID, MF_BYCOMMAND | MFCHECK(np2cfg.MOUSERAPID));
}
