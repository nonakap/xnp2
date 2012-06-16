#include	"compiler.h"
#include	"np2.h"
#include	"oemtext.h"
#include	"dosio.h"
#include	"extromio.h"


static const TCHAR str_extrom[] = _T("EXTROM");


EXTROMH extromio_open(const OEMCHAR *filename, UINT type) {

	EXTROMH	ret;
	HRSRC	hrsrc;
	HGLOBAL hg;

	ret = (EXTROMH)_MALLOC(sizeof(_EXTROMH), filename);
	if (ret == NULL) {
		goto erope_err1;
	}
	ret->type = type;
	if (type == EXTROMIO_FILE) {
		ret->fh = (void *)file_open_c(filename);
		if ((FILEH)ret->fh != FILEH_INVALID) {
			return(ret);
		}
	}
	else if (type == EXTROMIO_RES) {
#if defined(OSLANG_UTF8)
		TCHAR tchr[MAX_PATH];
		oemtotchar(tchr, NELEMENTS(tchr), filename, -1);
		hrsrc = FindResource(g_hInstance, tchr, str_extrom);
#else
		hrsrc = FindResource(g_hInstance, filename, str_extrom);
#endif
		if (hrsrc) {
			hg = LoadResource(g_hInstance, hrsrc);
			ret->fh = (void *)LockResource(hg);
			ret->pos = 0;
			ret->size = SizeofResource(g_hInstance, hrsrc);
			return(ret);
		}
	}
	_MFREE(ret);

erope_err1:
	return(NULL);
}

UINT extromio_read(EXTROMH erh, void *buf, UINT size) {

	if (erh) {
		if (erh->type == EXTROMIO_FILE) {
			return(file_read((FILEH)erh->fh, buf, size));
		}
		else if (erh->type == EXTROMIO_RES) {
			size = min(size, (UINT)(erh->size - erh->pos));
			if (size) {
				CopyMemory(buf, ((UINT8 *)erh->fh) + erh->pos, size);
				erh->pos += size;
			}
			return(size);
		}
	}
	return(0);
}

long extromio_seek(EXTROMH erh, long pos, int method) {

	long	ret;

	ret = 0;
	if (erh) {
		if (erh->type == EXTROMIO_FILE) {
			ret = file_seek((FILEH)erh->fh, pos, method);
		}
		else if (erh->type == EXTROMIO_RES) {
			if (method == ERSEEK_CUR) {
				ret = erh->pos;
			}
			else if (method == ERSEEK_END) {
				ret = erh->size;
			}
			ret += pos;
			if (ret < 0) {
				ret = 0;
			}
			else if ((UINT)ret > erh->size) {
				ret = erh->size;
			}
			erh->pos = ret;
		}
	}
	return(ret);
}

void extromio_close(EXTROMH erh) {

	if (erh) {
		if (erh->type == EXTROMIO_FILE) {
			file_close((FILEH)erh->fh);
		}
		_MFREE(erh);
	}
}

