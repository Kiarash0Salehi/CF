/* 
* Copyright (c) 2024-2025 Ki(y)arash Salehi
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
* documentation files (the "Software"), to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
* to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of 
* the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
* THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#ifndef _DEF_WINDOW_H_
#define _DEF_WINDOW_H_

#ifdef _WIN32
#define WIN32
#include <windows.h>
#include <windowsx.h>

#ifdef DIST
#define VGMain WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
#else
#define VGMain main()
#endif

#ifndef DEBUG
#define __debugbreak() 
#endif

#endif // _WIN32

#ifndef __cplusplus
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <wchar.h>
#include <assert.h>
#else
#include <cstdint>
#include <cstdio>
#include <cstdint>
#include <cstdbool>
#include <cstddef>
#include <cstdarg>
#include <cwchar>
#include <cassert>
#endif

#define failed -1

#define MAX_STRLEN (size_t)256

#ifdef __cplusplus
extern "C"
{
#endif

	typedef uintptr_t Window;

#define MK_RB 1 // rigtht mouse button
#define MK_MB 2 // middle mouse button
#define MK_LB 3 // left mouse button
#define MK_XB1 4 // X1 mouse button
#define MK_XB2 5 // X2 mouse button

	typedef void (*ONUSERDRAGFILECALLBACK)(Window*, const char** files, uint32_t count);
	typedef void (*ONUSERMOUSEMOVECALLBACK)(Window*, int x, int y, int key);
	typedef void (*ONUSERMOUSEWHEELCALLBACK)(Window*, int x, int y, int dir);
	typedef void (*ONUSERMOUSEDOWNCALLBACK)(Window*, int x, int y, int keyIndex);
	typedef void (*ONUSERMOUSEUPCALLBACK)(Window*, int x, int y, int keyIndex);
	typedef void (*ONUSERMOUSEDOUBLECLKCALLBACK)(Window*, int x, int y, int keyIndex);
	typedef void (*ONUSERKEYDOWNCALLBACK)(Window*, uintptr_t key);
	typedef void (*ONUSERKEYUPCALLBACK)(Window*, uintptr_t key);
	typedef void (*ONUSERRESIZECALLBACK)(Window*, int width, int height);
	typedef void (*ONUSERINITIALIZECALLBACK)(Window*);
	typedef void (*ONUSERRENDERCALLBACK)(Window*);
	typedef void (*ONUSERMOVECALLBACK)(Window*, int x, int y);
	typedef void (*ONUSERQUITCALLBACK)(Window*, int quitCode);
				 
	typedef struct
	{
		ONUSERDRAGFILECALLBACK OnUserDragFile;
		ONUSERMOUSEMOVECALLBACK OnUserMouseMove;
		ONUSERMOUSEWHEELCALLBACK OnUserMouseWheel;
		ONUSERMOUSEDOWNCALLBACK OnUserMouseDown;
		ONUSERMOUSEDOUBLECLKCALLBACK OnUserMouseDoubleClick;
		ONUSERMOUSEUPCALLBACK OnUserMouseUp;
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
#define WC_FLAG_ACCEPTDRAGFILES (1 << 4)

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
		THREADCALLBACK callback;
		void* id;
		void* user_data;
	} thread;

	typedef union 
	{
		uint32_t 	errorIndex;
		char*		message;
		void*		user_data;
		bool		success : 1;
	} *ErrorMessage, _ErrorMessage;
	

#ifdef USE_WINDOW_STRUCT_
	typedef struct
	{
		void* 			user_data;
		thread 			this_thread;
		WindowConfigure wndcnfg;
		WindowEvent		wndEvent;
		
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
			HWND m_hWnd;
			char* windowClass;
			HINSTANCE hInstance;
#else
#define __NON_WINDOWS_OPRATING_SYSTEM_ 1
			int null;
#endif
		} win32;
	} WindowStruct;
#endif // USE_WINDOW_STRUCT_

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !_DEF_WINDOW_H_