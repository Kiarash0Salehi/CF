#pragma once
#include "defWindow.h"

#ifndef _WIN32_WINDOW_H_
#define _WIN32_WINDOW_H_

#ifdef __cplusplus
extern "C"
{
#endif

int GFCreateWindow(WindowConfigure windowInfo, Window* window, WindowEvent* windowEvent);
void getWindowDimensions(Window* window, uint32_t* width, uint32_t* height);
void* getWindowUserDataPointer(Window* window);
void setWindowUserDataPointer(Window* window, void* data);
HWND getWin32Window(Window* window);
HINSTANCE getWin32Instance(Window* window);
bool doEvent(Window* window);

#ifdef __cplusplus
}
#endif

#endif // _WIN32_WINDOW_H_
