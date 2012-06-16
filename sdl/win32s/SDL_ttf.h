
#define	TTF_Font	void

#ifdef __cplusplus
extern "C" {
#endif

int TTF_Init(void);
TTF_Font *TTF_OpenFont(const char *file, int ptsize);
void TTF_CloseFont(TTF_Font *font);
SDL_Surface *TTF_RenderUNICODE_Solid(TTF_Font *font, const WORD *text,
															SDL_Color fg);

#ifdef __cplusplus
}
#endif

