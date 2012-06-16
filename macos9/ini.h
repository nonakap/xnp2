
enum {
	INITYPE_STR			= 0,
	INITYPE_BOOL,
	INITYPE_BITMAP,
	INITYPE_BYTEARG,
	INITYPE_SINT8,
	INITYPE_SINT16,
	INITYPE_SINT32,
	INITYPE_UINT8,
	INITYPE_UINT16,
	INITYPE_UINT32,
	INITYPE_HEX8,
	INITYPE_HEX16,
	INITYPE_HEX32,
	INITYPE_USER
};

typedef struct {
const char	*item;
	UINT	itemtype;
	void	*value;
	UINT	size;
} INITBL;


#ifdef __cplusplus
extern "C" {
#endif

void ini_read(const char *path, const char *title,
											const INITBL *tbl, UINT count);
void ini_write(const char *path, const char *title,
											const INITBL *tbl, UINT count);

void initload(void);
void initsave(void);

#ifdef __cplusplus
}
#endif

