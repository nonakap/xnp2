/**
 * @file	dialog.h
 * @breif	ダイアログの宣言
 */

#pragma once

// d_about.cpp
void dialog_about(HWND hwndParent);

// d_bmp.cpp
void dialog_writebmp(HWND hWnd);

// d_clnd.cpp
void dialog_calendar(HWND hwndParent);

// d_config.cpp
void dialog_configure(HWND hwndParent);

// d_disk.cpp
void dialog_changefdd(HWND hWnd, REG8 drv);
void dialog_changehdd(HWND hWnd, REG8 drv);
void dialog_newdisk(HWND hWnd);

// d_font.cpp
void dialog_font(HWND hWnd);

// d_mpu98.cpp
void dialog_mpu98(HWND hwndParent);

// d_screen.cpp
void dialog_scropt(HWND hwndParent);

// d_serial.cpp
void dialog_serial(HWND hWnd);

// d_sound.cpp
void dialog_sndopt(HWND hwndParent);

// d_soundlog.cpp
void dialog_soundlog(HWND hWnd);
