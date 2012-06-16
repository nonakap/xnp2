
#define	MAX_FDDFILE		4							// ver0.31

#include	"d88head.h"

enum {
	FDDFILE_MF			= 0x80,
	FDDFILE_FM			= 0x20,

	FDDFILE_AM			= 0x08,
	FDDFILE_DDAM		= 0x02,

	FDDFILE_NONE		= 0x00,
	FDDCHECK_FM			= (FDDFILE_MF | FDDFILE_FM),
	FDDCHECK_MFM		= FDDFILE_MF,
	FDDCHECK_DAM		= FDDFILE_AM,
	FDDCHECK_DDAM		= (FDDFILE_AM | FDDFILE_DDAM),

	FDDWRITE_FM			= 0x40,
	FDDWRITE_DDAM		= 0x04
};

enum {
	DISKTYPE_NOTREADY	= 0,
	DISKTYPE_BETA,
	DISKTYPE_D88
};

enum {
	DISKTYPE_2D			= 0,
	DISKTYPE_2DD,
	DISKTYPE_2HD
};

typedef struct {
	UINT32	headersize;
	UINT8	tracks;
	UINT8	sectors;
	UINT8	n;
	UINT8	disktype;
	UINT8	rpm;
} _XDFINFO, *XDFINFO;

typedef struct {
	UINT		fdtype_major;
	UINT		fdtype_minor;
	UINT32		fd_size;
	UINT32		ptr[164];
	_D88HEAD	head;
} _D88INFO, *D88INFO;

typedef struct {
	OEMCHAR	fname[MAX_PATH];
	UINT	ftype;
	int		ro;
	UINT8	type;
	UINT8	num;
	UINT8	protect;
	union {
		_XDFINFO	xdf;
		_D88INFO	d88;
	} inf;
} _FDDFILE, *FDDFILE;


#ifdef __cplusplus
extern "C" {
#endif

extern	_FDDFILE	fddfile[MAX_FDDFILE];
extern	UINT8		fddlasterror;

// ãNìÆéûÇ…àÍâÒÇæÇØèâä˙âª
void fddfile_initialize(void);

void fddfile_reset2dmode(void);

OEMCHAR *fdd_diskname(REG8 drv);
OEMCHAR *fdd_getfileex(REG8 drv, UINT *ftype, int *ro);
BOOL fdd_diskready(REG8 drv);
BOOL fdd_diskprotect(REG8 drv);

BRESULT fdd_set(REG8 drv, const OEMCHAR *fname, UINT ftype, int ro);
BRESULT fdd_eject(REG8 drv);

BRESULT fdd_diskaccess(void);
BRESULT fdd_seek(void);
BRESULT fdd_seeksector(void);
BRESULT fdd_read(void);
BRESULT fdd_write(void);
BRESULT fdd_diagread(void);
BRESULT fdd_readid(void);
BRESULT fdd_writeid(void);

BRESULT fdd_formatinit(void);
BRESULT fdd_formating(const UINT8 *ID);
BOOL fdd_isformating(void);

#ifdef __cplusplus
}
#endif

