
#ifdef __cplusplus
extern "C" {
#endif

BRESULT fddd88_set(FDDFILE fdd, const OEMCHAR *fname, int ro);
BRESULT fddd88_eject(FDDFILE fdd);

BRESULT fdd_diskaccess_d88(void);
BRESULT fdd_seek_d88(void);
BRESULT fdd_seeksector_d88(void);
BRESULT fdd_read_d88(void);
BRESULT fdd_write_d88(void);
BRESULT fdd_diagread_d88(void);
BRESULT fdd_readid_d88(void);
BRESULT fdd_writeid_d88(void);

BRESULT fdd_formatinit_d88(void);
BRESULT fdd_formating_d88(const UINT8 *ID);
BOOL fdd_isformating_d88(void);

#ifdef __cplusplus
}
#endif

