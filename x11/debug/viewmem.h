#if defined(SUPPORT_VIEWER)

#ifdef __cplusplus
extern "C" {
#endif

void viewmem_read(VIEWMEM_T *cfg, UINT32 adrs, BYTE *buf, UINT32 size);
void viewmem_write(VIEWMEM_T *cfg, UINT32 adrs, BYTE *buf, UINT32 size);

#ifdef __cplusplus
}
#endif

#endif
