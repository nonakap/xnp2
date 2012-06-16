#ifndef	NP2_X11_TASKMNG_H__
#define	NP2_X11_TASKMNG_H__

#include "compiler.h"

#include "np2.h"

#ifdef __cplusplus
extern "C" {
#endif

void taskmng_initialize(void);
BOOL taskmng_sleep(UINT32 tick);
void taskmng_exit(void);

#define	taskmng_isavail()	np2running

#ifdef __cplusplus
}
#endif

#endif	/* NP2_X11_TASKMNG_H__ */
