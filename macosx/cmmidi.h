
// ---- com manager midi for Mac OS X

#ifdef __cplusplus
extern "C" {
#endif

extern const char cmmidi_midimapper[];
#if defined(VERMOUTH_LIB)
extern const char cmmidi_vermouth[];
#endif
extern const char *cmmidi_mdlname[12];

void cmmidi_initailize(void);
COMMNG cmmidi_create(const char *midiout, const char *midiin,
														const char *module);
#if 0
void cmmidi_recvdata(HMIDIIN hdr, UINT32 data);
void cmmidi_recvexcv(HMIDIIN hdr, MIDIHDR *data);
#endif

#ifdef __cplusplus
}
#endif

