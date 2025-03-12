#pragma once
#include "defWindow.h"

#ifndef _WIN32_WINDOW_H_
#define _WIN32_WINDOW_H_

#ifdef __cplusplus
extern "C"
{
#endif

Window* GFCreateWindow(const WindowConfigure* windowInfo, const WindowEvent* windowEvent, Window* share);
void getWindowDimensions(Window* window, uint32_t* width, uint32_t* height);
void* getWindowUserDataPointer(Window* window);
void setWindowUserDataPointer(Window* window, void* data);
HWND getWin32Window(Window* window);
HINSTANCE getWin32Instance(Window* window);
bool pollEvent();

#ifdef __cplusplus
}
#endif

#endif // _WIN32_WINDOW_H_
