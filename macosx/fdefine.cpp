#include	"compiler.h"

#include	"dosio.h"
#include	"fdefine.h"
#include	"strres.h"
#include	"fontdata.h"

void Setfiletype(int ftype, OSType *creator, OSType *fileType) {

	*creator = 'SMil';
//	*fileType = '????';

	switch(ftype) {
//		case FTYPE_SMIL:			// システム予約

		case FTYPE_TEXT:			// テキストファイル

			*creator = 'ttxt';
			*fileType = 'TEXT';
			break;

//		case FTYPE_BMP:				// Bitmap
//		case FTYPE_PICT:			// Picture (予約)
//		case FTYPE_PNG:				// Png (予約)
//		case FTYPE_WAV:				// Wave

		case FTYPE_D88:				// D88
			*fileType = '.D88';
			break;

		case FTYPE_BETA:			// ベタイメージ
			*fileType = 'BETA';
			break;

		case FTYPE_THD:				// .thd ハードディスクイメージ
			*fileType = '.THD';
			break;

		case FTYPE_HDI:				// .hdi ハードディスクイメージ
			*fileType = '.HDI';
			break;

		case FTYPE_HDD:				// .hdd ハードディスクイメージ (予約)
			*fileType = '.HDD';
			break;

		case FTYPE_S98:				// .s98 ハードディスクイメージ
			*fileType = '.S98';
			break;

		case FTYPE_MIMPI:			// mimpi defaultファイル
			*fileType = '.DEF';
			break;
#if 0
        case FTYPE_AIFF:
            *fileType = 'AIFF';
            *creator = 'hook';
            break;
#endif
        case FTYPE_INI:
            *fileType = 'TEXT';
            *creator = 'SMil';
            break;
	}
}

static int Getfiletype(FInfo *fndrinfo) {


	switch(fndrinfo->fdType) {
		case '.D88':
			return(FTYPE_D88);

		case 'BETA':
		case '.XDF':
		case '.DUP':
			return(FTYPE_BETA);

		case '.THD':
			return(FTYPE_THD);

		case '.HDI':
			return(FTYPE_HDI);
            
		case '.NHD':
			return(FTYPE_NHD);
            
		case '.HDD':
			return(FTYPE_HDD);
            
		case '.FDI':
			return(FTYPE_FDI);
            
		case 'BMP ':
			return(FTYPE_BMP);
	}
	return(FTYPE_NONE);
}

static int GetFileExt(char* filename) {

    char*	p;
    char*	n;
    int		ftype;
    
    p = file_getext((char *)filename);
    n = file_getname((char *)filename);
    if ((!milstr_cmp(p, str_d88)) || (!milstr_cmp(p, str_d98))) {
			ftype = FTYPE_D88;
		}
    else if ((!milstr_cmp(p, str_ini))) {
			ftype = FTYPE_INI;
		}
    else if ((!milstr_cmp(p, str_bmp))) {
			ftype = FTYPE_BMP;
		}
    else if ((!milstr_cmp(p, str_thd))) {
			ftype = FTYPE_THD;
		}
    else if ((!milstr_cmp(p, str_nhd))) {
			ftype = FTYPE_NHD;
		}
    else if ((!milstr_cmp(p, str_hdi))) {
			ftype = FTYPE_HDI;
		}
    else if ((!milstr_cmp(p, str_hdd))) {
			ftype = FTYPE_HDD;
		}
    else if ((!milstr_cmp(p, str_fdi))) {
			ftype = FTYPE_FDI;
		}
    else if ((!milstr_cmp(p, "xdf")) || (!milstr_cmp(p, "dup")) || (!milstr_cmp(p, "hdm"))) {
        ftype = FTYPE_BETA;
    }
	else if (
        (!file_cmpname(n, v98fontname))	||
        (!file_cmpname(n, pc88ankname))	||
		(!file_cmpname(n, pc88knj1name))||
		(!file_cmpname(n, pc88knj2name))||
        (!file_cmpname(n, fm7ankname))	||
		(!file_cmpname(n, fm7knjname))	||
        (!file_cmpname(n, x1ank1name)) 	||
		(!file_cmpname(n, x1ank2name))	||
		(!file_cmpname(n, x1knjname))	||
        (!file_cmpname(n, x68kfontname))) {
        ftype = FTYPE_SMIL;
	}
    else {
        ftype = FTYPE_NONE;
    }
    return(ftype);
}

int file_getftype(char* filename) {

	FSSpec	fss;
	Str255	fname;
	FInfo	fndrInfo;
    int		ftype;

    ftype = GetFileExt(filename);
	if (ftype == FTYPE_NONE) {
        mkstr255(fname, filename);
        FSMakeFSSpec(0, 0, fname, &fss);
        if (FSpGetFInfo(&fss, &fndrInfo) != noErr) {
            return(FTYPE_NONE);
        }
        ftype = Getfiletype(&fndrInfo);
    }
	return(ftype);
}
