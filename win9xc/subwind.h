
#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
BOOL memdbg_initialize(HINSTANCE hInstance);
void memdbg_create(void);
void memdbg_destroy(void);
void memdbg_process(void);
void memdbg_readini(void);
void memdbg_writeini(void);
#else
#define memdbg_initialize(i)		(SUCCESS)
#define	memdbg_create()
#define	memdbg_destroy()
#define	memdbg_process()
#define memdbg_readini()
#define memdbg_writeini()
#endif

#if defined(SUPPORT_SOFTKBD)
BOOL skbdwin_initialize(HINSTANCE hInstance);
void skbdwin_deinitialize(void);
void skbdwin_create(void);
void skbdwin_destroy(void);
void skbdwin_process(void);
void skbdwin_readini(void);
void skbdwin_writeini(void);
#else
#define skbdwin_initialize(i)		(SUCCESS)
#define	skbdwin_deinitialize()
#define	skbdwin_create()
#define	skbdwin_destroy()
#define skbdwin_process()
#define skbdwin_readini()
#define skbdwin_writeini()
#endif

