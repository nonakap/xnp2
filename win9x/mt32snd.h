
#if defined(MT32SOUND_DLL)

typedef struct {
	void	(*shortmsg)(UINT32 msg);
	void	(*longmsg)(const UINT8 *ptr, UINT32 leng);
	UINT	(*mix32)(SINT32 *buff, UINT leng);
} _MT32HDL, *MT32HDL;

#ifdef __cplusplus
extern "C" {
#endif

BRESULT mt32sound_initialize(void);
void mt32sound_deinitialize(void);
BOOL mt32sound_isenable(void);
void mt32sound_setrate(UINT rate);

BRESULT mt32sound_open(void);
void mt32sound_close(void);
void mt32sound_shortmsg(UINT32 msg);
void mt32sound_longmsg(const UINT8 *ptr, UINT32 leng);
UINT mt32sound_mix32(SINT32 *buff, UINT leng);

#ifdef __cplusplus
}
#endif

#endif

