/*
 *  np2opening.h
 *  np2
 *
 *  Created by tk800 on Fri Oct 31 2003.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

void openingNP2(void);
PicHandle getBMPfromPath(char* path, Rect* srt);
PicHandle getBMPfromResource(const char* name, Rect* srt);
bool getResourceFile(const char* name, FSSpec* fsc);

#ifdef __cplusplus
}
#endif


