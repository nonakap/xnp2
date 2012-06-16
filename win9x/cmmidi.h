
// ---- com manager midi for windows

#ifdef __cplusplus
extern "C" {
#endif

extern const OEMCHAR cmmidi_midimapper[];
#if defined(VERMOUTH_LIB)
extern const OEMCHAR cmmidi_vermouth[];
#endif
#if defined(MT32SOUND_DLL)
extern const OEMCHAR cmmidi_mt32sound[];
#endif
extern const OEMCHAR *cmmidi_mdlname[12];

void cmmidi_initailize(void);
COMMNG cmmidi_create(const OEMCHAR *midiout, const OEMCHAR *midiin,
													const OEMCHAR *module);
void cmmidi_recvdata(HMIDIIN hdr, UINT32 data);
void cmmidi_recvexcv(HMIDIIN hdr, MIDIHDR *data);

#ifdef __cplusplus
}
#endif

