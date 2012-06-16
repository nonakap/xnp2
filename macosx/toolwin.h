enum {
	SKINMRU_MAX			= 4,
	FDDLIST_DRV			= 2,
	FDDLIST_MAX			= 8
};

typedef struct {
	int		insert;
	UINT	cnt;
	UINT	pos[FDDLIST_MAX];
	char	name[FDDLIST_MAX][MAX_PATH];
} TOOLFDD;

typedef struct {
	int		posx;
	int		posy;
	TOOLFDD	fdd[FDDLIST_DRV];
	char	skin[MAX_PATH];
	char	skinmru[SKINMRU_MAX][MAX_PATH];
} NP2TOOL;

extern	NP2TOOL		np2tool;

void toolwin_open(void);
void toolwin_close(void);

void toolwin_movingstart(void);
void toolwin_movingend(void);
void toolwin_setfdd(BYTE drv, const char *name);

#ifdef __cplusplus
extern "C" {
#endif
void toolwin_fddaccess(BYTE drv);
void toolwin_hddaccess(BYTE drv);
#ifdef __cplusplus
}
#endif
void toolwin_draw(BYTE frame);

void toolwin_readini(void);
void toolwin_writeini(void);

OSErr setDropFile(FSSpec spec, int drv);

