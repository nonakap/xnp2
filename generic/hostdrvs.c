#include	"compiler.h"

#if defined(SUPPORT_HOSTDRV)

#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
#include	"oemtext.h"
#endif
#include	"dosio.h"
#include	"pccore.h"
#include	"hostdrv.h"
#include	"hostdrvs.h"


static const HDRVDIR hddroot = {"           ", 0, 0, 0, 0x10, {0}, {0}};

static const UINT8 dospathchr[] = {
			0xfa, 0x23,		// '&%$#"!  /.-,+*)(
			0xff, 0x03,		// 76543210 ?>=<;:98
			0xff, 0xff,		// GFEDCBA@ ONMLKJIH
			0xff, 0xef,		// WVUTSRQP _^]\[ZYX
			0x01, 0x00,		// gfedcba` onmlkjih
			0x00, 0x40};	// wvutsrqp ~}|{zyx 


static void rcnvfcb(char *dst, UINT dlen, const char *src) {

	REG8	c;

	while(dlen) {
		c = (UINT8)*src++;
		if (c == 0) {
			break;
		}
#if defined(OSLANG_SJIS) || defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
		if ((((c ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			if (src[0] == '\0') {
				break;
			}
			if (dlen < 2) {
				break;
			}
			dst[0] = c;
			dst[1] = *src++;
			dst += 2;
			dlen -= 2;
		}
		else if (((c - 0x20) & 0xff) < 0x60) {
			if (((c - 'a') & 0xff) < 26) {
				c -= 0x20;
			}
			if (dospathchr[(c >> 3) - (0x20 >> 3)] & (1 << (c & 7))) {
				*dst++ = c;
				dlen--;
			}
		}
		else if (((c - 0xa0) & 0xff) < 0x40) {
			*dst++ = c;
			dlen--;
		}
#else
		if (((c - 0x20) & 0xff) < 0x60) {
			if (((c - 'a') & 0xff) < 26) {
				c -= 0x20;
			}
			if (dospathchr[(c >> 3) - (0x20 >> 3)] & (1 << (c & 7))) {
				*dst++ = c;
				dlen--;
			}
		}
		else if (c >= 0x80) {
			*dst++ = c;
			dlen--;
		}
#endif
	}
}

static BRESULT realname2fcb(char *fcbname, const FLINFO *fli) {

	OEMCHAR	*ext;
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	char	sjis[MAX_PATH];
#endif
	OEMCHAR	filename[MAX_PATH];

	FillMemory(fcbname, 11, ' ');

	ext = file_getext(fli->path);
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	oemtext_oemtosjis(sjis, sizeof(sjis), ext, (UINT)-1);
	rcnvfcb(fcbname+8, 3, sjis);
#else
	rcnvfcb(fcbname+8, 3, ext);
#endif

	file_cpyname(filename, fli->path, NELEMENTS(filename));
	file_cutext(filename);
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	oemtext_oemtosjis(sjis, sizeof(sjis), filename, (UINT)-1);
	rcnvfcb(fcbname+0, 8, sjis);
#else
	rcnvfcb(fcbname+0, 8, filename);
#endif
	return(SUCCESS);
}

static BOOL hddsea(void *vpItem, void *vpArg) {

	if (!memcmp(((HDRVLST)vpItem)->di.fcbname, vpArg, 11)) {
		return(TRUE);
	}
	return(FALSE);
}

static BOOL hddseadir(void *vpItem, void *vpArg) {

	if ((((HDRVLST)vpItem)->di.attr & 0x10) &&
		(!memcmp(((HDRVLST)vpItem)->di.fcbname, vpArg, 11))) {
		return(TRUE);
	}
	return(FALSE);
}

LISTARRAY hostdrvs_getpathlist(const OEMCHAR *realpath) {

	FLISTH		flh;
	FLINFO		fli;
	LISTARRAY	ret;
	char		fcbname[11];
	HDRVLST		hdd;

	flh = file_list1st(realpath, &fli);
	if (flh == FLISTH_INVALID) {
		goto hdgpl_err1;
	}
	ret = listarray_new(sizeof(_HDRVLST), 64);
	if (ret == NULL) {
		goto hdgpl_err2;
	}
	do {
		if ((realname2fcb(fcbname, &fli) == SUCCESS) &&
			(fcbname[0] != ' ') &&
			(listarray_enum(ret, hddsea, fcbname) == NULL)) {
			hdd = listarray_append(ret, NULL);
			if (hdd == NULL) {
				break;
			}
			CopyMemory(hdd->di.fcbname, fcbname, 11);
			hdd->di.exist = 1;
			hdd->di.caps = fli.caps;
			hdd->di.size = fli.size;
			hdd->di.attr = fli.attr;
			hdd->di.date = fli.date;
			hdd->di.time = fli.time;
			file_cpyname(hdd->realname, fli.path, NELEMENTS(hdd->realname));
//			TRACEOUT(("%s -> %11s", fli.path, fcbname));
		}
	} while(file_listnext(flh, &fli) == SUCCESS);
	if (listarray_getitems(ret) == 0) {
		goto hdgpl_err3;
	}
	file_listclose(flh);
	return(ret);

hdgpl_err3:
	listarray_destroy(ret);

hdgpl_err2:
	file_listclose(flh);

hdgpl_err1:
	return(NULL);
}


// ----

static char *dcnvfcb(char *dst, UINT len, char *src) {

	char	c;

	while(len) {
		c = src[0];
		if ((c == 0) || (c == '.') || (c == '\\')) {
			break;
		}
		if ((((c ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			if (src[1] == '\0') {
				break;
			}
			if (len < 2) {
				break;
			}
			src++;
			dst[0] = c;
			dst[1] = *src;
			dst += 2;
			len -= 2;
		}
		else {
			*dst++ = c;
			len--;
		}
		src++;
	}
	return(src);
}

static char *dospath2fcb(char *fcbname, char *dospath) {

	FillMemory(fcbname, 11, ' ');
	dospath = dcnvfcb(fcbname, 8, dospath);
	if (dospath[0] == '.') {
		dospath = dcnvfcb(fcbname + 8, 3, dospath + 1);
	}
	return(dospath);
}

BOOL hostdrvs_getrealpath(HDRVPATH *hdp, char *dospath) {

	OEMCHAR		path[MAX_PATH];
	LISTARRAY	lst;
const HDRVDIR 	*di;
	HDRVLST		hdl;
	char		fcbname[11];

	file_cpyname(path, np2cfg.hdrvroot, NELEMENTS(path));
	lst = NULL;
	di = &hddroot;
	while(dospath[0] != '\0') {
		if ((dospath[0] != '\\') || (!(di->attr & 0x10))) {
			goto hdsgrp_err;
		}
		file_setseparator(path, NELEMENTS(path));
		dospath++;
		if (dospath[0] == '\0') {
			di = &hddroot;
			break;
		}
		dospath = dospath2fcb(fcbname, dospath);
		listarray_destroy(lst);
		lst = hostdrvs_getpathlist(path);
		hdl = (HDRVLST)listarray_enum(lst, hddsea, fcbname);
		if (hdl == NULL) {
			goto hdsgrp_err;
		}
		file_catname(path, hdl->realname, NELEMENTS(path));
		di = &hdl->di;
	}
	if (hdp) {
		CopyMemory(&hdp->di, di, sizeof(HDRVDIR));
		file_cpyname(hdp->path, path, NELEMENTS(hdp->path));
	}
	listarray_destroy(lst);
	return(SUCCESS);

hdsgrp_err:
	listarray_destroy(lst);
	return(FAILURE);
}

BOOL hostdrvs_getrealdir(OEMCHAR *path, int size, char *fcb, char *dospath) {

	LISTARRAY	lst;
	HDRVLST		hdl;

	file_cpyname(path, np2cfg.hdrvroot, size);
	if (dospath[0] == '\\') {
		file_setseparator(path, size);
		dospath++;
	}
	else if (dospath[0] != '\0') {
		goto hdsgrd_err;
	}
	while(1) {
		dospath = dospath2fcb(fcb, dospath);
		if (dospath[0] != '\\') {
			break;
		}
		lst = hostdrvs_getpathlist(path);
		hdl = (HDRVLST)listarray_enum(lst, hddseadir, fcb);
		if (hdl != NULL) {
			file_catname(path, hdl->realname, size);
		}
		listarray_destroy(lst);
		if (hdl == NULL) {
			goto hdsgrd_err;
		}
		file_setseparator(path, size);
		dospath++;
	}
	if (dospath[0] != '\0') {
		goto hdsgrd_err;
	}
	return(SUCCESS);

hdsgrd_err:
	return(FAILURE);
}

BOOL hostdrvs_newrealpath(HDRVPATH *hdp, char *dospath) {

	OEMCHAR		path[MAX_PATH];
	char		fcb[11];
	LISTARRAY	lst;
	HDRVLST		hdl;
	char		dosname[16];
	UINT		i;
	char		*p;
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	OEMCHAR		oemname[64];
#endif

	if ((hostdrvs_getrealdir(path, NELEMENTS(path), fcb, dospath)
															!= SUCCESS) ||
	 	(fcb[0] == ' ')) {
		return(FAILURE);
	}
	lst = hostdrvs_getpathlist(path);
	hdl = (HDRVLST)listarray_enum(lst, hddsea, fcb);
	if (hdl != NULL) {
		file_catname(path, hdl->realname, NELEMENTS(path));
		if (hdp) {
			CopyMemory(&hdp->di, &hdl->di, sizeof(HDRVDIR));
			file_cpyname(hdp->path, path, NELEMENTS(hdp->path));
		}
	}
	else {
		p = dosname;
		for (i=0; (i<8) && (fcb[i] != ' '); i++) {
			*p++ = fcb[i];
		}
		if (fcb[8] != ' ') {
			*p++ = '.';
			for (i=8; (i<11) && (fcb[i] != ' '); i++) {
				*p++ = fcb[i];
			}
		}
		*p = '\0';
		// ここで SJIS->OEMコードに未変換！
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
		oemtext_sjistooem(oemname, NELEMENTS(oemname), dosname, (UINT)-1);
		file_catname(path, oemname, NELEMENTS(path));
#else
		file_catname(path, dosname, NELEMENTS(path));
#endif
		if (hdp) {
			ZeroMemory(&hdp->di, sizeof(hdp->di));
			CopyMemory(hdp->di.fcbname, fcb, 11);
			file_cpyname(hdp->path, path, NELEMENTS(hdp->path));
		}
	}
	listarray_destroy(lst);
	return(SUCCESS);
}


// ----

static BOOL fhdlallclose(void *vpItem, void *vpArg) {

	long	fh;

	fh = ((HDRVFILE)vpItem)->hdl;
	if (fh != (long)FILEH_INVALID) {
		((HDRVFILE)vpItem)->hdl = (long)FILEH_INVALID;
		file_close((FILEH)fh);
	}
	(void)vpArg;
	return(FALSE);
}

void hostdrvs_fhdlallclose(LISTARRAY fhdl) {

	listarray_enum(fhdl, fhdlallclose, NULL);
}

static BOOL fhdlsea(void *vpItem, void *vpArg) {

	if (((HDRVFILE)vpItem)->hdl == (long)FILEH_INVALID) {
		return(TRUE);
	}
	(void)vpArg;
	return(FALSE);
}

HDRVFILE hostdrvs_fhdlsea(LISTARRAY fhdl) {

	HDRVFILE	ret;

	if (fhdl == NULL) {
		TRACEOUT(("hostdrvs_fhdlsea hdl == NULL"));
	}
	ret = (HDRVFILE)listarray_enum(fhdl, fhdlsea, NULL);
	if (ret == NULL) {
		ret = (HDRVFILE)listarray_append(fhdl, NULL);
		if (ret != NULL) {
			ret->hdl = (long)FILEH_INVALID;
		}
	}
	return(ret);
}

#endif

