#include	"compiler.h"
#include	"np2.h"
#include	"joymng.h"
#include	"menu.h"
#include	"pccore.h"


static	UINT8	joyflag = 0xff;

#define	JOY_LEFT_BIT	0x04
#define	JOY_RIGHT_BIT	0x08
#define	JOY_UP_BIT		0x01
#define	JOY_DOWN_BIT	0x02
#define	JOY_BTN1_BIT	0x10
#define	JOY_BTN2_BIT	0x20

static UINT8 joypad1btn[4];										// ver0.28

void joy_init(void) {

	JOYINFO		ji;
	int			i;

	if ((!joyGetNumDevs()) ||
		(joyGetPos(JOYSTICKID1, &ji) == JOYERR_UNPLUGGED)) {
//		xmenu_setjoystick(np2cfg.JOYSTICK | 2);					// ver0.28
		np2oscfg.JOYPAD1 |= 2;
	}
	for (i=0; i<4; i++) {										// ver0.28
		joypad1btn[i] = 0xff ^
				((np2oscfg.JOY1BTN[i] & 3) << ((np2oscfg.JOY1BTN[i] & 4)?4:6));
	}
}

void joy_flash(void) {

	np2oscfg.JOYPAD1 &= 0x7f;										// ver0.28
	joyflag = 0xff;
}

UINT8 joymng_getstat(void) {

	JOYINFO		ji;

	if ((np2oscfg.JOYPAD1 == 1) &&								// ver0.28
		(joyGetPos(JOYSTICKID1, &ji) == JOYERR_NOERROR)) {
		np2oscfg.JOYPAD1 |= 0x80;								// ver0.28
		joyflag = 0xff;
		if (ji.wXpos < 0x4000U) {
			joyflag &= ~JOY_LEFT_BIT;
		}
		else if (ji.wXpos > 0xc000U) {
			joyflag &= ~JOY_RIGHT_BIT;
		}
		if (ji.wYpos < 0x4000U) {
			joyflag &= ~JOY_UP_BIT;
		}
		else if (ji.wYpos > 0xc000U) {
			joyflag &= ~JOY_DOWN_BIT;
		}
		if (ji.wButtons & JOY_BUTTON1) {
			joyflag &= joypad1btn[0];							// ver0.28
		}
		if (ji.wButtons & JOY_BUTTON2) {
			joyflag &= joypad1btn[1];							// ver0.28
		}
		if (ji.wButtons & JOY_BUTTON3) {
			joyflag &= joypad1btn[2];							// ver0.28
		}
		if (ji.wButtons & JOY_BUTTON4) {
			joyflag &= joypad1btn[3];							// ver0.28
		}
	}
	return(joyflag);
}

// joyflag	bit:0		up
// 			bit:1		down
// 			bit:2		left
// 			bit:3		right
// 			bit:4		trigger1 (rapid)
// 			bit:5		trigger2 (rapid)
// 			bit:6		trigger1
// 			bit:7		trigger2

