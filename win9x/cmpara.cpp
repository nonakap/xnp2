#include	"compiler.h"
#include	"commng.h"


typedef struct {
	HANDLE	hdl;
} _CMPARA, *CMPARA;


static UINT pararead(COMMNG self, UINT8 *data) {

	return(0);
}

static UINT parawrite(COMMNG self, UINT8 data) {

	CMPARA	para;
	DWORD	writesize;

	para = (CMPARA)(self + 1);
	WriteFile(para->hdl, &data, 1, &writesize, NULL);
	return(1);
}

static UINT8 paragetstat(COMMNG self) {

	return(0);
}

static long paramsg(COMMNG self, UINT msg, long param) {

	(void)self;
	(void)msg;
	(void)param;
	return(0);
}

static void pararelease(COMMNG self) {

	CMPARA	para;

	para = (CMPARA)(self + 1);
	CloseHandle(para->hdl);
	_MFREE(self);
}


// ----

COMMNG cmpara_create(UINT port) {

	TCHAR	commstr[16];
	HANDLE	hdl;
	COMMNG	ret;
	CMPARA	para;

	wsprintf(commstr, _T("LPT%u"), port);
	hdl = CreateFile(commstr, GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
	if (hdl == INVALID_HANDLE_VALUE) {
		goto cpcre_err1;
	}
	ret = (COMMNG)_MALLOC(sizeof(_COMMNG) + sizeof(_CMPARA), "PARALLEL");
	if (ret == NULL) {
		goto cpcre_err2;
	}
	ret->connect = COMCONNECT_PARALLEL;
	ret->read = pararead;
	ret->write = parawrite;
	ret->getstat = paragetstat;
	ret->msg = paramsg;
	ret->release = pararelease;
	para = (CMPARA)(ret + 1);
	para->hdl = hdl;
	return(ret);

cpcre_err2:
	CloseHandle(hdl);

cpcre_err1:
	return(NULL);
}

