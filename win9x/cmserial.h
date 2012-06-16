
// ---- com manager serial for windows

#ifdef __cplusplus
extern "C" {
#endif

extern const UINT32 cmserial_speed[10];

COMMNG cmserial_create(UINT port, UINT8 param, UINT32 speed);

#ifdef __cplusplus
}
#endif

