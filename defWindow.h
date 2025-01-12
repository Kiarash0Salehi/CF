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

#endif

#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define failed -1

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct WindowEvent
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

	typedef struct WindowConfigure
	{
		uint32_t width;
		uint32_t height;
		uint8_t resizable;
		uint8_t child;
		uint8_t fullscreen;
		const char* title;
		const char* name;
		uint8_t caption;
	} WindowConfigure;

	typedef void (*THREADCALLBACK)(void*);

	typedef struct
	{
		void* id;
		THREADCALLBACK threadFunc;
		void* data;
	} thread;

	typedef struct Window
	{
		struct _x11
		{
#ifdef __linux__
			int i = 0;
#else
			int null;
#endif
		} x11;
		struct _win32
		{
#if defined(_WIN32) || defined(_WIN64)
			HWND m_hWnd;
			const char* windowClass;
#else
			int null;
#endif
		} win32;
		uint32_t width;
		uint32_t height;
	} Window;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !_DEF_WINDOW_H_