#include	"compiler.h"
#include	"inputmng.h"


typedef struct {
	short	key;
	UINT	bit;
} KEYBIND;

typedef struct {
	UINT	kbs;
	KEYBIND	kb[32];
} INPMNG;

static	INPMNG	inpmng;

static const KEYBIND keybind[] = {
					{VK_UP,			KEY_UP},
					{VK_DOWN,		KEY_DOWN},
					{VK_LEFT,		KEY_LEFT},
					{VK_RIGHT,		KEY_RIGHT},
					{VK_RETURN,		KEY_ENTER},
					{VK_ESCAPE,		KEY_MENU},
					{VK_CONTROL,	KEY_SKIP}};


// ----

void inputmng_init(void) {

	INPMNG	*im;

	im = &inpmng;
	ZeroMemory(im, sizeof(INPMNG));
	im->kbs = NELEMENTS(keybind);
	CopyMemory(im->kb, keybind, sizeof(keybind));
}

void inputmng_keybind(short key, UINT bit) {

	INPMNG	*im;
	UINT	i;

	im = &inpmng;
	for (i=0; i<im->kbs; i++) {
		if (im->kb[i].key == key) {
			im->kb[i].bit = bit;
			return;
		}
	}
	if (im->kbs < NELEMENTS(im->kb)) {
		im->kb[im->kbs].key = key;
		im->kb[im->kbs].bit = bit;
		im->kbs++;
	}
}

UINT inputmng_getkey(short key) {

	INPMNG	*im;
	KEYBIND	*kb;
	int		kbs;

	im = &inpmng;
	kb = im->kb;
	kbs = im->kbs;
	while(kbs--) {
		if (kb->key == key) {
			return(kb->bit);
		}
		kb++;
	}
	return(0);
}

