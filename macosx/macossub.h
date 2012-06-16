#include	<QuickTime/QuickTime.h>
#include	<CoreMIDI/MIDIServices.h>
#define CFSTRj(name)	CFStringCreateWithCString(NULL,name,CFStringGetSystemEncoding())

#ifdef __cplusplus
extern "C" {
#endif

void macossub_init(void);
void macossub_term(void);

UINT32 macos_gettick(void);

void mkstr255(Str255 dst, const char *src);
void mkcstr(char *dst, int size, const Str255 src);

#ifdef __cplusplus
}
#endif


