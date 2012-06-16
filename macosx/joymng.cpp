#include	"compiler.h"
#include	"np2.h"
#include	"hid.h"
#include	"joymng.h"

#define	JOY_LEFT_BIT	0x04
#define	JOY_RIGHT_BIT	0x08
#define	JOY_UP_BIT		0x01
#define	JOY_DOWN_BIT	0x02
#define	JOY_BTN1_BIT	0x40
#define	JOY_BTN2_BIT	0x20

static	BYTE	joyflag = 0xff;

void joy_flash(void) {

#if 0
	np2oscfg.JOYPAD1 &= 0x7f;
#endif
	joyflag = 0xff;
}

BYTE joymng_getstat(void) {

    if (np2oscfg.JOYPAD1 == 1) {
#if 0
        np2oscfg.JOYPAD1 |= 0x80;									// ver0.28
#endif
        joyflag = 0xff;
        if (getCurrentPosition(hid_left, false)) {
            joyflag &= ~JOY_LEFT_BIT;
        }
        else if (getCurrentPosition(hid_right, true)) {
            joyflag &= ~JOY_RIGHT_BIT;
        }
        if (getCurrentPosition(hid_up, false)) {
            joyflag &= ~JOY_UP_BIT;
        }
        else if (getCurrentPosition(hid_down, true)) {
            joyflag &= ~JOY_DOWN_BIT;
        }
        if (getCurrenButton(hid_button2)) {
            joyflag &= ~JOY_BTN2_BIT;							// ver0.28
        }
        if (getCurrenButton(hid_button1)) {
            joyflag &= ~JOY_BTN1_BIT;							// ver0.28
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

