
#ifdef __cplusplus
extern "C" {
#endif

// extern	int		diskdrv_delay[4];
// extern	OEMCHAR	diskdrv_fname[4][MAX_PATH];

void diskdrv_setsxsi(REG8 drv, const OEMCHAR *fname);
OEMCHAR *diskdrv_getsxsi(REG8 drv);
void diskdrv_hddbind(void);

void diskdrv_readyfddex(REG8 drv, const OEMCHAR *fname,
												UINT ftype, int readonly);
void diskdrv_setfddex(REG8 drv, const OEMCHAR *fname,
												UINT ftype, int readonly);
void diskdrv_callback(void);

#ifdef __cplusplus
}
#endif


// ---- macro

#define diskdrv_readyfdd(d, f, r)	diskdrv_readyfddex(d, f, FTYPE_NONE, r)
#define diskdrv_setfdd(d, f, r)		diskdrv_setfddex(d, f, FTYPE_NONE, r)


// ---- old interface

#define	diskdrv_sethdd(d, f)		diskdrv_setsxsi(d, f)

