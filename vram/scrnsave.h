
enum {
	SCRNSAVE_1BIT	= 0,
	SCRNSAVE_4BIT	= 1,
	SCRNSAVE_8BIT	= 2,
	SCRNSAVE_24BIT	= 3
};

enum {
	SCRNSAVE_AUTO	= 0
};

typedef struct {
	int		width;
	int		height;
	UINT	pals;
	UINT	type;
} *SCRNSAVE;


#ifdef __cplusplus
extern "C" {
#endif

SCRNSAVE scrnsave_get(void);
void scrnsave_trash(SCRNSAVE hdl);
BRESULT scrnsave_writebmp(SCRNSAVE hdl, const OEMCHAR *filename, UINT flag);
BRESULT scrnsave_writegif(SCRNSAVE hdl, const OEMCHAR *filename, UINT flag);

#ifdef __cplusplus
}
#endif

