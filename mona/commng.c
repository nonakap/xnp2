#include	"compiler.h"
#include	"np2.h"
#include	"commng.h"


// ---- non connect

static UINT ncread(COMMNG self, UINT8 *data) {

	(void)self;
	(void)data;
	return(0);
}

static UINT ncwrite(COMMNG self, UINT8 data) {

	(void)self;
	(void)data;
	return(0);
}

static UINT8 ncgetstat(COMMNG self) {

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
}

COMMNG commng_create(UINT device) {

	return((COMMNG)&com_nc);
}

void commng_destroy(COMMNG hdl) {

	if (hdl) {
		hdl->release(hdl);
	}
}

