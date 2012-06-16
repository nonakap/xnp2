
#ifdef __cplusplus
extern "C" {
#endif

void memmng_initialize(UINT32 addr, UINT size);
void memmng_deinitialize(void);
void *memmng_alloc(UINT size);
void memmng_free(void *addr);

#ifdef __cplusplus
}
#endif

