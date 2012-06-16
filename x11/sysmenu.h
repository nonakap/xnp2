#ifndef	NP2_X11_SYSMENU_H__
#define	NP2_X11_SYSMENU_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	MENUTYPE_NORMAL	= 0
};

#if defined(USE_SYSMENU)

BOOL sysmenu_create(void);
void sysmenu_destroy(void);
BOOL sysmenu_menuopen(UINT menutype, int x, int y);

#else

#define sysmenu_create()	SUCCESS
#define	sysmenu_destroy()
#define	sysmenu_menuopen(menutype, x, y)

#endif

#ifdef __cplusplus
}
#endif

#endif	/* NP2_X11_SYSMENU_H__ */
