
#include	"profile.h"

enum {
	PFTYPE_ARGS16		= PFTYPE_USER,
	PFTYPE_BYTE3,
	PFTYPE_KB,
};


#ifdef __cplusplus
extern "C" {
#endif

void ini_read(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count);
void ini_write(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count);


void initgetfile(OEMCHAR *path, UINT size);
void initload(void);
void initsave(void);

#ifdef __cplusplus
}
#endif

