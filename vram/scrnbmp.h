
enum {
	SCRNBMP_1BIT	= 0,
	SCRNBMP_4BIT	= 1,
	SCRNBMP_8BIT	= 2,
	SCRNBMP_24BIT	= 3
};

typedef struct {
	UINT	type;
	UINT8	*ptr;
	UINT	size;
} _SCRNBMP, *SCRNBMP;


#ifdef __cplusplus
extern "C" {
#endif

SCRNBMP scrnbmp(void);

#ifdef __cplusplus
}
#endif

