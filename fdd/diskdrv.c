// フロッピーディスクの挿入延滞用のブリッヂ

#include	"compiler.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"diskdrv.h"
#include	"fddfile.h"
#include	"sxsi.h"


#define	DISK_DELAY	20			// (0.4sec)

	int		diskdrv_delay[4];
	OEMCHAR	diskdrv_fname[4][MAX_PATH];
	UINT	diskdrv_ftype[4];
	int		diskdrv_ro[4];


// ---- sxsi

void diskdrv_setsxsi(REG8 drv, const OEMCHAR *fname) {

	UINT	num;
	OEMCHAR	*p;
	int		leng;

	num = drv & 0x0f;
	p = NULL;
	leng = 0;
	if (!(drv & 0x20)) {			// SASI or IDE
		if (num < 2) {
			p = np2cfg.sasihdd[num];
			leng = NELEMENTS(np2cfg.sasihdd[0]);
		}
	}
#if defined(SUPPORT_SCSI)
	else {							// SCSI
		if (num < 4) {
			p = np2cfg.scsihdd[num];
			leng = NELEMENTS(np2cfg.scsihdd[0]);
		}
	}
#endif
	if (p) {
		if (fname) {
			file_cpyname(p, fname, leng);
		}
		else {
			p[0] = '\0';
		}
		sysmng_update(SYS_UPDATEHDD | SYS_UPDATECFG);
	}
	else {
		sxsi_devopen(drv, fname);
	}
}

OEMCHAR *diskdrv_getsxsi(REG8 drv) {

	UINT	num;

	num = drv & 0x0f;
	if (!(drv & 0x20)) {			// SASI or IDE
		if (num < 2) {
			return(np2cfg.sasihdd[num]);
		}
	}
#if defined(SUPPORT_SCSI)
	else {							// SCSI
		if (num < 4) {
			return(np2cfg.scsihdd[num]);
		}
	}
#endif
	return(sxsi_getfilename(drv));
}


void diskdrv_hddbind(void) {

	UINT	i;
	REG8	drv;

	drv = 0;
	for (i=0; i<2; i++) {
		sxsi_devclose(drv);
		sxsi_setdevtype(drv, SXSIDEV_HDD);
		if (sxsi_devopen(drv, np2cfg.sasihdd[i]) == SUCCESS) {
			drv++;
		}
		else {
			sxsi_setdevtype(drv, SXSIDEV_NC);
		}
	}
#if defined(SUPPORT_SCSI)
	drv = 0x20;
	for (i=0; i<4; i++) {
		sxsi_devclose(drv);
		sxsi_setdevtype(drv, SXSIDEV_HDD);
		if (sxsi_devopen(drv, np2cfg.scsihdd[i]) == SUCCESS) {
			drv++;
		}
		else {
			sxsi_setdevtype(drv, SXSIDEV_NC);
		}
	}
#endif
}

// ---- fdd

void diskdrv_readyfddex(REG8 drv, const OEMCHAR *fname,
												UINT ftype, int readonly) {

	if ((drv < 4) && (fdc.equip & (1 << drv))) {
		if ((fname != NULL) && (fname[0] != '\0')) {
			fdd_set(drv, fname, ftype, readonly);
			fdc.stat[drv] = FDCRLT_AI | drv;
			fdc_interrupt();
			sysmng_update(SYS_UPDATEFDD);
		}
	}
}

void diskdrv_setfddex(REG8 drv, const OEMCHAR *fname,
												UINT ftype, int readonly) {

	if ((drv < 4) && (fdc.equip & (1 << drv))) {
		fdd_eject(drv);
		diskdrv_delay[drv] = 0;
		diskdrv_fname[drv][0] = '\0';
		fdc.stat[drv] = FDCRLT_AI | FDCRLT_NR | drv;
		fdc_interrupt();

		if (fname) {
			diskdrv_delay[drv] = DISK_DELAY;
			diskdrv_ftype[drv] = ftype;
			diskdrv_ro[drv] = readonly;
			file_cpyname(diskdrv_fname[drv], fname,
												NELEMENTS(diskdrv_fname[0]));
		}
		sysmng_update(SYS_UPDATEFDD);
	}
}

void diskdrv_callback(void) {

	REG8	drv;

	for (drv=0; drv<4; drv++) {
		if (diskdrv_delay[drv]) {
			diskdrv_delay[drv]--;
			if (!diskdrv_delay[drv]) {
				diskdrv_readyfddex(drv, diskdrv_fname[drv],
										diskdrv_ftype[drv], diskdrv_ro[drv]);
				diskdrv_fname[drv][0] = '\0';
			}
		}
	}
}

