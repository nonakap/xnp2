
#if defined(ENABLE_TRAP)

#ifdef __cplusplus
extern "C" {
#endif

void CPUCALL softinttrap(UINT cs, UINT32 eip, UINT vect);

#ifdef __cplusplus
}
#endif

#endif

