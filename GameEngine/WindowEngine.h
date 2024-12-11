#pragma once
#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#endif

#include <stdint.h>

#define false 0
#define true 1
#define failed -1

//#if !defined(bool)
//#define bool uint8_t
//#endif

typedef struct
{
	void (*OnUserMouseMove)(int x, int y);
	void (*OnUserMouseWheel)(float pos);
	void (*OnUserMouseDown)(unsigned char right);
	void (*OnUserKeyDown)(unsigned char key);
	void (*OnUserKeyUp)(unsigned char key);
	void (*OnUserResize)(int width, int height);
	void (*OnUserInitialize)();
	void (*OnUserPaint)();
	void (*OnUserMove)(int x, int y);
	void (*OnUserQuit)(int code);
} WindowEvent;

typedef struct
{
	uint32_t width, height;
	uint8_t resizable;
	uint8_t child;
	uint8_t fullscreen;
	char* title;
	char* name;
	uint8_t caption;
} WindowInfo;

typedef struct
{
	struct _x11
	{
#ifdef __LINUX__
		int i = 0;
#else
		int null;
#endif
	} x11;
	struct _win32
	{
#ifdef _WIN32 || _WIN64
		HWND m_hWnd;
		char* windowClass;
#else
		int null;
#endif
	} win32;
	int width;
	int height;
} Window;