#include	"compiler.h"
#include	"mousemng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"keystat.h"


// �}�E�X ver0.28
// �ꕔ�̃Q�[���Ń}�E�X�f�[�^��؂�̂Ă�̂Ő���ȓ����Ȃ��Ȃ鎖������
// ������~���ׂ� �ϓ��Ɉړ��f�[�^���`���悤�ɂ��Ȃ���΂Ȃ�Ȃ�


void mouseif_sync(void) {

	// �O��̕���␳
	mouseif.x += mouseif.rx;
	mouseif.y += mouseif.ry;

	// ����̈ړ��ʂ��擾
	mouseif.b = mousemng_getstat(&mouseif.sx, &mouseif.sy, 1);
	if (np2cfg.KEY_MODE == 3) {
		mouseif.b &= keystat_getmouse(&mouseif.sx, &mouseif.sy);
	}
	mouseif.rx = mouseif.sx;
	mouseif.ry = mouseif.sy;

	mouseif.lastc = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
}

static void calc_mousexy(void) {

	UINT32	clk;
	SINT32	diff;

	clk = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
	diff = clk - mouseif.lastc;
	if (diff >= 2000) {
		SINT32 dx;
		SINT32 dy;
		mouseif.rapid ^= 0xa0;
		diff /= 1000;
		dx = mouseif.sx;
		if (dx > 0) {
			dx = dx * diff / mouseif.moveclock;
			if (dx > mouseif.rx) {
				dx = mouseif.rx;
			}
		}
		else if (dx < 0) {
			dx *= -1;
			dx = dx * diff / mouseif.moveclock;
			dx *= -1;
			if (dx < mouseif.rx) {
				dx = mouseif.rx;
			}
		}
		mouseif.x += dx;
		mouseif.rx -= dx;

		dy = mouseif.sy;
		if (dy > 0) {
			dy = dy * diff / mouseif.moveclock;
			if (dy > mouseif.ry) {
				dy = mouseif.ry;
			}
		}
		else if (dy < 0) {
			dy *= -1;
			dy = dy * diff / mouseif.moveclock;
			dy *= -1;
			if (dy < mouseif.ry) {
				dy = mouseif.ry;
			}
		}
		mouseif.y += dy;
		mouseif.ry -= dy;
		mouseif.lastc += diff * 1000;
	}
}

void mouseint(NEVENTITEM item) {

	if (!(mouseif.upd8255.portc & 0x10)) {
		pic_setirq(0x0d);
		nevent_set(NEVENT_MOUSE, mouseif.intrclock << mouseif.timing, mouseint, NEVENT_RELATIVE);
	}
}

static void setportc(REG8 value) {

	if ((value & 0x80) && (!(mouseif.upd8255.portc & 0x80))) {
		calc_mousexy();
		mouseif.latch_x = mouseif.x;
		mouseif.x = 0;
		mouseif.latch_y = mouseif.y;
		mouseif.y = 0;
		if (mouseif.latch_x > 127) {
			mouseif.latch_x = 127;
		}
		else if (mouseif.latch_x < -128) {
			mouseif.latch_x = -128;
		}
		if (mouseif.latch_y > 127) {
			mouseif.latch_y = 127;
		}
		else if (mouseif.latch_y < -128) {
			mouseif.latch_y = -128;
		}
	}
	if ((value ^ mouseif.upd8255.portc) & 0x10) {
		if (!(value & 0x10)) {
			if (!nevent_iswork(NEVENT_MOUSE)) {
				// ���荞�݂����Ƃ�
				pic_setirq(0x0d);
				nevent_set(NEVENT_MOUSE, mouseif.intrclock << mouseif.timing,
												mouseint, NEVENT_ABSOLUTE);
			}
		}
	}
	mouseif.upd8255.portc = (UINT8)value;
}


// ---- I/O

static void IOOUTCALL mouseif_o7fd9(UINT port, REG8 dat) {

	mouseif.upd8255.porta = dat;
	(void)port;
}

static void IOOUTCALL mouseif_o7fdb(UINT port, REG8 dat) {

	mouseif.upd8255.portb = dat;
	(void)port;
}

static void IOOUTCALL mouseif_o7fdd(UINT port, REG8 dat) {

	setportc(dat);
	(void)port;
}

static void IOOUTCALL mouseif_o7fdf(UINT port, REG8 dat) {

	REG8	portc;
	UINT	sft;

	portc = 0;
	if (dat & uPD8255_CTRL) {
		mouseif.upd8255.mode = (UINT8)dat;
#if 0
		pic_resetirq(0x0d);
		nevent_set(NEVENT_MOUSE, mouseif.intrclock << mouseif.timing,
												mouseint, NEVENT_ABSOLUTE);
#endif
	}
	else {
		sft = (dat >> 1) & 7;
		portc = mouseif.upd8255.portc;
		portc &= (~(1 << sft));
		portc |= (dat & 1) << sft;
	}
	setportc(portc);
	(void)port;
}

static REG8 IOINPCALL mouseif_i7fd9(UINT port) {

	SINT16	x;
	SINT16	y;
	REG8	ret;
	REG8	portc;

	if (mouseif.upd8255.mode & uPD8255_PORTA) {
		calc_mousexy();
		ret = mouseif.b;
		if (np2cfg.MOUSERAPID) {
			ret |= mouseif.rapid;
		}
		ret &= 0xf0;
#if 1
		ret |= 0x40;		// for shizuku
#else
		ret |= 0x50;
#endif
		portc = mouseif.upd8255.portc;
		if (portc & 0x80) {
			x = mouseif.latch_x;
			y = mouseif.latch_y;
		}
		else {
			x = mouseif.x;
			y = mouseif.y;
		}
		if (portc & 0x40) {
			x = y;
		}
		if (!(portc & 0x20)) {
			ret |= x & 0x0f;
		}
		else {
			ret |= (x >> 4) & 0x0f;
		}
		return(ret);
	}
	else {
		return(mouseif.upd8255.porta);
	}
	(void)port;
}

static REG8 IOINPCALL mouseif_i7fdb(UINT port) {

	if (mouseif.upd8255.mode & uPD8255_PORTB) {
		return(0x40);
	}
	else {
		return(mouseif.upd8255.portb);
	}
	(void)port;
}

static REG8 IOINPCALL mouseif_i7fdd(UINT port) {

	REG8	mode;
	REG8	ret;

	mode = mouseif.upd8255.mode;
	ret = mouseif.upd8255.portc;
	if (mode & uPD8255_PORTCH) {
		ret &= 0x1f;
	}
	if (mode & uPD8255_PORTCL) {
		ret &= 0xf0;
		ret |= 0x08;
		ret |= (pccore.dipsw[2] >> 5) & 0x04;
		ret |= ((~pccore.dipsw[0]) >> 4) & 0x03;
	}
	(void)port;
	return(ret);
}

static void IOOUTCALL mouseif_obfdb(UINT port, REG8 dat) {

	mouseif.timing = dat & 3;
	(void)port;
}


// ---- I/F

void mouseif_reset(const NP2CFG *pConfig) {

	ZeroMemory(&mouseif, sizeof(mouseif));
	mouseif.upd8255.porta = 0x00;
	mouseif.upd8255.portb = 0x00;
	mouseif.upd8255.portc = 0xf0;									// ver0.82
	mouseif.upd8255.mode = 0x93;
	mouseif.intrclock = pccore.realclock / 120;
	mouseif.moveclock = pccore.realclock / 56400;
	mouseif.latch_x = -1;
	mouseif.latch_y = -1;

	(void)pConfig;
}

void mouseif_bind(void) {

	iocore_attachout(0x7fd9, mouseif_o7fd9);
	iocore_attachout(0x7fdb, mouseif_o7fdb);
	iocore_attachout(0x7fdd, mouseif_o7fdd);
	iocore_attachout(0x7fdf, mouseif_o7fdf);
	iocore_attachinp(0x7fd9, mouseif_i7fd9);
	iocore_attachinp(0x7fdb, mouseif_i7fdb);
	iocore_attachinp(0x7fdd, mouseif_i7fdd);
	iocore_attachout(0xbfdb, mouseif_obfdb);
}

