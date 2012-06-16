
#ifdef __cplusplus
extern "C" {
#endif

#if defined(_UNICODE)
UINT oemtext_sjistooem(TCHAR *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_oemtosjis(char *dst, UINT dcnt, const TCHAR *src, UINT scnt);
#endif

#ifdef __cplusplus
}
#endif

