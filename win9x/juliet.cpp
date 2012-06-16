#include	"compiler.h"

#if defined(SUPPORT_ROMEO)

#include	"romeo.h"
#include	"juliet.h"


enum {
	ROMEO_AVAIL			= 0x01,
	ROMEO_YMF288		= 0x02,
	ROMEO_YM2151		= 0x04
};

typedef struct {
	HMODULE	mod;

	ULONG	(WINAPI * finddev)(ULONG ven, ULONG dev, ULONG index);
	ULONG	(WINAPI * read32)(ULONG pciaddr, ULONG regaddr);
	void	(WINAPI * out8)(ULONG addr, UCHAR param);
	void	(WINAPI * out16)(ULONG addr, USHORT param);
	void	(WINAPI * out32)(ULONG addr, ULONG param);
	UCHAR	(WINAPI * in8)(ULONG addr);
	USHORT	(WINAPI * in16)(ULONG addr);
	ULONG	(WINAPI * in32)(ULONG addr);

	ULONG	addr;
	ULONG	irq;
	ULONG	avail;

	UINT8	algo[8];
	UINT8	ttl[8*4];
	UINT8	psgmix;
} _ROMEO;


static const UINT8 opmask[] = {0x08,0x08,0x08,0x08,0x0c,0x0e,0x0e,0x0f};

typedef struct {
const char	*symbol;
	UINT	addr;
} DLLPROC;

static const DLLPROC dllproc[] = {
				{FN_PCIFINDDEV,		offsetof(_ROMEO, finddev)},
				{FN_PCICFGREAD32,	offsetof(_ROMEO, read32)},
				{FN_PCIMEMWR8,		offsetof(_ROMEO, out8)},
				{FN_PCIMEMWR16,		offsetof(_ROMEO, out16)},
				{FN_PCIMEMWR32,		offsetof(_ROMEO, out32)},
				{FN_PCIMEMRD8,		offsetof(_ROMEO, in8)},
				{FN_PCIMEMRD16,		offsetof(_ROMEO, in16)},
				{FN_PCIMEMRD32,		offsetof(_ROMEO, in32)}};


static	_ROMEO		romeo = {NULL};



// pciFindPciDevice使うと、OS起動後一発目に見つけられないことが多いので、
// 自前で検索する（矢野さん方式）

#define PCIBUSDEVFUNC(b, d, f)	(((b) << 8) | ((d) << 3) | (f))
#define	DEVVEND(v, d)			((ULONG)((v) | ((d) << 16)))

static ULONG searchRomeo(void) {

	UINT	bus;
	UINT	dev;
	UINT	func;
	ULONG	addr;
	ULONG	dev_vend;

	for (bus=0; bus<0x100; bus++) {
		for (dev=0; dev<0x20; dev++) {
			for (func=0; func<0x08; func++) {
				addr = PCIBUSDEVFUNC(bus, dev, func);
				dev_vend = romeo.read32(addr, 0x0000);
				if ((dev_vend == DEVVEND(ROMEO_VENDORID, ROMEO_DEVICEID)) ||
					(dev_vend == DEVVEND(ROMEO_VENDORID, ROMEO_DEVICEID2))) {
					return(addr);
				}
			}
		}
	}
	return((ULONG)0xffffffff);
}



BOOL juliet_initialize(void) {

	HMODULE		mod;
const DLLPROC	*d;
const DLLPROC	*dterm;
	long		proc;
	ULONG		pciaddr;

	juliet_deinitialize();

	mod = LoadLibrary(PCIDEBUG_DLL);
	if (mod == NULL) {
		goto jini_err1;
	}
	romeo.mod = mod;
	d = dllproc;
	dterm = d + NELEMENTS(dllproc);
	while(d < dterm) {
		proc = (long)GetProcAddress(mod, d->symbol);
		if (proc == (long)NULL) {
			goto jini_err2;
		}
		*(long *)(((UINT8 *)&romeo) + (d->addr)) = proc;
		d++;
	}

	pciaddr = searchRomeo();
	if (pciaddr == (ULONG)0xffffffff) {
		goto jini_err2;
	}
	romeo.addr = romeo.read32(pciaddr, ROMEO_BASEADDRESS1);
	romeo.irq  = romeo.read32(pciaddr, ROMEO_PCIINTERRUPT) & 0xff;
	if (!romeo.addr) {
		goto jini_err2;
	}
	romeo.avail = ROMEO_AVAIL | ROMEO_YMF288;
	juliet_YMF288Reset();
	TRACEOUT(("ROMEO enable"));
	return(SUCCESS);

jini_err2:
	juliet_deinitialize();

jini_err1:
	return(FAILURE);
}

void juliet_deinitialize(void) {

	if (romeo.mod) {
		FreeLibrary(romeo.mod);
	}
	ZeroMemory(&romeo, sizeof(romeo));
	FillMemory(romeo.ttl, 8*4, 0x7f);
	romeo.psgmix = 0x3f;
}


// ---- YMF288部

static void YMF288A(UINT8 addr, UINT8 data) {

	while(romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) {
		Sleep(0);
	}
	romeo.out8(romeo.addr + ROMEO_YMF288ADDR1, addr);
	while(romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) {
		Sleep(0);
	}
	romeo.out8(romeo.addr + ROMEO_YMF288DATA1, data);
}

static void YMF288B(UINT8 addr, UINT8 data) {

	while(romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) {
		Sleep(0);
	}
	romeo.out8(romeo.addr + ROMEO_YMF288ADDR2, addr);
	while(romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) {
		Sleep(0);
	}
	romeo.out8(romeo.addr + ROMEO_YMF288DATA2, data);
}

static void setvolume(UINT8 ch, int vol) {

	void	(*send)(UINT8 addr, UINT8 data);
	UINT8	mask;
	UINT8	*pttl;
	int		ttl;

	send = (ch & 4)?YMF288B:YMF288A;
	mask = opmask[romeo.algo[ch & 7] & 7];
	pttl = romeo.ttl + ((ch & 4) << 2);
	ch = 0x40 + (ch & 3);
	do {
		if (mask & 1) {
			ttl = pttl[ch & 0x0f] & 0x7f;
			ttl -= vol;
			if (ttl < 0) {
				ttl = 0;
			}
			else if (ttl > 0x7f) {
				ttl = 0x7f;
			}
			send(ch, (UINT8)ttl);
		}
		ch += 4;
		mask >>= 1;
	} while(mask);
}


void juliet_YMF288Reset(void) {

	if (romeo.avail & ROMEO_YMF288) {
		romeo.out32(romeo.addr + ROMEO_YMF288CTRL, 0x00);
		Sleep(150);
		romeo.out32(romeo.addr + ROMEO_YMF288CTRL, 0x80);
		Sleep(150);
	}
}

BOOL juliet_YMF288IsEnable(void) {

	return((romeo.avail & ROMEO_YMF288) != 0);
}

BOOL juliet_YMF288IsBusy(void) {

	return((!(romeo.avail & ROMEO_YMF288)) ||
			((romeo.in8(romeo.addr + ROMEO_YMF288ADDR1) & 0x80) != 0));
}

void juliet_YMF288A(UINT addr, UINT8 data) {

	if (romeo.avail & ROMEO_YMF288) {
		if (addr == 0x07) {							// psg mix
			romeo.psgmix = data;
		}
		else if ((addr & (~15)) == 0x40) {			// ttl
			romeo.ttl[addr & 15] = data;
		}
		else if ((addr & (~3)) == 0xb0) {			// algorithm
			romeo.algo[addr & 3] = data;
		}
		YMF288A(addr, data);
	}
}

void juliet_YMF288B(UINT addr, UINT8 data) {

	if (romeo.avail & ROMEO_YMF288) {
		if ((addr & (~15)) == 0x40) {				// ttl
			romeo.ttl[0x10 + (addr & 15)] = data;
		}
		else if ((addr & (~3)) == 0xb0) {			// algorithm
			romeo.algo[4 + (addr & 3)] = data;
		}
		YMF288B(addr, data);
	}
}

void juliet_YMF288Enable(BOOL enable) {

	UINT8	ch;
	int		vol;

	if (romeo.avail & ROMEO_YMF288) {
		YMF288A(0x07, (enable)?romeo.psgmix:0x3f);
		vol = (enable)?0:-127;
		for (ch=0; ch<3; ch++) {
			setvolume(ch+0, vol);
			setvolume(ch+4, vol);
		}
	}
}

#endif

