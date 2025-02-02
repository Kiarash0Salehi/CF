#pragma once

#ifndef _DEF_WINDOW_H_
#define _DEF_WINDOW_H_

#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>

#ifdef DIST
#define VGMain WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
#else
#define VGMain main()
#endif

#endif // _WIN32

#ifndef __cplusplus
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#else
#include <cstdint>
#include <cstdio>
#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstdarg>
#endif

#define failed -1



#ifdef __cplusplus
extern "C"
{
#endif

	typedef void* Window;

	typedef void (*ONUSERMOUSEMOVECALLBACK)(Window*, int x, int y);
	typedef void (*ONUSERMOUSEWHEELCALLBACK)(Window*, float);
	typedef void (*ONUSERMOUSEDOWNCALLBACK)(Window*, unsigned char);
	typedef void (*ONUSERKEYDOWNCALLBACK)(Window*, uintptr_t);
	typedef void (*ONUSERKEYUPCALLBACK)(Window*, uintptr_t);
	typedef void (*ONUSERRESIZECALLBACK)(Window*, int, int);
	typedef void (*ONUSERINITIALIZECALLBACK)(Window*);
	typedef void (*ONUSERRENDERCALLBACK)(Window*);
	typedef void (*ONUSERMOVECALLBACK)(Window*, int, int);
	typedef void (*ONUSERQUITCALLBACK)(Window*, int);
				 
	typedef struct WindowEvent
	{
		ONUSERMOUSEMOVECALLBACK OnUserMouseMove;
		ONUSERMOUSEWHEELCALLBACK OnUserMouseWheel;
		ONUSERMOUSEDOWNCALLBACK OnUserMouseDown;
		ONUSERKEYDOWNCALLBACK OnUserKeyDown;
		ONUSERKEYUPCALLBACK OnUserKeyUp;
		ONUSERRESIZECALLBACK OnUserResize;
		ONUSERINITIALIZECALLBACK OnUserInitialize;
		ONUSERRENDERCALLBACK OnUserRender;
		ONUSERMOVECALLBACK OnUserMove;
		ONUSERQUITCALLBACK OnUserQuit;
	} WindowEvent;

#define WC_SIZE_XCENTER (1 << 0)
#define WC_SIZE_YCENTER (1 << 1)

#define WC_FLAG_FULLSCREEN (1 << 0)
#define WC_FLAG_RESIZABLE (1 << 1)
#define WC_FLAG_CAPTION (1 << 2)
#define WC_FLAG_CHILD (1 << 3)


	typedef struct WindowConfigure
	{
		struct
		{
			uint32_t width;
			uint32_t height;
			uint32_t offsetX;
			uint32_t offsetY;
			uint8_t flags;
		} Size;
		uint16_t flags;
		const char* title;
		const char* appName;
	} WindowConfigure;

	typedef void (*THREADCALLBACK)(void*);

	typedef struct
	{
		void* id;
		THREADCALLBACK threadFunc;
		void* data;
	} thread;

	

#ifdef USE_WINDOW_STRUCT_
	typedef struct
	{
		enum {
			WINDOWS,
			LINUX,
			NONSUPPORTED
		} System;
		struct _x11
		{
#ifdef __linux__
			int i = 0;
#else
#define __NON_LINUX_OPRATING_SYSTEM_ 1
			int null;
#endif
		} x11;
		struct _win32
		{
#if defined(_WIN32) || defined(_WIN64)
			HWND parent;
			HWND m_hWnd;
			const char* windowClass;
			HINSTANCE hInstance;
			MSG message;
#else
#define __NON_WINDOWS_OPRATING_SYSTEM_ 1
			int null;
#endif
		} win32;
		void* user_data;
		thread this_thread;
		WindowConfigure wndcnfg;
	} WindowStruct;
#endif // USE_WINDOW_STRUCT_

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !_DEF_WINDOW_H_