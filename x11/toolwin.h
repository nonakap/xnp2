#ifndef	NP2_X11_TOOLWIN_H__
#define	NP2_X11_TOOLWIN_H__

enum {
	SKINMRU_MAX	= 4,
	FDDLIST_DRV	= 2,
	FDDLIST_MAX	= 8
};

typedef struct {
	int	insert;
	UINT	cnt;
	UINT	pos[FDDLIST_MAX];
	char	name[FDDLIST_MAX][MAX_PATH];
} TOOLFDD;

typedef struct {
	int	posx;
	int	posy;
	BOOL	type;
	TOOLFDD	fdd[FDDLIST_DRV];
	char	skin[MAX_PATH];
	char	skinmru[SKINMRU_MAX][MAX_PATH];
} NP2TOOL;

#ifdef __cplusplus
extern "C" {
#endif

extern NP2TOOL np2tool;

#if !defined(SUPPORT_TOOLWINDOW)

#define	toolwin_create()
#define	toolwin_destroy()
#define	toolwin_setfdd(drv, name)
#define	toolwin_fddaccess(drv)
#define	toolwin_hddaccess(drv)
#define	toolwin_draw(frame)		(void)frame
#define	toolwin_readini()
#define	toolwin_writeini()

#else	/* !SUPPORT_TOOLWIN */

void toolwin_create(void);
void toolwin_destroy(void);

void toolwin_setfdd(BYTE drv, const char *name);

void toolwin_fddaccess(BYTE drv);
void toolwin_hddaccess(BYTE drv);

void toolwin_draw(BYTE frame);

void toolwin_readini(void);
void toolwin_writeini(void);

#endif	/* SUPPORT_TOOLWIN */

#ifdef __cplusplus
}
#endif

#endif	/* NP2_X11_TOOLWIN_H__ */
