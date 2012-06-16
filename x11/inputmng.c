#include "compiler.h"

#if defined(USE_SYSMENU)

#include "inputmng.h"

static INPMNG inpmng;


void
inputmng_init(void)
{
	INPMNG *im = &inpmng;

	ZeroMemory(im, sizeof(INPMNG));
	im->kbs = nkeybind;
	if (im->kbs > 0) {
		if (im->kbs > MAX_KEYBIND)
			im->kbs = MAX_KEYBIND;
		CopyMemory(im->kb, keybind, sizeof(keybind[0]) * im->kbs);
	}
}

void
inputmng_keybind(short key, UINT bit)
{
	INPMNG *im = &inpmng;
	UINT i;

	for (i = 0; i < im->kbs; i++) {
		if (im->kb[i].key == key) {
			im->kb[i].bit = bit;
			return;
		}
	}
	if (im->kbs < nkeybind) {
		im->kb[im->kbs].key = key;
		im->kb[im->kbs].bit = bit;
		im->kbs++;
	}
}

UINT
inputmng_getkey(short key)
{
	INPMNG *im = &inpmng;
	KEYBIND	*kb;
	UINT kbs;

	for (kb = im->kb, kbs = im->kbs; kbs--; kb++) {
		if (kb->key == key)
			return(kb->bit);
	}
	return 0;
}

#endif	/* USE_SYSMENU */
