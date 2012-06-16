
#ifdef __cplusplus
extern "C" {
#endif

BRESULT fddxdf_set(FDDFILE fdd, const OEMCHAR *fname, int ro);
BRESULT fddxdf_setfdi(FDDFILE fdd, const OEMCHAR *fname, int ro);
BRESULT fddxdf_eject(FDDFILE fdd);

BRESULT fddxdf_diskaccess(FDDFILE fdd);
BRESULT fddxdf_seek(FDDFILE fdd);
BRESULT fddxdf_seeksector(FDDFILE fdd);
BRESULT fddxdf_read(FDDFILE fdd);
BRESULT fddxdf_write(FDDFILE fdd);
BRESULT fddxdf_readid(FDDFILE fdd);

#ifdef __cplusplus
}
#endif

