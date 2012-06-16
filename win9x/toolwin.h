
enum {
	SKINMRU_MAX			= 4,
	FDDLIST_DRV			= 2,
	FDDLIST_MAX			= 8
};

typedef struct {
	int		insert;
	UINT	cnt;
	UINT	pos[FDDLIST_MAX];
	OEMCHAR	name[FDDLIST_MAX][MAX_PATH];
} TOOLFDD;

typedef struct {
	int		posx;
	int		posy;
	BOOL	type;
	TOOLFDD	fdd[FDDLIST_DRV];
	OEMCHAR	skin[MAX_PATH];
	OEMCHAR	skinmru[SKINMRU_MAX][MAX_PATH];
} NP2TOOL;


extern	NP2TOOL		np2tool;

BOOL toolwin_initapp(HINSTANCE hInstance);
void toolwin_create(HINSTANCE hInstance);
void toolwin_destroy(void);
HWND toolwin_gethwnd(void);

void toolwin_setfdd(UINT8 drv, const OEMCHAR *name);

#ifdef __cplusplus
extern "C" {
#endif
void toolwin_fddaccess(UINT8 drv);
void toolwin_hddaccess(UINT8 drv);
#ifdef __cplusplus
}
#endif
void toolwin_draw(UINT8 frame);

void toolwin_readini(void);
void toolwin_writeini(void);

