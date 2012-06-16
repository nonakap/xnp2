
void winkbd_keydown(WPARAM wParam, LPARAM lParam);
void winkbd_keyup(WPARAM wParam, LPARAM lParam);
void winkbd_resetf12(void);

#if defined(WIN32_PLATFORM_PSPC)
void winkbd_bindinit(void);
void winkbd_bindcur(UINT type);
void winkbd_bindbtn(UINT type);
#endif

