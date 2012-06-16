#include	"compiler.h"
#include	<stdarg.h>
#include	"codecnv.h"


int trace_flag = 0;

void trace_init(void) {
}

void trace_term(void) {
}

void trace_fmt(const char *fmt, ...) {

	va_list	ap;
	char	buf[1024];
#ifndef WIN32
	char	euc[1024];
#endif

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
#ifndef WIN32
	codecnv_sjistoeuc(euc, sizeof(euc), buf, sizeof(buf));
	fprintf(stderr, "%s\n", euc);
#else
	fprintf(stderr, "%s\n", buf);
#endif
}
