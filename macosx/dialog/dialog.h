enum {
	OPEN_FDD	= 0,
	OPEN_SASI,
	OPEN_SCSI,
	OPEN_FONT,
	OPEN_INI,
	OPEN_MIMPI
};

void dialog_changefdd(BYTE drv);
void dialog_changehdd(BYTE drv);

void dialog_font(void);

void fsspec2path(FSSpec *fs, char *dst, int leng);
void dialog_writebmp(void);
void dialog_s98(void);

BOOL dialog_fileselect(char *name, int size, WindowRef parent, int opentype);
BOOL dialog_filewriteselect(OSType type, char *title, FSSpec *fsc, WindowRef parentWindow);
