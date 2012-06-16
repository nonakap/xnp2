
typedef struct {
	OEMCHAR	*disk[4];
	OEMCHAR	*ini;
	BOOL	fullscreen;
} NP2ARG_T;

extern NP2ARG_T	np2arg;

void np2arg_analize(void);

