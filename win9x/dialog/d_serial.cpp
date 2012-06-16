#include	"compiler.h"
#include	<prsht.h>
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"dosio.h"
#include	"commng.h"
#include	"sysmng.h"
#include	"np2class.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"pc9861k.h"
#include	"dipswbmp.h"


static const CBPARAM cpPort[] =
{
	{MAKEINTRESOURCE(IDS_NONCONNECT),	COMPORT_NONE},
	{MAKEINTRESOURCE(IDS_COM1),			COMPORT_COM1},
	{MAKEINTRESOURCE(IDS_COM2),			COMPORT_COM2},
	{MAKEINTRESOURCE(IDS_COM3),			COMPORT_COM3},
	{MAKEINTRESOURCE(IDS_COM4),			COMPORT_COM4},
	{MAKEINTRESOURCE(IDS_MIDI),			COMPORT_MIDI},
};

static const CBNPARAM cpChars[] =
{
	{5,	0x00},
	{6,	0x04},
	{7,	0x08},
	{8,	0x0c},
};

static const CBPARAM cpParity[] =
{
    {MAKEINTRESOURCE(IDS_PARITY_NONE),	0x00},
    {MAKEINTRESOURCE(IDS_PARITY_ODD),	0x20},
	{MAKEINTRESOURCE(IDS_PARITY_EVEN),	0x30},
};

static const CBPARAM cpSBit[] =
{
    {MAKEINTRESOURCE(IDS_1),			0x40},
    {MAKEINTRESOURCE(IDS_1HALF),		0x80},
	{MAKEINTRESOURCE(IDS_2),			0xc0},
};


#ifdef __cplusplus
extern "C" {
#endif
extern	COMMNG	cm_rs232c;
extern	COMMNG	cm_pc9861ch1;
extern	COMMNG	cm_pc9861ch2;
#ifdef __cplusplus
}
#endif


enum {
	ID_PORT		= 0,

	ID_SPEED,
	ID_CHARS,
	ID_PARITY,
	ID_SBIT,

	ID_MMAP,
	ID_MMDL,
	ID_DEFE,
	ID_DEFF,
	ID_DEFB,

	ID_MAX
};

typedef struct {
	UINT16		idc[ID_MAX];
	COMMNG		*cm;
	COMCFG		*cfg;
const UINT16	*com_item;
const UINT16	*midi_item;
	UINT		update;
} DLGCOM_P;


static const UINT16 com1serial[8] = {
			IDC_COM1STR00, IDC_COM1STR01, IDC_COM1STR02, IDC_COM1STR03,
			IDC_COM1STR04, IDC_COM1STR05, IDC_COM1STR06, IDC_COM1STR07};

static const UINT16 com1rsmidi[3] = {
			IDC_COM1STR10, IDC_COM1STR11, IDC_COM1STR12};

static const DLGCOM_P res_com1 =
		{{	IDC_COM1PORT,
			IDC_COM1SPEED, IDC_COM1CHARSIZE, IDC_COM1PARITY, IDC_COM1STOPBIT,
			IDC_COM1MMAP, IDC_COM1MMDL,
			IDC_COM1DEFE, IDC_COM1DEFF, IDC_COM1DEFB},
			&cm_rs232c, &np2oscfg.com1,
			com1serial, com1rsmidi, SYS_UPDATESERIAL1};


static const UINT16 com2serial[8] = {
			IDC_COM2STR00, IDC_COM2STR01, IDC_COM2STR02, IDC_COM2STR03,
			IDC_COM2STR04, IDC_COM2STR05, IDC_COM2STR06, IDC_COM2STR07};

static const UINT16 com2rsmidi[3] = {
			IDC_COM2STR10, IDC_COM2STR11, IDC_COM2STR12};

static const DLGCOM_P res_com2 =
		{{	IDC_COM2PORT,
			IDC_COM2SPEED, IDC_COM2CHARSIZE, IDC_COM2PARITY, IDC_COM2STOPBIT,
			IDC_COM2MMAP, IDC_COM2MMDL,
			IDC_COM2DEFE, IDC_COM2DEFF, IDC_COM2DEFB},
			&cm_pc9861ch1, &np2oscfg.com2,
			com2serial, com2rsmidi, SYS_UPDATESERIAL1};


static const UINT16 com3serial[8] = {
			IDC_COM3STR00, IDC_COM3STR01, IDC_COM3STR02, IDC_COM3STR03,
			IDC_COM3STR04, IDC_COM3STR05, IDC_COM3STR06, IDC_COM3STR07};

static const UINT16 com3rsmidi[3] = {
			IDC_COM3STR10, IDC_COM3STR11, IDC_COM3STR12};

static const DLGCOM_P res_com3 =
		{{	IDC_COM3PORT,
			IDC_COM3SPEED, IDC_COM3CHARSIZE, IDC_COM3PARITY, IDC_COM3STOPBIT,
			IDC_COM3MMAP, IDC_COM3MMDL,
			IDC_COM3DEFE, IDC_COM3DEFF, IDC_COM3DEFB},
			&cm_pc9861ch2, &np2oscfg.com3,
			com3serial, com3rsmidi, SYS_UPDATESERIAL1};


static void dlgcom_show(HWND hWnd, int ncmd, const UINT16 *item, int items) {

	while(items--) {
		ShowWindow(GetDlgItem(hWnd, *item++), ncmd);
	}
}

static void dlgcom_items(HWND hWnd, const DLGCOM_P *m, UINT r) {

	int		ncmd;

	ncmd = (((r >= 1) && (r <= 4))?SW_SHOW:SW_HIDE);
	dlgcom_show(hWnd, ncmd, m->com_item, 8);
	dlgcom_show(hWnd, ncmd, m->idc + 1, 4);

	ncmd = ((r == 5)?SW_SHOW:SW_HIDE);
	dlgcom_show(hWnd, ncmd, m->midi_item, 3);
	dlgcom_show(hWnd, ncmd, m->idc + 5, 5);
}


static void setChars(HWND hWnd, UINT uID, UINT8 cValue)
{
	dlgs_setcbcur(hWnd, uID, cValue & 0x0c);
}

static UINT8 getChars(HWND hWnd, UINT uID, UINT8 cDefault)
{
	return dlgs_getcbcur(hWnd, uID, cDefault & 0x0c);
}


static void setParity(HWND hWnd, UINT uID, UINT8 cValue)
{
	cValue = cValue & 0x30;
	if (!(cValue & 0x20))
	{
		cValue = 0;
	}
	dlgs_setcbcur(hWnd, uID, cValue);
}

static UINT8 getParity(HWND hWnd, UINT uID, UINT8 cDefault)
{
	return dlgs_getcbcur(hWnd, uID, cDefault & 0x30);
}


static void setStopBit(HWND hWnd, UINT uID, UINT8 cValue)
{
	cValue = cValue & 0xc0;
	if (!cValue)
	{
		cValue = 0x40;
	}
	dlgs_setcbcur(hWnd, uID, cValue);
}

static UINT8 getStopBit(HWND hWnd, UINT uID, UINT8 cDefault)
{
	return dlgs_getcbcur(hWnd, uID, cDefault & 0xc0);
}






static LRESULT CALLBACK dlgitem_proc(HWND hWnd, UINT msg,
								WPARAM wp, LPARAM lp, const DLGCOM_P *m) {

	UINT32	d;
	UINT8	b;
	LRESULT	r;
	union {
		OEMCHAR	mmap[MAXPNAMELEN];
		OEMCHAR	mmdl[64];
		OEMCHAR	mdef[MAX_PATH];
	} str;
	COMCFG	*cfg;
	UINT	update;
	COMMNG	cm;

	switch (msg) {
		case WM_INITDIALOG:
			cfg = m->cfg;
			dlgs_setcbitem(hWnd, m->idc[ID_PORT], cpPort, NELEMENTS(cpPort));
			SETLISTUINT32(hWnd, m->idc[ID_SPEED], cmserial_speed);
			dlgs_setcbnumber(hWnd, m->idc[ID_CHARS],
											cpChars, NELEMENTS(cpChars));
			dlgs_setcbitem(hWnd, m->idc[ID_PARITY],
											cpParity, NELEMENTS(cpParity));
			dlgs_setcbitem(hWnd, m->idc[ID_SBIT], cpSBit, NELEMENTS(cpSBit));
			for (d=0; d<(NELEMENTS(cmserial_speed) - 1); d++) {
				if (cmserial_speed[d] >= cfg->speed) {
					break;
				}
			}
			SendDlgItemMessage(hWnd, m->idc[ID_SPEED],
										CB_SETCURSEL, (WPARAM)d, (LPARAM)0);

			b = cfg->param;
			d = (b >> 2) & 3;

			setChars(hWnd, m->idc[ID_CHARS], b);
			setParity(hWnd, m->idc[ID_PARITY], b);
			setStopBit(hWnd, m->idc[ID_SBIT], b);

			dlgs_setlistmidiout(hWnd, m->idc[ID_MMAP], cfg->mout);
			SETLISTSTR(hWnd, m->idc[ID_MMDL], cmmidi_mdlname);
			SetDlgItemText(hWnd, m->idc[ID_MMDL], cfg->mdl);
			SetDlgItemCheck(hWnd, m->idc[ID_DEFE], cfg->def_en);
			SetDlgItemText(hWnd, m->idc[ID_DEFF], cfg->def);

			d = cfg->port;
			if (d >= NELEMENTS(cpPort))
			{
				d = 0;
			}
			dlgs_setcbcur(hWnd, m->idc[ID_PORT], d);
			dlgcom_items(hWnd, m, d);
			return(TRUE);

		case WM_COMMAND:
			if (LOWORD(wp) == m->idc[ID_PORT])
			{
				dlgcom_items(hWnd, m,
						dlgs_getcbcur(hWnd, m->idc[ID_PORT], COMPORT_NONE));
			}
			else if (LOWORD(wp) == m->idc[ID_DEFB])
			{
				dlgs_browsemimpidef(hWnd, m->idc[ID_DEFF]);
			}
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY)
			{
				cfg = m->cfg;
				update = 0;
				r = dlgs_getcbcur(hWnd, m->idc[ID_PORT], COMPORT_NONE);
				if (cfg->port != (UINT8)r)
				{
					cfg->port = (UINT8)r;
					update |= SYS_UPDATEOSCFG;
					update |= m->update;
				}
				r = SendDlgItemMessage(hWnd, m->idc[ID_SPEED],
										CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				if (r != CB_ERR) {
					if ((UINT)r >= NELEMENTS(cmserial_speed)) {
						r = 0;
					}
					if (cfg->speed != cmserial_speed[r]) {
						cfg->speed = cmserial_speed[r];
						update |= SYS_UPDATEOSCFG;
						update |= m->update;
					}
				}

				b = 0;
				b |= getChars(hWnd, m->idc[ID_CHARS], cfg->param);
				b |= getParity(hWnd, m->idc[ID_PARITY], cfg->param);
				b |= getStopBit(hWnd, m->idc[ID_SBIT], cfg->param);

				if (cfg->param != b) {
					cfg->param = b;
					update |= SYS_UPDATEOSCFG;
					update |= m->update;
				}

				GetDlgItemText(hWnd, m->idc[ID_MMAP],
											str.mmap, NELEMENTS(str.mmap));
				if (milstr_cmp(cfg->mout, str.mmap)) {
					milstr_ncpy(cfg->mout, str.mmap, NELEMENTS(cfg->mout));
					update |= SYS_UPDATEOSCFG;
					update |= m->update;
				}
				GetDlgItemText(hWnd, m->idc[ID_MMDL],
											str.mmdl, NELEMENTS(str.mmdl));
				if (milstr_cmp(cfg->mdl, str.mmdl)) {
					milstr_ncpy(cfg->mdl, str.mmdl, NELEMENTS(cfg->mdl));
					update |= SYS_UPDATEOSCFG;
					update |= m->update;
				}

				cfg->def_en = (UINT8)GetDlgItemCheck(hWnd, m->idc[ID_DEFE]);
				cm = *m->cm;
				if (cm) {
					cm->msg(cm, COMMSG_MIMPIDEFEN, cfg->def_en);
				}
				GetDlgItemText(hWnd, m->idc[ID_DEFF],
											str.mdef, NELEMENTS(str.mdef));
				if (milstr_cmp(cfg->def, str.mdef)) {
					milstr_ncpy(cfg->def, str.mdef, NELEMENTS(cfg->def));
					update |= SYS_UPDATEOSCFG;
					if (cm) {
						cm->msg(cm, COMMSG_MIMPIDEFFILE, (long)str.mdef);
					}
				}
				sysmng_update(update);
				return(TRUE);
			}
			break;
	}
	return(FALSE);
}


// ---------------------------------------------------------------------------

static LRESULT CALLBACK Com1Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	return(dlgitem_proc(hWnd, msg, wp, lp, &res_com1));
}

static LRESULT CALLBACK Com2Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	return(dlgitem_proc(hWnd, msg, wp, lp, &res_com2));
}

static LRESULT CALLBACK Com3Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	return(dlgitem_proc(hWnd, msg, wp, lp, &res_com3));
}



// --------------------------------------------------------------------

static	UINT8	pc9861_s[3];
static	UINT8	pc9861_j[6];

typedef struct {
	int		idc_speed;
	int		idc_int;
	int		idc_mode;
	UINT8	*dip_mode;
	UINT8	*dip_int;
	UINT8	sft_int;
} PC9861MODE_T;

static const PC9861MODE_T pc9861mode[2] = {
			{IDC_CH1SPEED, IDC_CH1INT, IDC_CH1MODE,
								&pc9861_s[0], &pc9861_s[1], 0},
			{IDC_CH2SPEED, IDC_CH2INT, IDC_CH2MODE,
								&pc9861_s[2], &pc9861_s[1], 2}};

enum {
	PC9861S1_X		= 1,
	PC9861S2_X		= 10,
	PC9861S3_X		= 17,
	PC9861S_Y		= 1,

	PC9861J1_X		= 1,
	PC9861J2_X		= 9,
	PC9861J3_X		= 17,
	PC9861J4_X		= 1,
	PC9861J5_X		= 11,
	PC9861J6_X		= 19,
	PC9861J1_Y		= 4,
	PC9861J4_Y		= 7
};

static const CBNPARAM cpInt1[] =
{
	{0,	0x00},
	{1,	0x02},
	{2,	0x01},
	{3,	0x03},
};

static const CBNPARAM cpInt2[] =
{
	{0,	0x00},
	{4,	0x08},
	{5,	0x04},
	{6,	0x0c},
};

static const CBPARAM cpSync[] =
{
	{MAKEINTRESOURCE(IDS_SYNC),		0x03},
	{MAKEINTRESOURCE(IDS_ASYNC),	0x00},
	{MAKEINTRESOURCE(IDS_ASYNC16X),	0x01},
	{MAKEINTRESOURCE(IDS_ASYNC64X),	0x02},
};

static void pc9861setspeed(HWND hWnd, const PC9861MODE_T *m)
{
	UINT8	cMode;
	UINT	uSpeed;

	cMode = *(m->dip_mode);
	uSpeed = (((~cMode) >> 2) & 0x0f) + 1;
	if (cMode)
	{
		if (uSpeed > 4)
		{
			uSpeed -= 4;
		}
		else
		{
			uSpeed = 0;
		}
	}
	if (uSpeed > (NELEMENTS(pc9861k_speed) - 1))
	{
		uSpeed = NELEMENTS(pc9861k_speed) - 1;
	}

	SendDlgItemMessage(hWnd, m->idc_speed,
								CB_SETCURSEL, (WPARAM)uSpeed, (LPARAM)0);
}

static void pc9861getspeed(HWND hWnd, const PC9861MODE_T *m)
{
	UINT8	cMode;
	LRESULT	r;
	UINT	uSpeed;

	cMode = *(m->dip_mode);
	r = SendDlgItemMessage(hWnd, m->idc_speed, CB_GETCURSEL, 0, 0);
	if (r != CB_ERR)
	{
		uSpeed = (UINT)r;
		if (uSpeed > (NELEMENTS(pc9861k_speed) - 1))
		{
			uSpeed = NELEMENTS(pc9861k_speed) - 1;
		}
		if (cMode & 2)
		{
			uSpeed += 3;
		}
		else
		{
			if (uSpeed)
			{
				uSpeed--;
			}
		}
		cMode &= 3;
		cMode |= ((~uSpeed) & 0x0f) << 2;
		*(m->dip_mode) = cMode;
	}
}

static void pc9861setsync(HWND hWnd, const PC9861MODE_T *m)
{
	UINT8	cMode;

	cMode = *(m->dip_mode);
	dlgs_setcbcur(hWnd, m->idc_mode, cMode & 0x03);
}

static void pc9861getsync(HWND hWnd, const PC9861MODE_T *m)
{
	UINT8	cMode;
	UINT8	cNewMode;

	cMode = *(m->dip_mode);
	cNewMode = (UINT8)dlgs_getcbcur(hWnd, m->idc_mode, cMode & 0x03);
	*(m->dip_mode) = (UINT8)((cMode & (~3)) | cNewMode);
}

static void pc9861setint(HWND hWnd, const PC9861MODE_T *m)
{
	UINT8	cMask;
	UINT8	cMode;

	cMask = 3 << (m->sft_int);
	cMode = *(m->dip_int);
	dlgs_setcbcur(hWnd, m->idc_int, cMode & cMask);
}

static void pc9861getint(HWND hWnd, const PC9861MODE_T *m)
{
	UINT8	cMask;
	UINT8	cMode;
	UINT8	cNewMode;

	cMask = 3 << (m->sft_int);
	cMode = *(m->dip_int);
	cNewMode = (UINT8)dlgs_getcbcur(hWnd, m->idc_int, cMode & cMask);
	*(m->dip_int) = (cMode & (~cMask)) | cNewMode;
}

static void pc9861setmode(HWND hWnd, const PC9861MODE_T *m)
{
	pc9861setspeed(hWnd, m);
	pc9861setint(hWnd, m);
	pc9861setsync(hWnd, m);
}

static void pc9861cmddipsw(HWND hWnd) {

	RECT	rect1;
	RECT	rect2;
	POINT	p;
	UINT8	bit;

	GetWindowRect(GetDlgItem(hWnd, IDC_PC9861DIP), &rect1);
	GetClientRect(GetDlgItem(hWnd, IDC_PC9861DIP), &rect2);
	GetCursorPos(&p);
	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 9;
	p.y /= 9;
	if ((p.y >= 1) && (p.y < 3)) {					// 1’i–Ú
		if ((p.x >= 1) && (p.x < 7)) {				// S1
			pc9861_s[0] ^= (1 << (p.x - 1));
			pc9861setmode(hWnd, pc9861mode);
		}
		else if ((p.x >= 10) && (p.x < 14)) {		// S2
			pc9861_s[1] ^= (1 << (p.x - 10));
			pc9861setint(hWnd, pc9861mode);
			pc9861setint(hWnd, pc9861mode+1);
		}
		else if ((p.x >= 17) && (p.x < 23)) {		// S3
			pc9861_s[2] ^= (1 << (p.x - 17));
			pc9861setmode(hWnd, pc9861mode+1);
		}
	}
	else if ((p.y >= 4) && (p.y < 6)) {				// 2’i–Ú
		if ((p.x >= 1) && (p.x < 7)) {				// J1
			pc9861_j[0] ^= (1 << (p.x - 1));
		}
		else if ((p.x >= 9) && (p.x < 15)) {		// J2
			pc9861_j[1] ^= (1 << (p.x - 9));
		}
		else if ((p.x >= 17) && (p.x < 19)) {		// J3
			pc9861_j[2] = (1 << (p.x - 17));
		}
	}
	else if ((p.y >= 7) && (p.y < 9)) {				// 3’i–Ú
		if ((p.x >= 1) && (p.x < 9)) {				// J4
			bit = (1 << (p.x - 1));
			if (pc9861_j[3] == bit) {
				bit = 0;
			}
			pc9861_j[3] = bit;
		}
		else if ((p.x >= 11) && (p.x < 17)) {		// J5
			pc9861_j[4] ^= (1 << (p.x - 11));
		}
		else if ((p.x >= 19) && (p.x < 25)) {		// J6
			pc9861_j[5] ^= (1 << (p.x - 19));
		}
	}
}

static LRESULT CALLBACK pc9861mainProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	HWND	sub;
	UINT8	r;
	UINT	update;

	switch (msg) {
		case WM_INITDIALOG:
			CopyMemory(pc9861_s, np2cfg.pc9861sw, 3);
			CopyMemory(pc9861_j, np2cfg.pc9861jmp, 6);
			SETLISTUINT32(hWnd, IDC_CH1SPEED, pc9861k_speed);
			SETLISTUINT32(hWnd, IDC_CH2SPEED, pc9861k_speed);
			dlgs_setcbnumber(hWnd, IDC_CH1INT, cpInt1, NELEMENTS(cpInt1));
			dlgs_setcbnumber(hWnd, IDC_CH2INT, cpInt2, NELEMENTS(cpInt2));
			dlgs_setcbitem(hWnd, IDC_CH1MODE, cpSync, NELEMENTS(cpSync));
			dlgs_setcbitem(hWnd, IDC_CH2MODE, cpSync, NELEMENTS(cpSync));

			SendDlgItemMessage(hWnd, IDC_PC9861E, BM_GETCHECK,
												np2cfg.pc9861enable & 1, 0);
			pc9861setmode(hWnd, pc9861mode);
			pc9861setmode(hWnd, pc9861mode+1);

			sub = GetDlgItem(hWnd, IDC_PC9861DIP);
			SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
							(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));
			return(TRUE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDC_CH1SPEED:
					pc9861getspeed(hWnd, pc9861mode);
					pc9861setspeed(hWnd, pc9861mode);
					break;

				case IDC_CH1INT:
					pc9861getint(hWnd, pc9861mode);
					pc9861setint(hWnd, pc9861mode);
					break;

				case IDC_CH1MODE:
					pc9861getsync(hWnd, pc9861mode);
					pc9861setsync(hWnd, pc9861mode);
					break;

				case IDC_CH2SPEED:
					pc9861getspeed(hWnd, pc9861mode+1);
					pc9861setspeed(hWnd, pc9861mode+1);
					break;

				case IDC_CH2INT:
					pc9861getint(hWnd, pc9861mode+1);
					pc9861setint(hWnd, pc9861mode+1);
					break;

				case IDC_CH2MODE:
					pc9861getsync(hWnd, pc9861mode+1);
					pc9861setsync(hWnd, pc9861mode+1);
					break;

				case IDC_PC9861DIP:
					pc9861cmddipsw(hWnd);
					break;

				default:
					return(FALSE);
			}
			InvalidateRect(GetDlgItem(hWnd, IDC_PC9861DIP), NULL, TRUE);
			break;

		case WM_NOTIFY:
			if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
				update = 0;
				r = GetDlgItemCheck(hWnd, IDC_PC9861E);
				if (np2cfg.pc9861enable != r) {
					np2cfg.pc9861enable = r;
					update |= SYS_UPDATECFG;
				}
				if (memcmp(np2cfg.pc9861sw, pc9861_s, 3)) {
					CopyMemory(np2cfg.pc9861sw, pc9861_s, 3);
					update |= SYS_UPDATECFG;
				}
				if (memcmp(np2cfg.pc9861jmp, pc9861_j, 6)) {
					CopyMemory(np2cfg.pc9861jmp, pc9861_j, 6);
					update |= SYS_UPDATECFG;
				}
				sysmng_update(update);
				return(TRUE);
			}
			break;

		case WM_DRAWITEM:
			if (LOWORD(wp) == IDC_PC9861DIP) {
				dlgs_drawbmp(((LPDRAWITEMSTRUCT)lp)->hDC,
										dipswbmp_get9861(pc9861_s, pc9861_j));
			}
			return(FALSE);
	}
	return(FALSE);
}


// --------------------------------------------------------------------------

void dialog_serial(HWND hWnd)
{
	HINSTANCE		hInstance;
	PROPSHEETPAGE	psp;
	PROPSHEETHEADER	psh;
	HPROPSHEETPAGE	hpsp[4];
	TCHAR			szTitle[128];

	hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

	ZeroMemory(&psp, sizeof(psp));
	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.dwFlags = PSP_DEFAULT;
	psp.hInstance = hInstance;

	psp.pszTemplate = MAKEINTRESOURCE(IDD_SERIAL1);
	psp.pfnDlgProc = (DLGPROC)Com1Proc;
	hpsp[0] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_PC9861A);
	psp.pfnDlgProc = (DLGPROC)pc9861mainProc;
	hpsp[1] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_PC9861B);
	psp.pfnDlgProc = (DLGPROC)Com2Proc;
	hpsp[2] = CreatePropertySheetPage(&psp);

	psp.pszTemplate = MAKEINTRESOURCE(IDD_PC9861C);
	psp.pfnDlgProc = (DLGPROC)Com3Proc;
	hpsp[3] = CreatePropertySheetPage(&psp);

	loadstringresource(IDS_SERIALOPTION, szTitle, NELEMENTS(szTitle));

	ZeroMemory(&psh, sizeof(psh));
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
	psh.hwndParent = hWnd;
	psh.hInstance = hInstance;
	psh.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	psh.nPages = 4;
	psh.phpage = hpsp;
	psh.pszCaption = szTitle;
	psh.pfnCallback = np2class_propetysheet;
	PropertySheet(&psh);
	InvalidateRect(hWnd, NULL, TRUE);
}

