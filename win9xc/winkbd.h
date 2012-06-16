
enum {
	KEY_PC98		= 0,
	KEY_KEY101		= 1,
	KEY_KEY106		= 2,
	KEY_TYPEMAX		= 3,
	KEY_UNKNOWN		= 0xff
};

#ifdef __cplusplus
extern "C" {
#endif

void winkbd_keydown(WPARAM wParam, LPARAM lParam);
void winkbd_keyup(WPARAM wParam, LPARAM lParam);
void winkbd_resetf12(void);

#ifdef __cplusplus
}
#endif

