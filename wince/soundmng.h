
// #define	WAVEMNG_CBMAIN

enum {
	SOUND_PCMSEEK		= 0,
	SOUND_PCMSEEK1		= 1,

	SOUND_MAXPCM
};

enum {
	SNDPROC_NP2			= 0,
	SNDPROC_MAIN,
	SNDPROC_MENU
};


#ifdef __cplusplus
extern "C" {
#endif

UINT soundmng_create(UINT rate, UINT ms);
void soundmng_destroy(void);
#define soundmng_reset()
#define	soundmng_play()			soundmng_enable(SNDPROC_NP2)
#define	soundmng_stop()			soundmng_disable(SNDPROC_NP2)
#define soundmng_sync()
#define soundmng_setreverse(r)

#define	soundmng_pcmplay(a, b)
#define	soundmng_pcmstop(a)


// ---- for windows

void soundmng_initialize(void);
void soundmng_deinitialize(void);

#if defined(WAVEMNG_CBMAIN)
void soundmng_cb(UINT msg, HWAVEOUT hwo, WAVEHDR *whd);
#endif
void soundmng_awake(void);

void soundmng_enable(UINT proc);
void soundmng_disable(UINT proc);

#ifdef __cplusplus
}
#endif

