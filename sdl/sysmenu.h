
enum {
	MENUTYPE_NORMAL	= 0
};


#ifdef __cplusplus
extern "C" {
#endif

BOOL sysmenu_create(void);
void sysmenu_destroy(void);

BOOL sysmenu_menuopen(UINT menutype, int x, int y);

#ifdef __cplusplus
}
#endif

