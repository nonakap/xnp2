
enum {
//	AUDIO_U8		= 0x0008,
//	AUDIO_S8		= 0x8008,
//	AUDIO_U16LSB	= 0x0010,
	AUDIO_S16LSB	= 0x8010,
//	AUDIO_U16MSB	= 0x1010,
//	AUDIO_S16MSB	= 0x9010,
//	AUDIO_U16		= AUDIO_U16LSB,
	AUDIO_S16		= AUDIO_S16LSB,
//	AUDIO_U16SYS	= AUDIO_U16LSB,
	AUDIO_S16SYS	= AUDIO_S16LSB
};

#define	SDL_MIX_MAXVOLUME	128

typedef struct {
	int		freq;
	WORD	format;
	BYTE	channels;
	BYTE	silence;
	WORD	samples;
	WORD	padding;
	DWORD	size;
	void	(*callback)(void *userdata, BYTE *stream, int len);
	void	*userdata;
} SDL_AudioSpec;


#ifdef __cplusplus
extern "C" {
#endif

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
void SDL_CloseAudio(void);
void SDL_PauseAudio(int pause_on);
void SDL_MixAudio(BYTE *dst, const BYTE *src, DWORD len, int volume);

#ifdef __cplusplus
}
#endif

