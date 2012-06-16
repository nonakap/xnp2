#include	"compiler.h"
#include	"strres.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"fddfile.h"
#include	"fdd_xdf.h"
#include	"fdd_d88.h"


	_FDDFILE	fddfile[MAX_FDDFILE];
	UINT8		fddlasterror;


// ----

void fddfile_initialize(void) {

	ZeroMemory(fddfile, sizeof(fddfile));
}

void fddfile_reset2dmode(void) { 			// ver0.29
#if 0
	int		i;

	for (i=0; i<4; i++) {
		fddfile[i].mode2d = 0;
	}
#endif
}

OEMCHAR *fdd_diskname(REG8 drv) {

	if (drv >= MAX_FDDFILE) {
		return(NULL);
	}
	return(fddfile[drv].fname);
}

OEMCHAR *fdd_getfileex(REG8 drv, UINT *ftype, int *ro) {

	FDDFILE	fdd;

	if (drv >= MAX_FDDFILE) {
		return((OEMCHAR *)str_null);
	}
	fdd = fddfile + drv;
	if (ftype) {
		*ftype = fdd->ftype;
	}
	if (ro) {
		*ro = fdd->ro;
	}
	return(fdd->fname);
}

BOOL fdd_diskready(REG8 drv) {

	if ((drv >= MAX_FDDFILE) || (!fddfile[drv].fname[0])) {
		return(FALSE);
	}
	return(TRUE);
}

BOOL fdd_diskprotect(REG8 drv) {

	if ((drv >= MAX_FDDFILE) || (!fddfile[drv].protect)) {
		return(FALSE);
	}
	return(TRUE);
}


// --------------------------------------------------------------------------

BRESULT fdd_set(REG8 drv, const OEMCHAR *fname, UINT ftype, int ro) {

	FDDFILE		fdd;
	UINT		fddtype;
const OEMCHAR	*p;
	BRESULT		r;

	if (drv >= MAX_FDDFILE) {
		return(FAILURE);
	}
	fddtype = ftype;
	if (fddtype == FTYPE_NONE) {
		p = file_getext(fname);
		if ((!milstr_cmp(p, str_d88)) || (!milstr_cmp(p, str_88d)) ||
			(!milstr_cmp(p, str_d98)) || (!milstr_cmp(p, str_98d))) {
			fddtype = FTYPE_D88;
		}
		else if (!milstr_cmp(p, str_fdi)) {
			fddtype = FTYPE_FDI;
		}
		else {
			fddtype = FTYPE_BETA;
		}
	}
	fdd = fddfile + drv;
	switch(fddtype) {
		case FTYPE_FDI:
			r = fddxdf_setfdi(fdd, fname, ro);
			if (r == SUCCESS) {
				break;
			}
			/* FALLTHROUGH */

		case FTYPE_BETA:
			r = fddxdf_set(fdd, fname, ro);
			break;

		case FTYPE_D88:
			r = fddd88_set(fdd, fname, ro);
			break;

		default:
			r = FAILURE;
	}
	if (r == SUCCESS) {
		file_cpyname(fdd->fname, fname, NELEMENTS(fdd->fname));
		fdd->ftype = ftype;
		fdd->ro = ro;
	}
	return(FAILURE);
}

BRESULT fdd_eject(REG8 drv) {

	FDDFILE		fdd;

	if (drv >= MAX_FDDFILE) {
		return(FAILURE);
	}
	fdd = fddfile + drv;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_eject(fdd));

		case DISKTYPE_D88:
			return(fddd88_eject(fdd));
	}
	return(FAILURE);
}


// ----

BRESULT fdd_diskaccess(void) {

	FDDFILE		fdd;

	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_diskaccess(fdd));

		case DISKTYPE_D88:
			return(fdd_diskaccess_d88());
	}
	return(FAILURE);
}

BRESULT fdd_seek(void) {

	BRESULT		ret;
	FDDFILE		fdd;

	ret = FAILURE;
	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			ret = fddxdf_seek(fdd);
			break;

		case DISKTYPE_D88:
			ret = fdd_seek_d88();
			break;
	}
	fdc.treg[fdc.us] = fdc.ncn;
	return(ret);
}

BRESULT fdd_seeksector(void) {

	FDDFILE		fdd;

	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_seeksector(fdd));

		case DISKTYPE_D88:
			return(fdd_seeksector_d88());
	}
	return(FAILURE);
}


BRESULT fdd_read(void) {

	FDDFILE		fdd;

	sysmng_fddaccess(fdc.us);
	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_read(fdd));

		case DISKTYPE_D88:
			return(fdd_read_d88());
	}
	return(FAILURE);
}

BRESULT fdd_write(void) {

	FDDFILE		fdd;

	sysmng_fddaccess(fdc.us);
	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_write(fdd));

		case DISKTYPE_D88:
			return(fdd_write_d88());
	}
	return(FAILURE);
}

BRESULT fdd_readid(void) {

	FDDFILE		fdd;

	sysmng_fddaccess(fdc.us);
	fdd = fddfile + fdc.us;
	switch(fdd->type) {
		case DISKTYPE_BETA:
			return(fddxdf_readid(fdd));

		case DISKTYPE_D88:
			return(fdd_readid_d88());
	}
	return(FAILURE);
}

BRESULT fdd_formatinit(void) {

	if (fddfile[fdc.us].type == DISKTYPE_D88) {
		return(fdd_formatinit_d88());
	}
	return(FAILURE);
}

BRESULT fdd_formating(const UINT8 *ID) {

	sysmng_fddaccess(fdc.us);
	if (fddfile[fdc.us].type == DISKTYPE_D88) {
		return(fdd_formating_d88(ID));
	}
	return(FAILURE);
}

BOOL fdd_isformating(void) {

	if (fddfile[fdc.us].type == DISKTYPE_D88) {
		return(fdd_isformating_d88());
	}
	return(FALSE);
}

