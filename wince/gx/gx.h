
enum {
	GX_FULLSCREEN		= 0x01,
	GX_NORMALKEYS		= 0x02,
};

enum {
	kfLandscape			= 0x008,
	kfPalette			= 0x010,
	kfDirect			= 0x020,
	kfDirect555			= 0x040,
	kfDirect565			= 0x080,
	kfDirect888			= 0x100,
	kfDirect444			= 0x200,
	kfDirectInverted	= 0x400
};

struct GXDisplayProperties {
	DWORD	cxWidth;
	DWORD	cyHeight;
	long	cbxPitch;
	long	cbyPitch;
	long	cBPP;
	DWORD	ffFormat;
};

struct GXKeyList {
	short	vkUp;
	POINT	ptUp;
	short	vkDown;
	POINT	ptDown;
	short	vkLeft;
	POINT	ptLeft;
	short	vkRight;
	POINT	ptRight;
	short	vkA;
	POINT	ptA;
	short	vkB;
	POINT	ptB;
	short	vkC;
	POINT	ptC;
	short	vkStart;
	POINT	ptStart;
};


int GXSetParam(GXDisplayProperties *gxdp);

int GXOpenDisplay(HWND hWnd, DWORD dwFlags);
int GXCloseDisplay(void);
void *GXBeginDraw(void);
int GXEndDraw(void);
GXDisplayProperties GXGetDisplayProperties(void);
int GXSuspend(void);
int GXResume(void);

int GXOpenInput(void);
int GXCloseInput(void);
GXKeyList GXGetDefaultKeys(int iOptions);

