#ifndef	NP2_X11_TRACE_H__
#define	NP2_X11_TRACE_H__

#ifdef	__cplusplus
extern "C" {
#endif

extern int trace_flag;

void trace_init(void);
void trace_term(void);
void trace_fmt(const char *str, ...);

#ifndef TRACE

#define	TRACEINIT()
#define	TRACETERM()
#define	TRACEOUT(a)
#ifndef	VERBOSE
#define	VERBOSE(s)
#endif

#else	/* TRACE */

#define	TRACEINIT()	trace_init()
#define	TRACETERM()	trace_term()
#define	TRACEOUT(arg)	trace_fmt arg
#ifndef	VERBOSE
#define	VERBOSE(arg)	if (trace_flag) trace_fmt arg
#endif

#endif	/* !TRACE */

#ifdef	__cplusplus
};
#endif

#endif	/* NP2_X11_TRACE_H__ */
