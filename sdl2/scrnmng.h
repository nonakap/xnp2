
typedef struct {
	UINT8	*ptr;
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

void scrnmng_setwidth(int posx, int width);
#define scrnmng_setextend(e)
void scrnmng_setheight(int posy, int height);
const SCRNSURF *scrnmng_surflock(void);
void scrnmng_surfunlock(const SCRNSURF *surf);

#define	scrnmng_isfullscreen()	(0)
#define	scrnmng_haveextend()	(0)
#define	scrnmng_getbpp()		(16)
#define	scrnmng_allflash()		
#define	scrnmng_palchanged()	

RGB16 scrnmng_makepal16(RGB32 pal32);

#ifdef __cplusplus
}
#endif


// ---- for SDL

void scrnmng_initialize(void);
BRESULT scrnmng_create(int width, int height);
void scrnmng_destroy(void);


// ---- for menubase

typedef struct {
	int		width;
	int		height;
	int		bpp;
} SCRNMENU;

BRESULT scrnmng_entermenu(SCRNMENU *smenu);
void scrnmng_leavemenu(void);
void scrnmng_menudraw(const RECT_T *rct);

