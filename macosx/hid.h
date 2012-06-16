/*
 *  hid.h
 *  from X1EMx
 *
 *  Created by tk800 on Mon Apr 28 2003.
 *
 */

#include	"HID_Utilities_External.h"

typedef struct {
    pRecElement Element;
    pRecDevice Device;
} pRecSetting;

#ifdef __cplusplus
extern "C" {
#endif

bool getCurrentPosition(int num, bool getPositiveValue);
bool getCurrenButton(int num);

void hid_init(void);
void hid_clear(void);

void initTemporal(void);
void revertTemporal(void);
Boolean setJoypad(OSType type, char* name);
void changeJoyPadSetup (void );
bool getJoypadName(OSType type, char* name);

#ifdef __cplusplus
}
#endif

#define	hid_up		1
#define	hid_down	2
#define	hid_right	3
#define	hid_left	4
#define	hid_button1	5
#define	hid_button2	6

#define	JOYPAD_UP		'jpup'
#define	JOYPAD_DOWN		'jpdn'
#define	JOYPAD_LEFT		'jplf'
#define	JOYPAD_RIGHT	'jprt'
#define	JOYPAD_ABUTTON	'jpab'
#define	JOYPAD_BBUTTON	'jpbb'
