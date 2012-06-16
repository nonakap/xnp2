
void sysmenu_initialize(void);
void sysmenu_settoolwin(UINT8 value);
void sysmenu_setkeydisp(UINT8 value);
void sysmenu_setwinsnap(UINT8 value);
void sysmenu_setbackground(UINT8 value);
void sysmenu_setbgsound(UINT8 value);
void sysmenu_setscrnmul(UINT8 value);

#define	MFCHECK(a) ((a) ? MF_CHECKED : MF_UNCHECKED)

BOOL menu_searchmenu(HMENU hMenu, UINT uID, HMENU *phmenuRet, int *pnPos);
int menu_addmenu(HMENU hMenu, int nPos, HMENU hmenuAdd, BOOL bSeparator);
int menu_addmenures(HMENU hMenu, int nPos, UINT uID, BOOL bSeparator);
int menu_addmenubyid(HMENU hMenu, UINT uByID, UINT uID);
BOOL menu_insertmenures(HMENU hMenu, int nPosition, UINT uFlags,
											UINT_PTR uIDNewItem, UINT uID);
void menu_addmenubar(HMENU popup, HMENU menubar);

void xmenu_initialize(void);
void xmenu_disablewindow(void);
void xmenu_setroltate(UINT8 value);
void xmenu_setdispmode(UINT8 value);
void xmenu_setraster(UINT8 value);
void xmenu_setwaitflg(UINT8 value);
void xmenu_setframe(UINT8 value);
void xmenu_setkey(UINT8 value);
void xmenu_setxshift(UINT8 value);
void xmenu_setf12copy(UINT8 value);
void xmenu_setbeepvol(UINT8 value);
void xmenu_setsound(UINT8 value);
void xmenu_setjastsound(UINT8 value);
void xmenu_setmotorflg(UINT8 value);
void xmenu_setextmem(UINT8 value);
void xmenu_setmouse(UINT8 value);
void xmenu_sets98logging(UINT8 value);
void xmenu_setwaverec(UINT8 value);
void xmenu_setshortcut(UINT8 value);
void xmenu_setdispclk(UINT8 value);
void xmenu_setbtnmode(UINT8 value);
void xmenu_setbtnrapid(UINT8 value);
void xmenu_setmsrapid(UINT8 value);
void xmenu_setsstp(UINT8 value);

