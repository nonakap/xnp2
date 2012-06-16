#include	"compiler.h"
#include	"dosio.h"
#include	"wavefile.h"


static BRESULT headwrite(WAVEWR hdl) {

	RIFF_HEADER		rif;
	WAVE_HEADER		hdr;
	WAVE_INFOS		inf;
	UINT			filesize;
	UINT			blk;
	UINT			rps;

	filesize = hdl->size;
	filesize += 4 + (sizeof(WAVE_HEADER) * 2) + sizeof(WAVE_INFOS);

	rif.sig = WAVE_SIG('R', 'I', 'F', 'F');
	STOREINTELDWORD(rif.size, filesize);
	rif.fmt = WAVE_SIG('W', 'A', 'V', 'E');
	if (file_write((FILEH)hdl->fh, &rif, sizeof(rif)) != sizeof(rif)) {
		return(FAILURE);
	}

	blk = hdl->ch * (hdl->bits / 8);
	rps = blk * hdl->rate;
	hdr.sig = WAVE_SIG('f', 'm', 't', ' ');
	STOREINTELDWORD(hdr.size, sizeof(inf));
	if (file_write((FILEH)hdl->fh, &hdr, sizeof(hdr)) != sizeof(hdr)) {
		return(FAILURE);
	}
	STOREINTELWORD(inf.format, 1);
	STOREINTELWORD(inf.channel, hdl->ch);
	STOREINTELDWORD(inf.rate, hdl->rate);
	STOREINTELDWORD(inf.rps, rps);
	STOREINTELWORD(inf.block, blk);
	STOREINTELWORD(inf.bit, hdl->bits);
	if (file_write((FILEH)hdl->fh, &inf, sizeof(inf)) != sizeof(inf)) {
		return(FAILURE);
	}

	hdr.sig = WAVE_SIG('d', 'a', 't', 'a');
	STOREINTELDWORD(hdr.size, hdl->size);
	if (file_write((FILEH)hdl->fh, &hdr, sizeof(hdr)) != sizeof(hdr)) {
		return(FAILURE);
	}
	return(SUCCESS);
}

static void dataflash(WAVEWR hdl) {

	UINT	size;

	size = (UINT)(hdl->ptr - hdl->buf);
	if (size) {
		hdl->size += file_write((FILEH)hdl->fh, hdl->buf, size);
	}
	hdl->ptr = hdl->buf;
	hdl->remain = sizeof(hdl->buf);
}


// ----

WAVEWR wavewr_open(const OEMCHAR *filename, UINT rate, UINT bits, UINT ch) {

	FILEH	fh;
	WAVEWR	ret;

	if ((filename == NULL) || (!rate)) {
		goto wwope_err1;
	}
	if ((bits != 8) && (bits != 16)) {
		goto wwope_err1;
	}
	if ((ch != 1) && (ch != 2)) {
		goto wwope_err1;
	}
	fh = file_create(filename);
	if (fh == FILEH_INVALID) {
		goto wwope_err1;
	}
	ret = (WAVEWR)_MALLOC(sizeof(_WAVEWR), "WAVEWR");
	if (ret == NULL) {
		goto wwope_err2;
	}
	ZeroMemory(ret, sizeof(_WAVEWR));
	ret->fh = (long)fh;
	ret->rate = rate;
	ret->bits = bits;
	ret->ch = ch;
	if (headwrite(ret) != SUCCESS) {
		goto wwope_err3;
	}
	ret->ptr = ret->buf;
	ret->remain = sizeof(ret->buf) - (sizeof(RIFF_HEADER) +
							(sizeof(WAVE_HEADER) * 2) + sizeof(WAVE_INFOS));
	return(ret);

wwope_err3:
	_MFREE(ret);

wwope_err2:
	file_close(fh);

wwope_err1:
	return(NULL);
}


UINT wavewr_write(WAVEWR hdl, const void *buf, UINT size) {

	UINT	wr;

	if (hdl == NULL) {
		return(0);
	}
	while(size) {
		wr = min(hdl->remain, size);
		CopyMemory(hdl->ptr, buf, wr);
		size -= wr;
		buf = ((UINT8 *)buf) + wr;
		hdl->ptr += wr;
		hdl->remain -= wr;
		if (!hdl->remain) {
			dataflash(hdl);
		}
	}
	return(0);
}

void wavewr_close(WAVEWR hdl) {

	if (hdl) {
		dataflash(hdl);
		file_seek((FILEH)hdl->fh, 0, FSEEK_SET);
		headwrite(hdl);
		file_close((FILEH)hdl->fh);
		_MFREE(hdl);
	}
}

