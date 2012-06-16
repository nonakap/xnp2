
enum {
	DCLOCK_WIDTH	= 56,
	DCLOCK_HEIGHT	= 12,
	DCLOCK_YALIGN	= (56 / 8)
};


typedef struct {
	UINT8	*pos;
	UINT16	mask;
	UINT8	rolbit;
	UINT8	reserved;
} DCPOS;

typedef struct {
const UINT8	*fnt;
const DCPOS	*pos;
	UINT8	flm[8];
	UINT8	now[8];
	UINT8	bak[8];
	UINT16	drawing;
	UINT8	clk_x;
	UINT8	_padding;
	UINT8	dat[(DCLOCK_HEIGHT * DCLOCK_YALIGN) + 4];
} _DCLOCK, *DCLOCK;

typedef struct {
	RGB32	pal32[4];
	RGB16	pal16[4];
	UINT32	pal8[4][16];
} DCLOCKPAL;


#ifdef __cplusplus
extern "C" {
#endif

extern	_DCLOCK		dclock;
extern	DCLOCKPAL	dclockpal;

void dclock_init(void);
void dclock_palset(UINT bpp);
void dclock_reset(void);
void dclock_callback(void);
void dclock_redraw(void);
BOOL dclock_disp(void);
void dclock_cntdown(UINT8 value);
void dclock_make(void);
void dclock_out8(void *ptr, UINT yalign);
void dclock_out16(void *ptr, UINT yalign);
void dclock_out24(void *ptr, UINT yalign);
void dclock_out32(void *ptr, UINT yalign);

#ifdef __cplusplus
}
#endif

