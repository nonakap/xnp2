
#ifdef __cplusplus
extern "C" {
#endif

enum {
	SXSIMEDIA_DATA = 0x10,
	SXSIMEDIA_AUDIO = 0x20
};

typedef struct {
	UINT8	type;
	UINT8	track;
	UINT32	pos;
} _CDTRK, *CDTRK;

BRESULT sxsicd_open(SXSIDEV sxsi, const OEMCHAR *fname);

CDTRK sxsicd_gettrk(SXSIDEV sxsi, UINT *tracks);
BRESULT sxsicd_readraw(SXSIDEV sxsi, long pos, void *buf);

#ifdef __cplusplus
}
#endif

