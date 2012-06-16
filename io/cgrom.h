
typedef struct {
	UINT	code;
	UINT	lr;
	UINT	line;
} _CGROM, *CGROM;

typedef struct {
#if !defined(CGWND_FONTPTR)
	UINT32	low;
	UINT32	high;
#else
	BYTE	*fontlow;
	BYTE	*fonthigh;
#endif
	UINT8	writable;
} _CGWINDOW, *CGWINDOW;


#ifdef __cplusplus
extern "C" {
#endif

void cgrom_reset(const NP2CFG *pConfig);
void cgrom_bind(void);

#ifdef __cplusplus
}
#endif

