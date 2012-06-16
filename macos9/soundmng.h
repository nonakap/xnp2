
enum {
	SOUND_PCMSEEK		= 0,
	SOUND_PCMSEEK1		= 1,

	SOUND_MAXPCM
};


#ifdef __cplusplus
extern "C" {
#endif

UINT soundmng_create(UINT rate, UINT ms);
void soundmng_destroy(void);
#define soundmng_reset()
void soundmng_play(void);
void soundmng_stop(void);
#if !defined(SOUND_CRITICAL)
void soundmng_sync(void);
#else
#define	soundmng_sync()
#endif
#define soundmng_setreverse(reverse)

#define soundmng_pcmplay(num, loop)
#define	soundmng_pcmstop(num)

#ifdef __cplusplus
}
#endif


// ---- for mac


