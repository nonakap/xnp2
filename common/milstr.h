
#ifndef STRCALL
#define	STRCALL
#endif


#ifdef __cplusplus
extern "C" {
#endif

// �P�������̃T�C�Y���擾
int STRCALL milank_charsize(const OEMCHAR *str);
int STRCALL milsjis_charsize(const char *str);
int STRCALL mileuc_charsize(const char *str);
int STRCALL milutf8_charsize(const char *str);

// �啶���������𓯈ꎋ���Ĕ�r
// ret 0:��v
int STRCALL milank_cmp(const OEMCHAR *str, const OEMCHAR *cmp);
int STRCALL milsjis_cmp(const char *str, const char *cmp);
int STRCALL mileuc_cmp(const char *str, const char *cmp);
int STRCALL milutf8_cmp(const char *str, const char *cmp);

// �啶���������� ���ꎋ����cmp�̃k���܂Ŕ�r
// ret 0:��v
int STRCALL milank_memcmp(const OEMCHAR *str, const OEMCHAR *cmp);
int STRCALL milsjis_memcmp(const char *str, const char *cmp);
int STRCALL mileuc_memcmp(const char *str, const char *cmp);
int STRCALL milutf8_memcmp(const char *str, const char *cmp);

// str[pos]�������P�o�C�g�ڂ��ǂ����c
int STRCALL milsjis_kanji1st(const char *str, int pos);
int STRCALL mileuc_kanji1st(const char *str, int pos);
int STRCALL milutf8_kanji1st(const char *str, int pos);

// str[pos]�������Q�o�C�g�ڂ��ǂ����c
int STRCALL milsjis_kanji2nd(const char *str, int pos);
int STRCALL mileuc_kanji2nd(const char *str, int pos);
int STRCALL milutf8_kanji2nd(const char *str, int pos);

// maxlen��������������R�s�[
void STRCALL milank_ncpy(OEMCHAR *dst, const OEMCHAR *src, UINT maxlen);
void STRCALL milsjis_ncpy(char *dst, const char *src, UINT maxlen);
void STRCALL mileuc_ncpy(char *dst, const char *src, UINT maxlen);
void STRCALL milutf8_ncpy(char *dst, const char *src, UINT maxlen);

// maxlen��������������L���b�g
void STRCALL milank_ncat(OEMCHAR *dst, const OEMCHAR *src, UINT maxlen);
void STRCALL milsjis_ncat(char *dst, const char *src, UINT maxlen);
void STRCALL mileuc_ncat(char *dst, const char *src, UINT maxlen);
void STRCALL milutf8_ncat(char *dst, const char *src, UINT maxlen);

// ����������
OEMCHAR * STRCALL milank_chr(const OEMCHAR *str, int c);
char * STRCALL milsjis_chr(const char *str, int c);
char * STRCALL mileuc_chr(const char *str, int c);
char * STRCALL milutf8_chr(const char *str, int c);


// 0~9, A~Z �݂̂�啶���������𓯈ꎋ���Ĕ�r
// ret 0:��v
int STRCALL milstr_extendcmp(const OEMCHAR *str, const OEMCHAR *cmp);

// ���̌���擾
OEMCHAR * STRCALL milstr_nextword(const OEMCHAR *str);

// �����񂩂�ARG�̎擾
UINT STRCALL milstr_getarg(OEMCHAR *str, OEMCHAR *arg[], UINT maxarg);

// HEX2INT
long STRCALL milstr_solveHEX(const OEMCHAR *str);

// STR2INT
long STRCALL milstr_solveINT(const OEMCHAR *str);

// STRLIST
OEMCHAR * STRCALL milstr_list(const OEMCHAR *lststr, UINT pos);

#ifdef __cplusplus
}
#endif


// ---- macros

#if defined(OSLANG_SJIS)
#define	milstr_charsize(s)		milsjis_charsize(s)
#define	milstr_cmp(s, c)		milsjis_cmp(s, c)
#define	milstr_memcmp(s, c)		milsjis_memcmp(s, c)
#define	milstr_kanji1st(s, p)	milsjis_kanji1st(s, p)
#define	milstr_kanji2nd(s, p)	milsjis_kanji2nd(s, p)
#define	milstr_ncpy(d, s, l)	milsjis_ncpy(d, s, l)
#define	milstr_ncat(d, s, l)	milsjis_ncat(d, s, l)
#define	milstr_chr(s, c)		milsjis_chr(s, c)
#elif defined(OSLANG_EUC)
#define	milstr_charsize(s)		mileuc_charsize(s)
#define	milstr_cmp(s, c)		mileuc_cmp(s, c)
#define	milstr_memcmp(s, c)		mileuc_memcmp(s, c)
#define	milstr_kanji1st(s, p)	mileuc_kanji1st(s, p)
#define	milstr_kanji2nd(s, p)	mileuc_kanji2nd(s, p)
#define	milstr_ncpy(d, s, l)	mileuc_ncpy(d, s, l)
#define	milstr_ncat(d, s, l)	mileuc_ncat(d, s, l)
#define	milstr_chr(s, c)		mileuc_chr(s, c)
#elif defined(OSLANG_UTF8)
#define	milstr_charsize(s)		milutf8_charsize(s)
#define	milstr_cmp(s, c)		milutf8_cmp(s, c)
#define	milstr_memcmp(s, c)		milutf8_memcmp(s, c)
#define	milstr_kanji1st(s, p)	milutf8_kanji1st(s, p)
#define	milstr_kanji2nd(s, p)	milutf8_kanji2nd(s, p)
#define	milstr_ncpy(d, s, l)	milutf8_ncpy(d, s, l)
#define	milstr_ncat(d, s, l)	milutf8_ncat(d, s, l)
#define	milstr_chr(s, c)		milutf8_chr(s, c)
#else
#define	milstr_charsize(s)		milank_charsize(s)
#define	milstr_cmp(s, c)		milank_cmp(s, c)
#define	milstr_memcmp(s, c)		milank_memcmp(s, c)
#define	milstr_kanji1st(s, p)	(0)
#define	milstr_kanji2nd(s, p)	(0)
#define	milstr_ncpy(d, s, l)	milank_ncpy(d, s, l)
#define	milstr_ncat(d, s, l)	milank_ncat(d, s, l)
#define	milstr_chr(s, c)		milank_chr(s, c)
#endif

