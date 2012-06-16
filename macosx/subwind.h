#if defined(SUPPORT_KEYDISP)

BOOL kdispwin_initialize(void);
void kdispwin_create(void);
void kdispwin_destroy(void);
void kdispwin_draw(BYTE cnt);
void kdispwin_readini(void);
void kdispwin_writeini(void);

#else

#define kdispwin_initialize()
#define kdispwin_create()
#define kdispwin_destroy()
#define kdispwin_draw(a)
#define kdispwin_readini()
#define kdispwin_writeini()

#endif

#if defined(SUPPORT_SOFTKBD)

BOOL skbdwin_initialize(void);
void skbdwin_deinitialize(void);
void skbdwin_create(void);
void skbdwin_destroy(void);
void skbdwin_process(void);
void skbdwin_readini(void);
void skbdwin_writeini(void);

#else

#define	skbdwin_initialize(i)	(SUCCESS)
#define	skbdwin_deinitialize()
#define	skbdwin_create()
#define	skbdwin_destroy()
#define	skbdwin_process()
#define	skbdwin_readini()
#define	skbdwin_writeini()

#endif

