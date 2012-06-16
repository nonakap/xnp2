
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
void soundmng_setreverse(BOOL reverse);

BOOL soundmng_pcmplay(UINT num, BOOL loop);
void soundmng_pcmstop(UINT num);


// ---- for mac

BOOL soundmng_initialize(void);
void soundmng_deinitialize(void);

void soundmng_pcmload(UINT num, const char *filename, UINT type);
void soundmng_pcmvolume(UINT num, int volume);

#ifdef __cplusplus
}
#endif

