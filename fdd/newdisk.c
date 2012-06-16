#include	"compiler.h"
#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
#include	"oemtext.h"
#endif
#include	"dosio.h"
#include	"newdisk.h"
#include	"fddfile.h"
#include	"sxsi.h"
#include	"hddboot.res"


// ---- fdd

void newdisk_fdd(const OEMCHAR *fname, REG8 type, const OEMCHAR *label) {

	_D88HEAD	d88head;
	FILEH		fh;

	ZeroMemory(&d88head, sizeof(d88head));
	STOREINTELDWORD(d88head.fd_size, sizeof(d88head));
#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	oemtext_oemtosjis((char *)d88head.fd_name, sizeof(d88head.fd_name),
															label, (UINT)-1);
#else
	milstr_ncpy((char *)d88head.fd_name, label, sizeof(d88head.fd_name));
#endif
	d88head.fd_type = type;
	fh = file_create(fname);
	if (fh != FILEH_INVALID) {
		file_write(fh, &d88head, sizeof(d88head));
		file_close(fh);
	}
}


// ---- hdd

static BOOL writezero(FILEH fh, UINT size) {

	UINT8	work[256];
	UINT	wsize;

	ZeroMemory(work, sizeof(work));
	while(size) {
		wsize = min(size, sizeof(work));
		if (file_write(fh, work, wsize) != wsize) {
			return(FAILURE);
		}
		size -= wsize;
	}
	return(SUCCESS);
}

static BOOL writehddipl(FILEH fh, UINT ssize, UINT32 tsize) {

	UINT8	work[1024];
	UINT	size;

	ZeroMemory(work, sizeof(work));
	CopyMemory(work, hdddiskboot, sizeof(hdddiskboot));
	if (ssize < 1024) {
		work[ssize - 2] = 0x55;
		work[ssize - 1] = 0xaa;
	}
	if (file_write(fh, work, sizeof(work)) != sizeof(work)) {
		return(FAILURE);
	}
	if (tsize > sizeof(work)) {
		tsize -= sizeof(work);
		ZeroMemory(work, sizeof(work));
		while(tsize) {
			size = min(tsize, sizeof(work));
			tsize -= size;
			if (file_write(fh, work, size) != size) {
				return(FAILURE);
			}
		}
	}
	return(SUCCESS);
}

void newdisk_thd(const OEMCHAR *fname, UINT hddsize) {

	FILEH	fh;
	UINT8	work[256];
	UINT	size;
	BOOL	r;

	if ((fname == NULL) || (hddsize < 5) || (hddsize > 256)) {
		goto ndthd_err;
	}
	fh = file_create(fname);
	if (fh == FILEH_INVALID) {
		goto ndthd_err;
	}
	ZeroMemory(work, 256);
	size = hddsize * 15;
	STOREINTELWORD(work, size);
	r = (file_write(fh, work, 256) != 256);
	r |= writehddipl(fh, 256, 0);
	file_close(fh);
	if (r) {
		file_delete(fname);
	}

ndthd_err:
	return;
}

void newdisk_nhd(const OEMCHAR *fname, UINT hddsize) {

	FILEH	fh;
	NHDHDR	nhd;
	UINT	size;
	BOOL	r;

	if ((fname == NULL) || (hddsize < 5) || (hddsize > 512)) {
		goto ndnhd_err;
	}
	fh = file_create(fname);
	if (fh == FILEH_INVALID) {
		goto ndnhd_err;
	}
	ZeroMemory(&nhd, sizeof(nhd));
	CopyMemory(&nhd.sig, sig_nhd, 15);
	STOREINTELDWORD(nhd.headersize, sizeof(nhd));
	size = hddsize * 15;
	STOREINTELDWORD(nhd.cylinders, size);
	STOREINTELWORD(nhd.surfaces, 8);
	STOREINTELWORD(nhd.sectors, 17);
	STOREINTELWORD(nhd.sectorsize, 512);
	r = (file_write(fh, &nhd, sizeof(nhd)) != sizeof(nhd));
	r |= writehddipl(fh, 512, size * 8 * 17 * 512);
	file_close(fh);
	if (r) {
		file_delete(fname);
	}

ndnhd_err:
	return;
}

// hddtype = 0:5MB / 1:10MB / 2:15MB / 3:20MB / 5:30MB / 6:40MB
void newdisk_hdi(const OEMCHAR *fname, UINT hddtype) {

const SASIHDD	*sasi;
	FILEH		fh;
	HDIHDR		hdi;
	UINT32		size;
	BOOL		r;

	hddtype &= 7;
	if ((fname == NULL) || (hddtype == 7)) {
		goto ndhdi_err;
	}
	sasi = sasihdd + hddtype;
	fh = file_create(fname);
	if (fh == FILEH_INVALID) {
		goto ndhdi_err;
	}
	ZeroMemory(&hdi, sizeof(hdi));
	size = 256 * sasi->sectors * sasi->surfaces * sasi->cylinders;
//	STOREINTELDWORD(hdi.hddtype, 0);
	STOREINTELDWORD(hdi.headersize, 4096);
	STOREINTELDWORD(hdi.hddsize, size);
	STOREINTELDWORD(hdi.sectorsize, 256);
	STOREINTELDWORD(hdi.sectors, sasi->sectors);
	STOREINTELDWORD(hdi.surfaces, sasi->surfaces);
	STOREINTELDWORD(hdi.cylinders, sasi->cylinders);
	r = (file_write(fh, &hdi, sizeof(hdi)) != sizeof(hdi));
	r |= writezero(fh, 4096 - sizeof(hdi));
	r |= writehddipl(fh, 256, size);
	file_close(fh);
	if (r) {
		file_delete(fname);
	}

ndhdi_err:
	return;
}

void newdisk_vhd(const OEMCHAR *fname, UINT hddsize) {

	FILEH	fh;
	VHDHDR	vhd;
	UINT	tmp;
	BOOL	r;

	if ((fname == NULL) || (hddsize < 2) || (hddsize > 512)) {
		goto ndvhd_err;
	}
	fh = file_create(fname);
	if (fh == FILEH_INVALID) {
		goto ndvhd_err;
	}
	ZeroMemory(&vhd, sizeof(vhd));
	CopyMemory(&vhd.sig, sig_vhd, 7);
	STOREINTELWORD(vhd.mbsize, (UINT16)hddsize);
	STOREINTELWORD(vhd.sectorsize, 256);
	vhd.sectors = 32;
	vhd.surfaces = 8;
	tmp = hddsize *	16;		// = * 1024 * 1024 / (8 * 32 * 256);
	STOREINTELWORD(vhd.cylinders, (UINT16)tmp);
	tmp *= 8 * 32;
	STOREINTELDWORD(vhd.totals, tmp);
	r = (file_write(fh, &vhd, sizeof(vhd)) != sizeof(vhd));
	r |= writehddipl(fh, 256, 0);
	file_close(fh);
	if (r) {
		file_delete(fname);
	}

ndvhd_err:
	return;
}

