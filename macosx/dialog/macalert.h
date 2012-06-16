/*
 *  macalert.h
 *  np2
 *
 *  Created by tk800 on Fri Oct 31 2003.
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

void ResumeErrorDialogProc(void);
int ResumeWarningDialogProc(const char *string);
bool ResetWarningDialogProc(void);
bool QuitWarningDialogProc(void);

#ifdef __cplusplus
}
#endif
