#pragma once
#include "defWindow.h"

#ifndef _X11_WINDOW_H_
#define _X11_WINDOW_H_

#ifdef __cplusplus
extern "C"
{
#endif

	CFWindow CFCreateWindow(const WindowConfigure* windowInfo, const WindowEventHandle* windowEvent, CFWindow share);
	void getWindowDimensions(CFWindow window, uint32_t* width, uint32_t* height);
	void* getWindowUserDataPointer(CFWindow window);
	void setWindowUserDataPointer(CFWindow window, void* data);
	void setWindowTitle(CFWindow window, const char* const title);
	void setUserEventCallback(WindowEventHandle* event, CFWindow window);
	void setWindowSize(CFWindow window, int width, int height);
	WindowEventHandle* getWindowEventHandle(CFWindow window);
	unsigned long getX11Window(CFWindow window);
	void* getX11Display(CFWindow window);
	bool pollEvent();
	Event* CFGetWindowEvent();

#ifdef __cplusplus
}
#endif

#endif // _X11_WINDOW_H_