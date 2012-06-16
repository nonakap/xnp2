
enum {
	SDL_SWSURFACE	= 0x00000000,
	SDL_HWSURFACE	= 0x00000001,
//	SDL_OPENGL      = 0x00000002,
//	SDL_ASYNCBLIT	= 0x00000004,
	SDL_ANYFORMAT	= 0x10000000,
//	SDL_HWPALETTE	= 0x20000000,
	SDL_DOUBLEBUF	= 0x40000000,
	SDL_FULLSCREEN	= 0x80000000
};

typedef struct {
	BYTE	r;
	BYTE	g;
	BYTE	b;
	BYTE	unused;
} SDL_Color;

typedef struct {
	int			ncolors;
	SDL_Color	*colors;
} SDL_Palette;

typedef struct SDL_PixelFormat {
	SDL_Palette	*palette;
	BYTE		BitsPerPixel;
	BYTE		BytesPerPixel;
//	BYTE		Rloss;
//	BYTE		Gloss;
//	BYTE		Bloss;
//	BYTE		Aloss;
	BYTE		Rshift;
	BYTE		Gshift;
	BYTE		Bshift;
	BYTE		Ashift;
	DWORD		Rmask;
	DWORD		Gmask;
	DWORD		Bmask;
	DWORD		Amask;

	// RGB color key information
//	DWORD		colorkey;

	// Alpha value information (per-surface alpha)
//	BYTE		alpha;
} SDL_PixelFormat;

typedef struct SDL_Surface {
//	Uint32	flags;
	SDL_PixelFormat *format;
	int		w;
	int		h;
	WORD	pitch;
	void	*pixels;
//	int		offset;

	// Hardware-specific surface info
//	struct private_hwdata *hwdata;

	// clipping information
//	SDL_Rect	clip_rect;
//	DWORD		unused1;

	// Allow recursive locks
//	DWORD		locked;

	// info for fast blit mapping to other surfaces
//	struct SDL_BlitMap *map;

	// format version, bumped at every change to invalidate blit maps
//	unsigned int format_version;

	// Reference count -- used when freeing surface
//	int		refcount;
} SDL_Surface;

typedef struct {
//	Uint32 hw_available :1;	/* Flag: Can you create hardware surfaces? */
//	Uint32 wm_available :1;	/* Flag: Can you talk to a window manager? */
//	Uint32 UnusedBits1  :6;
//	Uint32 UnusedBits2  :1;
//	Uint32 blit_hw      :1;	/* Flag: Accelerated blits HW --> HW */
//	Uint32 blit_hw_CC   :1;	/* Flag: Accelerated blits with Colorkey */
//	Uint32 blit_hw_A    :1;	/* Flag: Accelerated blits with Alpha */
//	Uint32 blit_sw      :1;	/* Flag: Accelerated blits SW --> HW */
//	Uint32 blit_sw_CC   :1;	/* Flag: Accelerated blits with Colorkey */
//	Uint32 blit_sw_A    :1;	/* Flag: Accelerated blits with Alpha */
//	Uint32 blit_fill    :1;	/* Flag: Accelerated color fill */
//	Uint32 UnusedBits3  :16;
//	Uint32 video_mem;	/* The total amount of video memory (in K) */
	SDL_PixelFormat *vfmt;	/* Value: The format of the video surface */
} SDL_VideoInfo;


#ifdef __cplusplus
extern "C" {
#endif

SDL_Surface *SDL_CreateRGBSurface(DWORD flags, int width, int height,
			int depth, DWORD Rmask, DWORD Gmask, DWORD Bmask, DWORD Amask);
void SDL_FreeSurface(SDL_Surface *surface);

void SDL_WM_SetCaption(const char *title, const char *icon);
const SDL_VideoInfo *SDL_GetVideoInfo(void);
char *SDL_VideoDriverName(char *namebuf, int maxlen);
SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, DWORD flags);
SDL_Surface *SDL_GetVideoSurface(void);
int SDL_LockSurface(SDL_Surface *surface);
void SDL_UnlockSurface(SDL_Surface *surface);
int SDL_Flip(SDL_Surface *screen);

#ifdef __cplusplus
}
#endif

