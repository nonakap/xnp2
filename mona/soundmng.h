
enum {
	SOUND_PCMSEEK		= 0,
	SOUND_PCMSEEK1		= 1,

	SOUND_MAXPCM
};


#ifdef __cplusplus
extern "C" {
#endif

#if !defined(DISABLE_SOUND)
UINT soundmng_create(UINT rate, UINT ms);
void soundmng_destroy(void);
#define soundmng_reset()
void soundmng_play(void);
void soundmng_stop(void);
#define soundmng_sync()
#define soundmng_setreverse(r)
#else
#define soundmng_create(r, m)		(0)
#define soundmng_destroy()
#define soundmng_reset()
#define soundmng_play()
#define soundmng_stop()
#define soundmng_sync()
#define soundmng_setreverse(r)
#endif

#define	soundmng_pcmplay(a, b)
#define	soundmng_pcmstop(a)

#ifdef __cplusplus
}
#endif

// ---- for SDL

#define soundmng_initialize()
#define soundmng_deinitialize()

