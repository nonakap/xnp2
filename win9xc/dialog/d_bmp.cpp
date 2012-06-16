#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrnbmp.h"
#include	"font.h"


static const TCHAR fontui_title[] = _T("Select font file");
static const TCHAR fontui_filter[] =									\
					_T("PC-98 BMP font (*.bmp)\0")						\
						_T("*.bmp\0")									\
					_T("Virtual98 font\0")								\
						_T("font.rom\0")								\
					_T("All supported files\0")							\
						_T("*.bmp;")									\
						_T("pc88.fnt;kanji1.rom;kanji2.rom;")			\
						_T("font.rom;")									\
						_T("subsys_c.rom;kanji.rom;")					\
						_T("fnt0808.x1;fnt0816.x1;fnt1616.x1;")			\
						_T("cgrom.dat\0");
static const FILESEL fontui = {fontui_title, str_bmp, fontui_filter, 3};

static const TCHAR bmpui_file[] = _T("NP2_%04d.BMP");
static const TCHAR bmpui_title[] = _T("Save as bitmap file");
static const TCHAR bmpui_filter1[] = _T("1bit-bitmap (*.bmp)\0*.bmp\0");
static const TCHAR bmpui_filter4[] = _T("4bit-bitmap (*.bmp)\0*.bmp\0");
static const TCHAR bmpui_filter8[] = _T("8bit-bitmap (*.bmp)\0*.bmp\0");
static const TCHAR *bmpui_filter[3] = {
								bmpui_filter1, bmpui_filter4, bmpui_filter8};


void dialog_font(HWND hWnd) {

	TCHAR	path[MAX_PATH];

	file_cpyname(path, np2cfg.fontfile, NELEMENTS(path));
	if ((dlgs_selectfile(hWnd, &fontui, path, NELEMENTS(path), NULL)) &&
		(font_load(path, FALSE))) {
		gdcs.textdisp |= GDCSCRN_ALLDRAW2;
		milstr_ncpy(np2cfg.fontfile, path, NELEMENTS(np2cfg.fontfile));
		sysmng_update(SYS_UPDATECFG);
	}
}

void dialog_writebmp(HWND hWnd) {

	SCRNBMP	bmp;
	FILESEL	bmpui;
	TCHAR	path[MAX_PATH];
	FILEH	fh;

	bmp = scrnbmp();
	if (bmp) {
		bmpui.title = bmpui_title;
		bmpui.ext = str_bmp;
		bmpui.filter = bmpui_filter[bmp->type];
		bmpui.defindex = 1;
		file_cpyname(path, bmpfilefolder, NELEMENTS(path));
		file_cutname(path);
		file_catname(path, bmpui_file, NELEMENTS(path));
		if (dlgs_selectwritenum(hWnd, &bmpui, path, NELEMENTS(path))) {
			file_cpyname(bmpfilefolder, path, NELEMENTS(bmpfilefolder));
			sysmng_update(SYS_UPDATEOSCFG);
			fh = file_create(path);
			if (fh != FILEH_INVALID) {
				file_write(fh, bmp->ptr, bmp->size);
				file_close(fh);
			}
		}
		_MFREE(bmp);
	}
}

