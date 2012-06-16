
typedef struct {
	BYTE	*ptr;
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	UINT	bpp;
	int		extend;
} SCRNSURF;


#ifdef __cplusplus
extern "C" {
#endif

void scrnmng_initialize(void);
BOOL scrnmng_create(HWND hWnd, LONG width, LONG height);
void scrnmng_destroy(void);

void scrnmng_setwidth(int posx, int width);
#define scrnmng_setextend(e)
void scrnmng_setheight(int posy, int height);
const SCRNSURF *scrnmng_surflock(void);
void scrnmng_surfunlock(const SCRNSURF *surf);

#define	scrnmng_haveextend()	(0)
#define	scrnmng_getbpp()		(16)
#define	scrnmng_palchanged()	
#define scrnmng_update()		

#if defined(SUPPORT_SOFTKBD)
void scrnmng_allflash(void);
#else
#define scrnmng_allflash()
#endif

RGB16 scrnmng_makepal16(RGB32 pal32);


// ---- for wince

void scrnmng_enable(BOOL enable);
BOOL scrnmng_mousepos(LPARAM *lp);
void scrnmng_clear(BOOL logo);
void scrnmng_keybinds(void);

#if defined(SUPPORT_SOFTKBD)
BOOL scrnmng_kbdpos(LPARAM *lp);
BOOL scrnmng_ismenu(LPARAM lp);
#else
#define scrnmng_kbdpos(lp)		(FAILURE)
#define	scrnmng_ismenu(lp)		(TRUE)
#endif


// ---- for menubase

typedef struct {
	int		width;
	int		height;
	int		bpp;
} SCRNMENU;

BOOL scrnmng_entermenu(SCRNMENU *smenu);
void scrnmng_leavemenu(void);
void scrnmng_menudraw(const RECT_T *rct);

#ifdef __cplusplus
}
#endif

