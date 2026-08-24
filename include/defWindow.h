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
#include <windows.h>
#include <windowsx.h>

#ifdef DIST
#define VGMain WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
#else
#define VGMain main()
#endif

#ifndef DEBUG
#undef __debugbreak() 
#endif

#endif // _WIN32

#ifndef __cplusplus
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <wchar.h>
#include <assert.h>
#include <string.h>
#else
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstdbool>
#include <cstddef>
#include <cstdarg>
#include <cwchar>
#include <cassert>
#include <cstring>
#endif

#define failed -1

#define MAX_STRLEN (size_t)256

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct CFWindow* CFWindow;

#define MK_RB 1 // rigtht mouse button
#define MK_MB 2 // middle mouse button
#define MK_LB 3 // left mouse button
#define MK_XB1 4 // X1 mouse button
#define MK_XB2 5 // X2 mouse button

	typedef void (*ONUSERDRAGFILECALLBACK)(CFWindow, const char** files, uint32_t count);
	typedef void (*ONUSERMOUSEMOVECALLBACK)(CFWindow, int x, int y, int key);
	typedef void (*ONUSERMOUSEWHEELCALLBACK)(CFWindow, int x, int y, int dir);
	typedef void (*ONUSERMOUSEDOWNCALLBACK)(CFWindow, int x, int y, int keyIndex);
	typedef void (*ONUSERMOUSEUPCALLBACK)(CFWindow, int x, int y, int keyIndex);
	typedef void (*ONUSERMOUSEDOUBLECLKCALLBACK)(CFWindow, int x, int y, int keyIndex);
	typedef void (*ONUSERKEYDOWNCALLBACK)(CFWindow, uintptr_t key);
	typedef void (*ONUSERKEYUPCALLBACK)(CFWindow, uintptr_t key);
	typedef void (*ONUSERRESIZECALLBACK)(CFWindow, int width, int height);
	typedef void (*ONUSERINITIALIZECALLBACK)(CFWindow);
	typedef void (*ONUSERRENDERCALLBACK)(CFWindow);
	typedef void (*ONUSERMOVECALLBACK)(CFWindow, int x, int y);
	typedef void (*ONUSERQUITCALLBACK)(CFWindow, int quitCode);
				 
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
	} WindowEventHandle;

	typedef struct
	{
		CFWindow window;
		enum
		{
			ONUSERDRAGFILE = 1,
			ONUSERMOUSEMOVE,
			ONUSERMOUSEWHEEL,
			ONUSERMOUSEDOWN,
			ONUSERMOUSEDOUBLECLK,
			ONUSERMOUSEUP,
			ONUSERKEYDOWN,
			ONUSERKEYUP,
			ONUSERRESIZE,
			ONUSERINITIALIZE,
			ONUSERRENDER,
			ONUSERMOVE,
			ONUSERQUIT,
		} type;
		union
		{
			struct 
			{
				const char** files;
				uint32_t count;
			} ONUSERDRAGFILE;
		
			struct 
			{
				int x;
				int y;
				int key;
			} ONUSERMOUSEMOVE;
		
			struct 
			{
				int x;
				int y;
				int dir;
			} ONUSERMOUSEWHEEL;
		
			struct 
			{
				int x;
				int y;
				int keyIndex;
			} ONUSERMOUSEDOWN;
		
			struct 
			{
				int x;
				int y;
				int keyIndex;
			} ONUSERMOUSEDOUBLECLK;
		
			struct 
			{
				int x;
				int y;
				int keyIndex;
			} ONUSERMOUSEUP;
		
			struct 
			{
				uintptr_t key;
			} ONUSERKEYDOWN;
		
			struct 
			{
				uintptr_t key;
			} ONUSERKEYUP;
		
			struct 
			{
				int width;
				int height;
			} ONUSERRESIZE;
		
			struct 
			{
				CFWindow window;
			} ONUSERINITIALIZE;
		
			struct 
			{
				CFWindow window;
			} ONUSERRENDER;

			struct 
			{
				int x;
				int y;
			} ONUSERMOVE;

			struct
			{
				int quitCode;
			} ONUSERQUIT;
		} event;
		WindowEventHandle* handler;
	} Event;

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
		WindowEventHandle		wndEvent;
		
		enum {
			WINDOWS,
			LINUX,
			NONSUPPORTED
		} System;
		struct _x11
		{
#ifdef __linux__
			unsigned long window;
			void* display;
			unsigned long atomDeleteWindow;
			struct 
			{
				/* fetching data */
				unsigned long XdndTypeList;
				unsigned long XdndSelection;

				/* client messages */
				unsigned long XdndEnter;
				unsigned long XdndPosition;
				unsigned long XdndStatus;
				unsigned long XdndLeave;
				unsigned long XdndDrop;
				unsigned long XdndFinished;

				/* actions */
				unsigned long XdndActionCopy;
				unsigned long XdndActionMove;
				unsigned long XdndActionLink;
				unsigned long XdndActionAsk;
				unsigned long XdndActionPrivate;

				unsigned long XtextUriList;
				unsigned long XtextPlain;

				unsigned long XdndAware;
			} xdnd;
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