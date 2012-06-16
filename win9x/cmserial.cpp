#include	"compiler.h"
#include	"commng.h"


const UINT32 cmserial_speed[10] = {110, 300, 1200, 2400, 4800,
							9600, 19200, 38400, 57600, 115200};


typedef struct {
	HANDLE	hdl;
} _CMSER, *CMSER;


static UINT serialread(COMMNG self, UINT8 *data) {

	CMSER	serial;
	COMSTAT	ct;
	DWORD	err;
	DWORD	readsize;

	serial = (CMSER)(self + 1);
	ClearCommError(serial->hdl, &err, &ct);
	if (ct.cbInQue) {
		if (ReadFile(serial->hdl, data, 1, &readsize, NULL)) {
			return(1);
		}
	}
	return(0);
}

static UINT serialwrite(COMMNG self, UINT8 data) {

	CMSER	serial;
	DWORD	writesize;

	serial = (CMSER)(self + 1);
	WriteFile(serial->hdl, &data, 1, &writesize, NULL);
	return(1);
}

static UINT8 serialgetstat(COMMNG self) {

	CMSER	serial;
	DCB		dcb;

	serial = (CMSER)(self + 1);
	GetCommState(serial->hdl, &dcb);
	if (!dcb.fDsrSensitivity) {
		return(0x20);
	}
	else {
		return(0x00);
	}
}

static long serialmsg(COMMNG self, UINT msg, long param) {

	(void)self;
	(void)msg;
	(void)param;
	return(0);
}

static void serialrelease(COMMNG self) {

	CMSER	serial;

	serial = (CMSER)(self + 1);
	CloseHandle(serial->hdl);
	_MFREE(self);
}


// ----

COMMNG cmserial_create(UINT port, UINT8 param, UINT32 speed) {

	TCHAR	commstr[16];
	HANDLE	hdl;
	DCB		dcb;
	UINT	i;
	COMMNG	ret;
	CMSER	serial;

	wsprintf(commstr, _T("COM%u"), port);
	hdl = CreateFile(commstr, GENERIC_READ | GENERIC_WRITE,
												0, 0, OPEN_EXISTING, 0, NULL);
	if (hdl == INVALID_HANDLE_VALUE) {
		goto cscre_err1;
	}
	GetCommState(hdl, &dcb);
	for (i=0; i<NELEMENTS(cmserial_speed); i++) {
		if (cmserial_speed[i] >= speed) {
			dcb.BaudRate = cmserial_speed[i];
			break;
		}
	}
	dcb.ByteSize = (UINT8)(((param >> 2) & 3) + 5);
	switch(param & 0x30) {
		case 0x10:
			dcb.Parity = ODDPARITY;
			break;

		case 0x30:
			dcb.Parity = EVENPARITY;
			break;

		default:
			dcb.Parity = NOPARITY;
			break;
	}
	switch(param & 0xc0) {
		case 0x80:
			dcb.StopBits = ONE5STOPBITS;
			break;

		case 0xc0:
			dcb.StopBits = TWOSTOPBITS;
			break;

		default:
			dcb.StopBits = ONESTOPBIT;
			break;
	}
	SetCommState(hdl, &dcb);
	ret = (COMMNG)_MALLOC(sizeof(_COMMNG) + sizeof(_CMSER), "SERIAL");
	if (ret == NULL) {
		goto cscre_err2;
	}
	ret->connect = COMCONNECT_MIDI;
	ret->read = serialread;
	ret->write = serialwrite;
	ret->getstat = serialgetstat;
	ret->msg = serialmsg;
	ret->release = serialrelease;
	serial = (CMSER)(ret + 1);
	serial->hdl = hdl;
	return(ret);

cscre_err2:
	CloseHandle(hdl);

cscre_err1:
	return(NULL);
}

