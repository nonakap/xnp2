
#ifdef __cplusplus
extern "C" {
#endif

void newdisk_fdd(const OEMCHAR *fname, REG8 type, const OEMCHAR *label);

void newdisk_thd(const OEMCHAR *fname, UINT hddsize);
void newdisk_nhd(const OEMCHAR *fname, UINT hddsize);
void newdisk_hdi(const OEMCHAR *fname, UINT hddtype);
void newdisk_vhd(const OEMCHAR *fname, UINT hddsize);

#ifdef __cplusplus
}
#endif

