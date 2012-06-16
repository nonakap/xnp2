#include	"compiler.h"
#include	"SDL_ttf.h"


#pragma pack(push, 1)
typedef struct {
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD				bmiColors[256];
} BMPINFO;
#pragma pack(pop)


typedef struct {
	int		ptsize;
	int		bmpalign;
	HDC		hdcimage;
	HBITMAP	hBitmap;
	BYTE	*image;
	HFONT	hfont;
	RECT	rect;
} _TTFFNT, *TTFFNT;

static const TCHAR deffontface[] = "ÇlÇr ÉSÉVÉbÉN";


int TTF_Init(void) {

	return(0);
}

TTF_Font *TTF_OpenFont(const char *file, int ptsize) {

	TTFFNT	ret;
	BMPINFO	bi;
	int		i;
	HDC		hdc;

	if (ptsize < 0) {
		ptsize *= -1;
	}
	if (ptsize < 6) {
		ptsize = 6;
	}
	else if (ptsize > 128) {
		ptsize = 128;
	}
	ret = (TTFFNT)_MALLOC(sizeof(_TTFFNT), "TTF_Font");
	if (ret == NULL) {
		return(NULL);
	}
	ZeroMemory(ret, sizeof(_TTFFNT));
	ret->ptsize = ptsize;
	ret->bmpalign = ((ptsize + 31) / 8) & (~3);

	ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = ptsize;
	bi.bmiHeader.biHeight = 0 - ptsize;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 1;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = ret->bmpalign * ptsize;
	bi.bmiHeader.biClrUsed = 2;
	bi.bmiHeader.biClrImportant = 2;
	for (i=0; i<2; i++) {
		bi.bmiColors[i].rgbRed = (i ^ 1) - 1;
		bi.bmiColors[i].rgbGreen = (i ^ 1) - 1;
		bi.bmiColors[i].rgbBlue = (i ^ 1) - 1;
		bi.bmiColors[i].rgbReserved = PC_RESERVED;
	}

    hdc = GetDC(NULL);
	ret->hBitmap = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
											(void **)&ret->image, NULL, 0);
	ret->hdcimage = CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);
	ret->hBitmap = (HBITMAP)SelectObject(ret->hdcimage, ret->hBitmap);
	SetDIBColorTable(ret->hdcimage, 0, 2, bi.bmiColors);
	ret->hfont = CreateFont(ptsize, 0,
						FW_DONTCARE, FW_DONTCARE, FW_REGULAR,
						FALSE, FALSE, FALSE, SHIFTJIS_CHARSET,
						OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						NONANTIALIASED_QUALITY, FIXED_PITCH, deffontface);
	ret->hfont = (HFONT)SelectObject(ret->hdcimage, ret->hfont);
	SetTextColor(ret->hdcimage, RGB(255, 255, 255));
	SetBkColor(ret->hdcimage, RGB(0, 0, 0));
	SetRect(&ret->rect, 0, 0, ptsize, ptsize);
	return(ret);
}

void TTF_CloseFont(TTF_Font *font) {

	TTFFNT	fhdl;

	if (font) {
		fhdl = (TTFFNT)font;
		DeleteObject(SelectObject(fhdl->hdcimage, fhdl->hBitmap));
		DeleteObject(SelectObject(fhdl->hdcimage, fhdl->hfont));
		DeleteDC(fhdl->hdcimage);
		_MFREE(fhdl);
	}
}

SDL_Surface *TTF_RenderUNICODE_Solid(TTF_Font *font, const WORD *text,
															SDL_Color fg) {

	SDL_Surface	*ret;
	char		sjis[4];
	int			leng;
	TTFFNT		fhdl;
	SIZE		fntsize;
	SDL_Palette	*palette;
const BYTE		*src;
	BYTE		*dst;
	int			x;

	ret = NULL;
	if ((font == NULL) || (text == NULL)) {
		goto trus_exit;
	}
	leng = WideCharToMultiByte(CP_ACP, 0, text, -1,
										sjis, sizeof(sjis), NULL, NULL) - 1;
	fhdl = (TTFFNT)font;
	FillRect(fhdl->hdcimage, &fhdl->rect,
										(HBRUSH)GetStockObject(BLACK_BRUSH));
	TextOut(fhdl->hdcimage, 0, 0, sjis, leng);
	if ((!GetTextExtentPoint32(fhdl->hdcimage, sjis, leng, &fntsize)) ||
		(fntsize.cx == 0) || (fntsize.cy == 0)) {
		goto trus_exit;
	}
	fntsize.cx = min(fntsize.cx, fhdl->ptsize);
	fntsize.cy = min(fntsize.cy, fhdl->ptsize);
	ret = SDL_CreateRGBSurface(SDL_SWSURFACE, fntsize.cx, fntsize.cy, 8,
																0, 0, 0, 0);
	if (ret == NULL) {
		goto trus_exit;
	}
	palette = ret->format->palette;
	palette->colors[0].r = (BYTE)(fg.r ^ 0xff);
	palette->colors[0].g = (BYTE)(fg.g ^ 0xff);
	palette->colors[0].b = (BYTE)(fg.b ^ 0xff);
	palette->colors[1].r = fg.r;
	palette->colors[1].g = fg.g;
	palette->colors[1].b = fg.b;

	src = (BYTE *)fhdl->image;
	dst = (BYTE *)ret->pixels;
	do {
		for (x=0; x<fntsize.cx; x++) {
			dst[x] = (src[x / 8] >> ((x ^ 7) & 7)) & 1;
		}
		src += fhdl->bmpalign;
		dst += ret->pitch;
	} while(--fntsize.cy);

trus_exit:
	return(ret);
}

