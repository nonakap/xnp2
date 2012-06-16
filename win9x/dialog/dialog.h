
LRESULT CALLBACK CfgDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK AboutDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK MidiDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK ClndDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

void dialog_scropt(HWND hWnd);
void dialog_sndopt(HWND hWnd);
void dialog_s98(HWND hWnd);
#if defined(SUPPORT_WAVEREC)
void dialog_waverec(HWND hWnd);
#endif
void dialog_serial(HWND hWnd);
void dialog_newdisk(HWND hWnd);
void dialog_changefdd(HWND hWnd, REG8 drv);
void dialog_changehdd(HWND hWnd, REG8 drv);
void dialog_font(HWND hWnd);
void dialog_writebmp(HWND hWnd);

