
#if !defined(SUPPORT_ROMEO)

#define	juliet_initialize()		(FAILURE)
#define	juliet_deinitialize()

#define	juliet_YMF288Reset()
#define juliet_YMF288IsEnable()	(FALSE)
#define juliet_YMF288IsBusy()	(FALSE)
#define juliet_YMF288A(a, d)
#define juliet_YMF288B(a, d)
#define juliet_YMF288Enable(e)

#else

#ifdef __cplusplus
extern "C" {
#endif

BOOL juliet_initialize(void);
void juliet_deinitialize(void);

void juliet_YMF288Reset(void);
BOOL juliet_YMF288IsEnable(void);
BOOL juliet_YMF288IsBusy(void);
void juliet_YMF288A(UINT addr, UINT8 data);
void juliet_YMF288B(UINT addr, UINT8 data);
void juliet_YMF288Enable(BOOL enable);

#ifdef __cplusplus
}
#endif

#endif

