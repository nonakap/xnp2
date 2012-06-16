
// ---- com manager interface

enum {
	COMCREATE_SERIAL		= 0,
	COMCREATE_PC9861K1,
	COMCREATE_PC9861K2,
	COMCREATE_PRINTER,
	COMCREATE_MPU98II
};

enum {
	COMCONNECT_OFF			= 0,
	COMCONNECT_SERIAL,
	COMCONNECT_MIDI,
	COMCONNECT_PARALLEL
};

enum {
	COMMSG_MIDIRESET		= 0,
	COMMSG_SETFLAG,
	COMMSG_GETFLAG,
	COMMSG_USER
};

struct _commng;
typedef struct _commng	_COMMNG;
typedef struct _commng	*COMMNG;

struct _commng {
	UINT	connect;
	UINT	(*read)(COMMNG self, UINT8 *data);
	UINT	(*write)(COMMNG self, UINT8 data);
	UINT8	(*getstat)(COMMNG self);
	long	(*msg)(COMMNG self, UINT msg, long param);
	void	(*release)(COMMNG self);
};

typedef struct {
	UINT32	size;
	UINT32	sig;
	UINT32	ver;
	UINT32	param;
} _COMFLAG, *COMFLAG;


#ifdef __cplusplus
extern "C" {
#endif

COMMNG commng_create(UINT device);
void commng_destroy(COMMNG hdl);

#ifdef __cplusplus
}
#endif


// ----

void commng_initialize(void);

