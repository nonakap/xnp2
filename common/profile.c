#include	"compiler.h"
#include	"strres.h"
#include	"textfile.h"
#include	"profile.h"
#if defined(SUPPORT_TEXTCNV)
#include	"textcnv.h"
#endif
#include	"dosio.h"


static void strdelspace(OEMCHAR **buf, int *size) {

	OEMCHAR	*p;
	int		len;

	p = *buf;
	len = *size;
	while((len > 0) && (p[0] == ' ')) {
		p++;
		len--;
	}
	while((len > 0) && (p[len - 1] == ' ')) {
		len--;
	}
	*buf = p;
	*size = len;
}

static OEMCHAR *profana(OEMCHAR *buf, OEMCHAR **data) {

	int		len;
	OEMCHAR	*buf2;
	int		l;

	len = OEMSTRLEN(buf);
	strdelspace(&buf, &len);
	if ((len >= 2) && (buf[0] == '[') && (buf[len - 1] == ']')) {
		buf++;
		len -= 2;
		strdelspace(&buf, &len);
		buf[len] = '\0';
		*data = NULL;
		return(buf);
	}
	for (l=0; l<len; l++) {
		if (buf[l] == '=') {
			break;
		}
	}
	if (l < len) {
		len -= (l + 1);
		buf2 = buf + (l + 1);
		strdelspace(&buf, &l);
		buf[l] = '\0';
		strdelspace(&buf2, &len);
		if ((len >= 2) && (buf2[0] == '\"') && (buf2[len - 1] == '\"')) {
			buf2++;
			len -= 2;
			strdelspace(&buf2, &len);
		}
		buf2[len] = '\0';
		*data = buf2;
		return(buf);
	}
	return(NULL);
}

BOOL profile_enum(const OEMCHAR *filename, void *arg,
							BOOL (*proc)(void *arg, const OEMCHAR *para,
								const OEMCHAR *key, const OEMCHAR *data)) {
	TEXTFILEH	fh;
	BOOL		r;
	OEMCHAR		buf[0x200];
	OEMCHAR		para[0x100];
	OEMCHAR		*key;
	OEMCHAR		*data;

	r = FALSE;
	if (proc == NULL) {
		goto gden_err0;
	}
	fh = textfile_open(filename, 0x800);
	if (fh == NULL) {
		goto gden_err0;
	}
	para[0] = '\0';
	while(1) {
		if (textfile_read(fh, buf, NELEMENTS(buf)) != SUCCESS) {
			break;
		}
		key = profana(buf, &data);
		if (key) {
			if (data == NULL) {
				milstr_ncpy(para, key, NELEMENTS(para));
			}
			else {
				r = proc(arg, para, key, data);
				if (r != SUCCESS) {
					break;
				}
			}
		}
	}
	textfile_close(fh);

gden_err0:
	return(r);
}


// ----

const OEMCHAR *profile_getarg(const OEMCHAR *str, OEMCHAR *buf, UINT leng) {

	OEMCHAR	c;

	if (leng) {
		leng--;
	}
	else {
		buf = NULL;
	}
	if (str) {
		c = *str;
		while((c > 0) && (c <= 0x20)) {
			str++;
			c = *str;
		}
		if (c == 0) {
			str = NULL;
		}
	}
	if (str) {
		c = *str;
		while((c < 0) || (c > 0x20)) {
			if (leng) {
				*buf++ = c;
				leng--;
			}
			str++;
			c = *str;
		}
	}
	if (buf) {
		buf[0] = '\0';
	}
	return(str);
}



// ---- ‚Ü‚¾ƒeƒXƒg

typedef struct {
	UINT		applen;
	UINT		keylen;
	UINT		pos;
	UINT		size;
	UINT		apphit;
const OEMCHAR	*data;
	UINT		datasize;
} PFPOS;

#define	PFBUFSIZE	(1 << 8)

static OEMCHAR *delspace(const OEMCHAR *buf, UINT *len) {

	UINT	l;

	if ((buf != NULL) && (len != NULL)) {
		l = *len;
		while((l) && (buf[0] == ' ')) {
			l--;
			buf++;
		}
		while((l) && (buf[l - 1] == ' ')) {
			l--;
		}
		*len = l;
	}
	return((OEMCHAR *)buf);
}

static BRESULT seakey(PFILEH hdl, PFPOS *pfp, const OEMCHAR *app,
														const OEMCHAR *key) {

	PFPOS	ret;
	UINT	pos;
	UINT	size;
	OEMCHAR	*buf;
	UINT	len;
	UINT	cnt;

	if ((hdl == NULL) || (app == NULL) || (key == NULL)) {
		return(FAILURE);
	}
	ZeroMemory(&ret, sizeof(ret));
	ret.applen = OEMSTRLEN(app);
	ret.keylen = OEMSTRLEN(key);
	if ((ret.applen == 0) || (ret.keylen == 0)) {
		return(FAILURE);
	}

	pos = 0;
	size = hdl->size;
	while(size) {
		buf = hdl->buffer + pos;
		len = 0;
		cnt = 0;
		do {
			if (buf[len] == '\r') {
				if (((len + 1) < size) && (buf[len + 1] == '\n')) {
					cnt = 2;
				}
				else {
					cnt = 1;
				}
				break;
			}
			if (buf[len] == '\n') {
				cnt = 1;
				break;
			}
			len++;
		} while(len < size);
		cnt += len;
		buf = delspace(buf, &len);
		if ((len >= 2) && (buf[0] == '[') && (buf[len - 1] == ']')) {
			if (ret.apphit) {
				break;
			}
			buf++;
			len -= 2;
			buf = delspace(buf, &len);
			if ((len == ret.applen) && (!milstr_memcmp(buf, app))) {
				ret.apphit = 1;
			}
		}
		else if ((ret.apphit) && (len > ret.keylen) &&
				(!milstr_memcmp(buf, key))) {
			buf += ret.keylen;
			len -= ret.keylen;
			buf = delspace(buf, &len);
			if ((len) && (buf[0] == '=')) {
				buf++;
				len--;
				buf = delspace(buf, &len);
				ret.pos = pos;
				ret.size = cnt;
				ret.data = buf;
				ret.datasize = len;
				break;
			}
		}
		if (len) {
			ret.pos = pos + cnt;
			ret.size = 0;
		}
		pos += cnt;
		size -= cnt;
	}
	if (pfp) {
		*pfp = ret;
	}
	return(SUCCESS);
}

static BRESULT replace(PFILEH hdl, UINT pos, UINT size1, UINT size2) {

	UINT	cnt;
	UINT	size;
	UINT	newsize;
	OEMCHAR	*p;
	OEMCHAR	*q;

	size1 += pos;
	size2 += pos;
	if (size1 > hdl->size) {
		return(FAILURE);
	}
	cnt = hdl->size - size1;
	if (size1 < size2) {
		size = hdl->size + size2 - size1;
		if (size > hdl->buffers) {
			newsize = (size & (~(PFBUFSIZE - 1))) + PFBUFSIZE;
			p = (OEMCHAR *)_MALLOC(newsize * sizeof(OEMCHAR), "profile");
			if (p == NULL) {
				return(FAILURE);
			}
			if (hdl->buffer) {
				CopyMemory(p, hdl->buffer, hdl->buffers * sizeof(OEMCHAR));
				_MFREE(hdl->buffer);
			}
			hdl->buffer = p;
			hdl->buffers = newsize;
		}
		hdl->size = size;
		if (cnt) {
			p = hdl->buffer + size1;
			q = hdl->buffer + size2;
			do {
				--cnt;
				q[cnt] = p[cnt];
			} while(cnt);
		}
	}
	else if (size1 > size2) {
		hdl->size -= (size1 - size2);
		if (cnt) {
			p = hdl->buffer + size1;
			q = hdl->buffer + size2;
			do {
				*q++ = *p++;
			} while(--cnt);
		}
	}
	hdl->flag |= PFILEH_MODIFY;
	return(SUCCESS);
}

static PFILEH registfile(FILEH fh) {

	UINT	rsize;
#if defined(SUPPORT_TEXTCNV)
	TCINF	inf;
#endif
	UINT	hdrsize;
	UINT	width;
	UINT8	hdr[4];
	UINT	size;
	UINT	newsize;
	void	*buf;
#if defined(SUPPORT_TEXTCNV)
	void	*buf2;
#endif
	PFILEH	ret;

	rsize = file_read(fh, hdr, sizeof(hdr));
#if defined(SUPPORT_TEXTCNV)
	if (textcnv_getinfo(&inf, hdr, rsize) == 0) {
		goto rf_err1;
	}
	if (!(inf.caps & TEXTCNV_READ)) {
		goto rf_err1;
	}
	if ((inf.width != 1) && (inf.width != 2)) {
		goto rf_err1;
	}
	hdrsize = inf.hdrsize;
	width = inf.width;
#else
	hdrsize = 0;
	width = 1;
	if ((rsize >= 3) &&
		(hdr[0] == 0xef) && (hdr[1] == 0xbb) && (hdr[2] == 0xbf)) {
		// UTF-8
		hdrsize = 3;
	}
	else if ((rsize >= 2) && (hdr[0] == 0xff) && (hdr[1] == 0xfe)) {
		// UCSLE
		hdrsize = 2;
		width = 2;
#if defined(BYTESEX_BIG)
		goto rf_err1;
#endif
	}
	else if ((rsize >= 2) && (hdr[0] == 0xfe) && (hdr[1] == 0xff)) {
		// UCS2BE
		hdrsize = 2;
		width = 2;
#if defined(BYTESEX_LITTLE)
		goto rf_err1;
#endif
	}
	if (width != sizeof(OEMCHAR)) {
		goto rf_err1;
	}
#endif

	size = file_getsize(fh);
	if (size < hdrsize) {
		goto rf_err1;
	}
	if (file_seek(fh, (long)hdrsize, FSEEK_SET) != (long)hdrsize) {
		goto rf_err1;
	}
	size = (size - hdrsize) / width;
	newsize = (size & (~(PFBUFSIZE - 1))) + PFBUFSIZE;
	buf = _MALLOC(newsize * width, "profile");
	if (buf == NULL) {
		goto rf_err1;
	}
	rsize = file_read(fh, buf, newsize * width) / width;
#if defined(SUPPORT_TEXTCNV)
	if (inf.xendian) {
		textcnv_swapendian16(buf, rsize);
	}
	if (inf.tooem) {
		size = (inf.tooem)(NULL, 0, buf, rsize);
		newsize = (size & (~(PFBUFSIZE - 1))) + PFBUFSIZE;
		buf2 = _MALLOC(newsize * sizeof(OEMCHAR), "profile tmp");
		if (buf2 == NULL) {
			goto rf_err2;
		}
		(inf.tooem)(buf2, size, buf, rsize);
		_MFREE(buf);
		buf = buf2;
		rsize = size;
	}
#endif	// defined(SUPPORT_TEXTCNV)

	ret = (PFILEH)_MALLOC(sizeof(_PFILEH), "profile");
	if (ret == NULL) {
		goto rf_err2;
	}
	ZeroMemory(ret, sizeof(_PFILEH));
	ret->buffer = buf;
	ret->buffers = newsize;
	ret->size = rsize;
	if (hdrsize) {
		CopyMemory(ret->hdr, hdr, hdrsize);
	}
	ret->hdrsize = hdrsize;
	return(ret);

rf_err2:
	_MFREE(buf);

rf_err1:
	return(NULL);
}

static PFILEH registnew(void) {

const UINT8	*hdr;
	UINT	hdrsize;
	PFILEH	ret;

#if defined(OSLANG_UTF8)
	hdr = str_utf8;
	hdrsize = sizeof(str_utf8);
#elif defined(OSLANG_UCS2) 
	hdr = (UINT8 *)str_ucs2;
	hdrsize = sizeof(str_ucs2);
#else
	hdr = NULL;
	hdrsize = 0;
#endif

	ret = (PFILEH)_MALLOC(sizeof(_PFILEH), "profile");
	if (ret == NULL) {
		goto rn_err;
	}
	ZeroMemory(ret, sizeof(_PFILEH));
//	ret->buffer = NULL;
//	ret->buffers = 0;
//	ret->size = 0;
	if (hdrsize) {
		CopyMemory(ret->hdr, hdr, hdrsize);
	}
	ret->hdrsize = hdrsize;
	return(ret);

rn_err:
	return(NULL);
}

PFILEH profile_open(const OEMCHAR *filename, UINT flag) {

	PFILEH	ret;
	FILEH	fh;

	ret = NULL;
	if (filename != NULL) {
		fh = file_open_rb(filename);
		if (fh != FILEH_INVALID) {
			ret = registfile(fh);
			file_close(fh);
		}
		else if (flag & PFILEH_READONLY) {
		}
		else {
			ret = registnew();
		}
	}
	if (ret) {
		ret->flag = flag;
		file_cpyname(ret->path, filename, NELEMENTS(ret->path));
	}
	return(ret);
}

void profile_close(PFILEH hdl) {

	void	*buf;
	UINT	bufsize;
#if defined(SUPPORT_TEXTCNV)
	TCINF	inf;
	void	*buf2;
	UINT	buf2size;
#endif
	UINT	hdrsize;
	UINT	width;
	FILEH	fh;

	if (hdl == NULL) {
		return;
	}
	buf = hdl->buffer;
	bufsize = hdl->size;
	if (hdl->flag & PFILEH_MODIFY) {
#if defined(SUPPORT_TEXTCNV)
		if (textcnv_getinfo(&inf, hdl->hdr, hdl->hdrsize) == 0) {
			goto wf_err1;
		}
		if (!(inf.caps & TEXTCNV_WRITE)) {
			goto wf_err1;
		}
		if ((inf.width != 1) && (inf.width != 2)) {
			goto wf_err1;
		}
		if (inf.fromoem) {
			buf2size = (inf.fromoem)(NULL, 0, buf, bufsize);
			buf2 = _MALLOC(buf2size * inf.width, "profile tmp");
			if (buf2 == NULL) {
				goto wf_err1;
			}
			(inf.fromoem)(buf2, buf2size, buf, bufsize);
			_MFREE(buf);
			buf = buf2;
			bufsize = buf2size;
		}
		if (inf.xendian) {
			textcnv_swapendian16(buf, bufsize);
		}
		hdrsize = inf.hdrsize;
		width = inf.width;
#else	// defined(SUPPORT_TEXTCNV)
		hdrsize = hdl->hdrsize;
		width = sizeof(OEMCHAR);
#endif	// defined(SUPPORT_TEXTCNV)
		fh = file_create(hdl->path);
		if (fh == FILEH_INVALID) {
			goto wf_err1;
		}
		if (hdrsize) {
			file_write(fh, hdl->hdr, hdrsize);
		}
		file_write(fh, buf, bufsize * width);
		file_close(fh);
	}

wf_err1:
	if (buf) {
		_MFREE(buf);
	}
	_MFREE(hdl);
}

BRESULT profile_read(const OEMCHAR *app, const OEMCHAR *key,
					const OEMCHAR *def, OEMCHAR *ret, UINT size, PFILEH hdl) {

	PFPOS	pfp;

	if ((seakey(hdl, &pfp, app, key) != SUCCESS) || (pfp.data == NULL)) {
		if (def == NULL) {
			def = str_null;
		}
		milstr_ncpy(ret, def, size);
		return(FAILURE);
	}
	else {
		size = min(size, pfp.datasize + 1);
		milstr_ncpy(ret, pfp.data, size);
		return(SUCCESS);
	}
}

BRESULT profile_write(const OEMCHAR *app, const OEMCHAR *key,
											const OEMCHAR *data, PFILEH hdl) {

	PFPOS	pfp;
	UINT	newsize;
	UINT	datalen;
	OEMCHAR	*buf;

	if ((hdl == NULL) || (hdl->flag & PFILEH_READONLY) ||
		(data == NULL) || (seakey(hdl, &pfp, app, key) != SUCCESS)) {
		return(FAILURE);
	}
	if (!pfp.apphit) {
		newsize = pfp.applen + 2;
#if defined(OSLINEBREAK_CR) || defined(OSLINEBREAK_CRLF)
		newsize++;
#endif
#if defined(OSLINEBREAK_LF) || defined(OSLINEBREAK_CRLF)
		newsize++;
#endif
		if (replace(hdl, pfp.pos, 0, newsize) != SUCCESS) {
			return(FAILURE);
		}
		buf = hdl->buffer + pfp.pos;
		*buf++ = '[';
		CopyMemory(buf, app, pfp.applen * sizeof(OEMCHAR));
		buf += pfp.applen;
		*buf++ = ']';
#if defined(OSLINEBREAK_CR) || defined(OSLINEBREAK_CRLF)
		*buf++ = '\r';
#endif
#if defined(OSLINEBREAK_LF) || defined(OSLINEBREAK_CRLF)
		*buf++ = '\n';
#endif
		pfp.pos += newsize;
	}
	datalen = OEMSTRLEN(data);
	newsize = pfp.keylen + 1 + datalen;
#if defined(OSLINEBREAK_CR) || defined(OSLINEBREAK_CRLF)
	newsize++;
#endif
#if defined(OSLINEBREAK_LF) || defined(OSLINEBREAK_CRLF)
	newsize++;
#endif
	if (replace(hdl, pfp.pos, pfp.size, newsize) != SUCCESS) {
		return(FAILURE);
	}
	buf = hdl->buffer + pfp.pos;
	CopyMemory(buf, key, pfp.keylen * sizeof(OEMCHAR));
	buf += pfp.keylen;
	*buf++ = '=';
	CopyMemory(buf, data, datalen * sizeof(OEMCHAR));
	buf += datalen;
#if defined(OSLINEBREAK_CR) || defined(OSLINEBREAK_CRLF)
	*buf++ = '\r';
#endif
#if defined(OSLINEBREAK_LF) || defined(OSLINEBREAK_CRLF)
	*buf++ = '\n';
#endif
	return(SUCCESS);
}


// ----

static void bitmapset(UINT8 *ptr, UINT pos, BOOL set) {

	UINT8	bit;

	ptr += (pos >> 3);
	bit = 1 << (pos & 7);
	if (set) {
		*ptr |= bit;
	}
	else {
		*ptr &= ~bit;
	}
}

static BOOL bitmapget(const UINT8 *ptr, UINT pos) {

	return((ptr[pos >> 3] >> (pos & 7)) & 1);
}

static void binset(UINT8 *bin, UINT binlen, const OEMCHAR *src) {

	UINT	i;
	UINT8	val;
	BOOL	set;
	OEMCHAR	c;

	for (i=0; i<binlen; i++) {
		val = 0;
		set = FALSE;
		while(*src == ' ') {
			src++;
		}
		while(1) {
			c = *src;
			if ((c == '\0') || (c == ' ')) {
				break;
			}
			else if ((c >= '0') && (c <= '9')) {
				val <<= 4;
				val += c - '0';
				set = TRUE;
			}
			else {
				c |= 0x20;
				if ((c >= 'a') && (c <= 'f')) {
					val <<= 4;
					val += c - 'a' + 10;
					set = TRUE;
				}
			}
			src++;
		}
		if (set == FALSE) {
			break;
		}
		bin[i] = val;
	}
}

static void binget(OEMCHAR *work, int size, const UINT8 *bin, UINT binlen) {

	UINT	i;
	OEMCHAR	tmp[8];

	if (binlen) {
		OEMSPRINTF(tmp, OEMTEXT("%.2x"), bin[0]);
		milstr_ncpy(work, tmp, size);
	}
	for (i=1; i<binlen; i++) {
		OEMSPRINTF(tmp, OEMTEXT(" %.2x"), bin[i]);
		milstr_ncat(work, tmp, size);
	}
}

void profile_iniread(const OEMCHAR *path, const OEMCHAR *app,
								const PFTBL *tbl, UINT count, PFREAD cb) {

	PFILEH	pfh;
const PFTBL	*p;
const PFTBL	*pterm;
	OEMCHAR	work[512];

	pfh = profile_open(path, 0);
	if (pfh == NULL) {
		return;
	}
	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		if (profile_read(app, p->item, NULL, work, sizeof(work), pfh)
																== SUCCESS) {
			switch(p->itemtype & PFTYPE_MASK) {
				case PFTYPE_STR:
					milstr_ncpy((OEMCHAR *)p->value, work, p->arg);
					break;

				case PFTYPE_BOOL:
					*((UINT8 *)p->value) = (!milstr_cmp(work, str_true))?1:0;
					break;

				case PFTYPE_BITMAP:
					bitmapset((UINT8 *)p->value, p->arg,
									(!milstr_cmp(work, str_true))?TRUE:FALSE);
					break;

				case PFTYPE_BIN:
					binset((UINT8 *)p->value, p->arg, work);
					break;

				case PFTYPE_SINT8:
				case PFTYPE_UINT8:
					*(UINT8 *)p->value = (UINT32)milstr_solveINT(work);
					break;

				case PFTYPE_SINT16:
				case PFTYPE_UINT16:
					*(UINT16 *)p->value = (UINT32)milstr_solveINT(work);
					break;

				case PFTYPE_SINT32:
				case PFTYPE_UINT32:
					*(UINT32 *)p->value = (UINT32)milstr_solveINT(work);
					break;

				case PFTYPE_HEX8:
					*(UINT8 *)p->value = (UINT8)milstr_solveHEX(work);
					break;

				case PFTYPE_HEX16:
					*(UINT16 *)p->value = (UINT16)milstr_solveHEX(work);
					break;

				case PFTYPE_HEX32:
					*(UINT32 *)p->value = (UINT32)milstr_solveHEX(work);
					break;

				default:
					if (cb != NULL) {
						(*cb)(p, work);
					}
					break;
			}
		}
		p++;
	}
	profile_close(pfh);
}

void profile_iniwrite(const OEMCHAR *path, const OEMCHAR *app,
								const PFTBL *tbl, UINT count, PFWRITE cb) {

	PFILEH		pfh;
const PFTBL		*p;
const PFTBL		*pterm;
const OEMCHAR	*set;
	OEMCHAR		work[512];

	pfh = profile_open(path, 0);
	if (pfh == NULL) {
		return;
	}
	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		if (!(p->itemtype & PFFLAG_RO)) {
			work[0] = '\0';
			set = work;
			switch(p->itemtype & PFTYPE_MASK) {
				case PFTYPE_STR:
					set = (OEMCHAR *)p->value;
					break;

				case PFTYPE_BOOL:
					set = (*((UINT8 *)p->value))?str_true:str_false;
					break;

				case PFTYPE_BITMAP:
					set = (bitmapget((UINT8 *)p->value, p->arg))?
														str_true:str_false;
					break;

				case PFTYPE_BIN:
					binget(work, NELEMENTS(work), (UINT8 *)p->value, p->arg);
					break;

				case PFTYPE_SINT8:
					OEMSPRINTF(work, str_d, *((SINT8 *)p->value));
					break;

				case PFTYPE_SINT16:
					OEMSPRINTF(work, str_d, *((SINT16 *)p->value));
					break;

				case PFTYPE_SINT32:
					OEMSPRINTF(work, str_d, *((SINT32 *)p->value));
					break;

				case PFTYPE_UINT8:
					OEMSPRINTF(work, str_u, *((UINT8 *)p->value));
					break;

				case PFTYPE_UINT16:
					OEMSPRINTF(work, str_u, *((UINT16 *)p->value));
					break;

				case PFTYPE_UINT32:
					OEMSPRINTF(work, str_u, *((UINT32 *)p->value));
					break;

				case PFTYPE_HEX8:
					OEMSPRINTF(work, str_x, *((UINT8 *)p->value));
					break;

				case PFTYPE_HEX16:
					OEMSPRINTF(work, str_x, *((UINT16 *)p->value));
					break;

				case PFTYPE_HEX32:
					OEMSPRINTF(work, str_x, *((UINT32 *)p->value));
					break;

				default:
					if (cb != NULL) {
						set = (*cb)(p, work, NELEMENTS(work));
					}
					else {
						set = NULL;
					}
					break;
			}
			if (set) {
				profile_write(app, p->item, set, pfh);
			}
		}
		p++;
	}
	profile_close(pfh);
}

