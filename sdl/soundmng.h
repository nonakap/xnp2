
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
#define soundmng_sync()
#define soundmng_setreverse(r)

#define	soundmng_pcmplay(a, b)
#define	soundmng_pcmstop(a)

#ifdef __cplusplus
}
#endif


// ---- for SDL

void soundmng_initialize(void);
void soundmng_deinitialize(void);

