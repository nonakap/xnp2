#include	"compiler.h"
#include	"np2.h"
#include	"commng.h"
#include	"cmjasts.h"
#include	"cmmidi.h"

// ---- non connect

static UINT ncread(COMMNG self, BYTE *data) {

	(void)self;
	(void)data;
	return(0);
}

static UINT ncwrite(COMMNG self, BYTE data) {

	(void)self;
	(void)data;
	return(0);
}

static BYTE ncgetstat(COMMNG self) {

	(void)self;
	return(0xf0);
}

static long ncmsg(COMMNG self, UINT msg, long param) {

	(void)self;
	(void)msg;
	(void)param;
	return(0);
}

static void ncrelease(COMMNG self) {
}

static const _COMMNG com_nc = {
		COMCONNECT_OFF, ncread, ncwrite, ncgetstat, ncmsg, ncrelease};


// ----

void commng_initialize(void) {

	cmmidi_initailize();
}

COMMNG commng_create(UINT device) {

	COMMNG	ret;
	COMCFG* cfg;
	
	ret = NULL;
	switch(device) {
		case COMCREATE_PRINTER:
			if (np2oscfg.jastsnd) {
				ret = cmjasts_create();
			}
			break;

		case COMCREATE_MPU98II:
			cfg = &np2oscfg.mpu;;
			ret = cmmidi_create(cfg->mout, cfg->min, cfg->mdl);
			if (ret) {
				ret->msg(ret, COMMSG_MIMPIDEFFILE, (long)cfg->def);
				ret->msg(ret, COMMSG_MIMPIDEFEN, (long)cfg->def_en);
			}
			break;

		default:
			break;
	}
	if (ret == NULL) {
		ret = (COMMNG)&com_nc;
	}
	return(ret);
}

void commng_destroy(COMMNG hdl) {

	if (hdl) {
		hdl->release(hdl);
	}
}

