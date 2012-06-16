
#include	"profile.h"

#ifdef __cplusplus
extern "C" {
#endif

void ini_read(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count);
void ini_write(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count);

void initload(void);
void initsave(void);

#ifdef __cplusplus
}
#endif

