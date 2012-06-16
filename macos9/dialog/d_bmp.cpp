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
#include	"scrnsave.h"
#include	"font.h"


void dialog_font(void) {

	char	path[MAX_PATH];

	if (dlgs_selectfile(path, sizeof(path))) {
		if (font_load(path, FALSE)) {
			gdcs.textdisp |= GDCSCRN_ALLDRAW2;
			milstr_ncpy(np2cfg.fontfile, path, sizeof(np2cfg.fontfile));
			sysmng_update(SYS_UPDATECFG);
		}
	}
}

void dialog_writebmp(void) {

	SCRNSAVE	ss;
	char		path[MAX_PATH];
const char		*ext;

	ss = scrnsave_get();
	if (ss == NULL) {
		return;
	}
	if (dlgs_selectwritefile(path, sizeof(path), "np2.bmp")) {
		ext = file_getext(path);
		if ((ss->type <= SCRNSAVE_8BIT) &&
			(!file_cmpname(ext, "gif"))) {
			scrnsave_writegif(ss, path, SCRNSAVE_AUTO);
		}
		else if (!file_cmpname(ext, str_bmp)) {
			scrnsave_writebmp(ss, path, SCRNSAVE_AUTO);
		}
	}
	scrnsave_trash(ss);
}

