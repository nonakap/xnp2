#include	"compiler.h"
#include	"timemng.h"


BOOL timemng_gettime(_SYSTIME *systime) {

	GetLocalTime((SYSTEMTIME *)systime);
	return(SUCCESS);
}

