#ifdef __cplusplus
extern "C" {
#endif

void Setfiletype(int ftype, OSType *creator, OSType *fileType);
int file_getftype(char* filename);

#ifdef __cplusplus
}
#endif

