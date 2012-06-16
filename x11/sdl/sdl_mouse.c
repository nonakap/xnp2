#include "compiler.h"

#include "mousemng.h"


int
mousemng_initialize(void)
{

	return SUCCESS;
}

void
mouse_running(BYTE flg)
{

	UNUSED(flg);
}

void
mousemng_callback(void)
{
}

BYTE
mousemng_getstat(SINT16 *x, SINT16 *y, int clear)
{

	UNUSED(clear);

	*x = 0;
	*y = 0;
	return 0xa0;
}

