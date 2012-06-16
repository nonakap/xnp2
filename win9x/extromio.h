
typedef struct {
	UINT	type;
	void	*fh;
	long	pos;
	UINT	size;
} _EXTROMH, *EXTROMH;


enum {
	ERSEEK_SET		= 0,
	ERSEEK_CUR		= 1,
	ERSEEK_END
};

enum {
	EXTROMIO_FILE	= 0,
	EXTROMIO_RES	= 1
};


#ifdef __cplusplus
extern "C" {
#endif

EXTROMH extromio_open(const OEMCHAR *filename, UINT type);
UINT extromio_read(EXTROMH erh, void *buf, UINT size);
long extromio_seek(EXTROMH erh, long pos, int mothod);
void extromio_close(EXTROMH erh);

#ifdef __cplusplus
}
#endif

