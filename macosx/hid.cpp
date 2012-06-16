/*
 *  hid.cpp
 *  from X1EMx
 *
 *  Created by tk800 on Mon Apr 28 2003.
 *
 */

#include	"compiler.h"
#include	"np2.h"
#include	"hid.h"

#define hasAxis 1

static void getElementName (pRecElement pElement, char * cstr);
static pRecSetting readJoyConfig(const char * key);
static void initSetting (pRecSetting *setting);

static pRecSetting up;
static pRecSetting down;
static pRecSetting left;
static pRecSetting right;
static pRecSetting button1;
static pRecSetting button2;

static pRecSetting temporalUp;
static pRecSetting temporalDown;
static pRecSetting temporalLeft;
static pRecSetting temporalRight;
static pRecSetting temporalButton1;
static pRecSetting temporalButton2;


void hid_init(void) {
    pRecDevice pHIDDevice = NULL;
    
    initSetting(&up);
    initSetting(&down);
    initSetting(&left);
    initSetting(&right);
    initSetting(&button1);
    initSetting(&button2);
    
    HIDBuildDeviceList (0, 0);
    pHIDDevice = HIDGetFirstDevice ();
    if (pHIDDevice==NULL) {
        np2oscfg.JOYPAD1 = 0;
        return;
    }
    np2oscfg.JOYPAD1 = 1;
    
    up=readJoyConfig("HID UP");
    down=readJoyConfig("HID DOWN");
    left=readJoyConfig("HID LEFT");
    right=readJoyConfig("HID RIGHT");
    button1=readJoyConfig("HID BUTTON1");
    button2=readJoyConfig("HID BUTTON2");
}

void hid_clear(void) {
    HIDReleaseDeviceList ();
}

bool getCurrentPosition(int num, bool getPositiveValue) {

    pRecSetting	p;
    switch (num) {
        case hid_up:
            p = up;
            break;
        case hid_down:
            p = down;
            break;
        case hid_right:
            p = right;
            break;
        case hid_left:
            p = left;
            break;
        case hid_button1:
            p = button1;
            break;
        case hid_button2:
            p = button2;
            break;
        default:
            return false;
    }

    if ( p.Element == NULL )	return false;	// –³‚µ
    
    if (p.Element->type!=hasAxis) {
        return getCurrenButton(num);
    }

    int raw = (HIDCalibrateValue (HIDGetElementValue(p.Device, p.Element), p.Element) - p.Element->min) / 3;//X1EMx0.5i3‚ÅŠ„‚é‚±‚Æ‚É‚µ‚½j
    int middle = (p.Element->max - p.Element->min - 1) / 2 / 3;
    if ( raw == middle )
    {
        return false;	// ’†S
    }
    if (getPositiveValue && (raw > middle)) {
        return true;
    }
    else if (!getPositiveValue && (raw < middle)) {
        return true;
    }
    return false;
}

bool getCurrenButton(int num)
{
    pRecSetting	p;
    switch (num) {
        case hid_up:
            p = up;
            break;
        case hid_down:
            p = down;
            break;
        case hid_right:
            p = right;
            break;
        case hid_left:
            p = left;
            break;
        case hid_button1:
            p = button1;
            break;
        case hid_button2:
            p = button2;
            break;
        default:
            return false;
    }
    if ( p.Element == NULL )	return false;	// –³‚µ
    return (bool)HIDGetElementValue(p.Device, p.Element);
}

Boolean setJoypad(OSType type, char* name) {
    pRecElement element;
    pRecDevice device;

    if (HIDConfigureAction (&device, &element, 10.0)) // timeout ticks
    {
            switch (type) {
                case JOYPAD_UP:
                    up.Element=element;
                    up.Device=device;
                    break;
                case JOYPAD_DOWN:
                    down.Element=element;
                    down.Device=device;
                   break;
                case JOYPAD_LEFT:
                    left.Element=element;
                    left.Device=device;
                    break;
                case JOYPAD_RIGHT:
                    right.Element=element;
                    right.Device=device;
                    break;
                case JOYPAD_ABUTTON:
                    button1.Element=element;
                    button1.Device=device;
                   break;
                case JOYPAD_BBUTTON:
                    button2.Element=element;
                    button2.Device=device;
                    break;
                default:
                    return false;
                    break;
            }
            getElementName(element, name);
            return true;
    }

    return false;
}    

static void getElementName (pRecElement pElement, char * cstr) {

    if (*(pElement->name))
        BlockMoveData (pElement->name, cstr, 256);
    else // if no name
    {
        HIDGetUsageName (pElement->usagePage, pElement->usage, cstr);
        if (!*cstr) // if not usage
            sprintf (cstr, "No Name");
    }
}

void changeJoyPadSetup (void ) {
    bool syncFlag = false;
    syncFlag = HIDSaveElementPref(CFSTR("HID UP"), kCFPreferencesCurrentApplication, up.Device, up.Element);
    syncFlag = HIDSaveElementPref(CFSTR("HID DOWN"), kCFPreferencesCurrentApplication, down.Device, down.Element);
    syncFlag = HIDSaveElementPref(CFSTR("HID LEFT"), kCFPreferencesCurrentApplication, left.Device, left.Element);
    syncFlag = HIDSaveElementPref(CFSTR("HID RIGHT"), kCFPreferencesCurrentApplication, right.Device, right.Element);
    syncFlag = HIDSaveElementPref(CFSTR("HID BUTTON1"), kCFPreferencesCurrentApplication, button1.Device, button1.Element);
    syncFlag = HIDSaveElementPref(CFSTR("HID BUTTON2"), kCFPreferencesCurrentApplication, button2.Device, button2.Element);
}

static pRecSetting readJoyConfig(const char *key) {
    pRecDevice pDevice = NULL;
    pRecElement pElement = NULL;
    pRecSetting setting;

    bool result = false;
    CFStringRef keyString;
    keyString = CFStringCreateWithCString(NULL, key, CFStringGetSystemEncoding());
    result = HIDRestoreElementPref (keyString, kCFPreferencesCurrentApplication, &pDevice, &pElement);
    if (keyString) CFRelease(keyString);

    if (pDevice) {
        if (pDevice->vendorID == 0 && pDevice->productID == 0) result = false;
    }
    if (pDevice && pElement && result)
    {
        setting.Device = pDevice;
        setting.Element = pElement;
    }
    else {
        initSetting(&setting);
    }
    
    return setting;
}

bool getJoypadName(OSType type, char* name) {
    pRecSetting setting;
    initSetting(&setting);

        switch (type) {
            case JOYPAD_UP:
                setting=up;
                break;
            case JOYPAD_DOWN:
                setting=down;
                break;
            case JOYPAD_LEFT:
                setting=left;
                break;
            case JOYPAD_RIGHT:
                setting=right;
                break;
            case JOYPAD_ABUTTON:
                setting=button1;
                break;
            case JOYPAD_BBUTTON:
                setting=button2;
                break;
            default:
                return false;
                break;
        }
        if (setting.Element == NULL) {
            return false;
        }
        
        getElementName(setting.Element, name);
        return true;
}

static void initSetting (pRecSetting *setting) {
    (*setting).Element = NULL;
    (*setting).Device = NULL;
}

void initTemporal(void) {
    temporalUp=up;
    temporalDown=down;
    temporalLeft=left;
    temporalRight=right;
    temporalButton1=button1;
    temporalButton2=button2;
}

void revertTemporal(void) {
    up=temporalUp;
    down=temporalDown;
    left=temporalLeft;
    right=temporalRight;
    button1=temporalButton1;
    button2=temporalButton2;
}

void HIDReportErrorNum (char * strError, long numError)
{
    return;
}
void HIDReportError (char * strError)
{
    return;
}
