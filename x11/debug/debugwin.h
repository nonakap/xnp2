/*	$Id: debugwin.h,v 1.1 2004/03/23 15:16:43 monaka Exp $	*/

#if defined(SUPPORT_MEMDBG32)

void debugwin_create(void);
void debugwin_destroy(void);
void debugwin_process(void);

#else

#define	debugwin_create()
#define	debugwin_destroy()
#define	debugwin_process()

#endif
