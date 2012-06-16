#include	"compiler.h"
#include	"dosio.h"
#include	"trace.h"

#ifdef TRACE

	static	FILEH	fh = FILEH_INVALID;

#if defined(WIN32) || defined(_WIN32_WCE)
static const char	cr[] = "\r\n";
#elif defined(MACOS)
static const char	cr[] = "\r";
#else
static const char	cr[] = "\n";
#endif


void trace_init(void) {

	fh = file_create_c("trace.txt");
}

void trace_term(void) {

	if (fh != FILEH_INVALID) {
		file_close(fh);
		fh = FILEH_INVALID;
	}
}

void trace_fmt(const char *fmt, ...) {

	char	buf[0x1000];
	va_list	ap;

	if (fh != FILEH_INVALID) {
		va_start(ap, fmt);
		vsprintf(buf, fmt, ap);
		va_end(ap);
		file_write(fh, buf, strlen(buf));
		file_write(fh, cr, strlen(cr));
	}
}

#endif
